/**
 * @file arena_interactor.hpp
 *
 * @copyright 2017 John Harwell, All rights reserved.
 *
 * This file is part of FORDYCA.
 *
 * FORDYCA is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * FORDYCA is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * FORDYCA.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef INCLUDE_FORDYCA_SUPPORT_DEPTH0_ARENA_INTERACTOR_HPP_
#define INCLUDE_FORDYCA_SUPPORT_DEPTH0_ARENA_INTERACTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <argos3/core/simulator/entity/floor_entity.h>
#include <string>

#include "fordyca/events/free_block_pickup.hpp"
#include "fordyca/events/nest_block_drop.hpp"
#include "fordyca/representation/arena_map.hpp"
#include "fordyca/representation/line_of_sight.hpp"
#include "fordyca/support/loop_functions_utils.hpp"
#include "rcppsw/er/server.hpp"
#include "fordyca/metrics/fsm/goal_acquisition_metrics.hpp"
#include "fordyca/fsm/block_transporter.hpp"
#include "fordyca/support/depth0/stateless_metrics_aggregator.hpp"
#include "fordyca/support/block_op_penalty_handler.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);

using acquisition_goal_type = metrics::fsm::goal_acquisition_metrics::goal_type;
using transport_goal_type = fsm::block_transporter::goal_type;

NS_START(depth0);

/*******************************************************************************
 * Classes
 ******************************************************************************/

/**
 * @class arena_interactor
 * @ingroup support
 *
 * @brief Handle's a robot's interactions with the environment on each timestep:
 *
 * - Picking up a free block (possibly with penalty).
 * - Dropping a carried block in the nest (possibly with a penalty).
 */
template <typename T>
class arena_interactor : public rcppsw::er::client {
 public:
  arena_interactor(std::shared_ptr<rcppsw::er::server> server,
                   representation::arena_map* const map,
                   stateless_metrics_aggregator *const metrics_agg,
                   argos::CFloorEntity* const floor,
                   const ct::waveform_params* const block_penalty)
      : client(server),
        m_floor(floor),
        m_metrics_agg(metrics_agg),
        m_map(map),
        m_block_penalty_handler(server, map, block_penalty) {}

  arena_interactor& operator=(const arena_interactor& other) = delete;
  arena_interactor(const arena_interactor& other) = delete;

  /**
   * @brief The actual handling function for the interactions.
   *
   * @param controller The controller to handle interactions for.
   * @param timestep The current timestep.
   */
  void operator()(T& controller, uint timestep) {
    if (controller.is_carrying_block()) {
      handle_nest_block_drop(controller, timestep);
    } else { /* The foot-bot has no block item */
      handle_free_block_pickup(controller, timestep);
    }
  }

 protected:
  void handle_free_block_pickup(T& controller, uint timestep) {
    if (m_block_penalty_handler.is_serving_penalty(controller)) {
      if (m_block_penalty_handler.penalty_satisfied(controller,
                                                    timestep)) {
        finish_free_block_pickup(controller, timestep);
      }
    } else {
      m_block_penalty_handler.penalty_init(controller, timestep);
   }
  }

  void handle_nest_block_drop(T& controller, uint timestep) {
    if (m_block_penalty_handler.is_serving_penalty(controller)) {
      if (m_block_penalty_handler.penalty_satisfied(controller,
                                                    timestep)) {
        finish_nest_block_drop(controller, timestep);
      }
    } else {
      m_block_penalty_handler.penalty_init(controller, timestep);
    }
  }

  /**
   * @brief Determine if a robot is waiting to pick up a free block, and if it
   * is actually on a free block, send it the \ref free_block_pickup event.
   */
  void finish_free_block_pickup(T& controller, uint timestep) {
    ER_ASSERT(controller.goal_acquired() &&
              acquisition_goal_type::kBlock == controller.acquisition_goal(),
              "FATAL: Controller not waiting for free block pickup");
    /*
     * More than 1 robot can pick up a block in a timestep, so we have to
     * search for this robot's controller
     */
    const temporal_penalty<T>& p = *m_block_penalty_handler.find(controller);

    /* Check whether the foot-bot is actually on a block */
    perform_free_block_pickup(controller, p, timestep);
  }

  /**
   * @brief Perform the actual picking up of a free block once all
   * preconditions have been satisfied.
   */
  void perform_free_block_pickup(T& controller,
                                 const temporal_penalty<T>& penalty,
                                 uint timestep) {
    events::free_block_pickup pickup_op(rcppsw::er::g_server,
                                        m_map->blocks()[penalty.id()],
                                        utils::robot_id(controller),
                                        timestep);
    controller.block()->pickup_event(true);
    m_metrics_agg->collect_from_block(controller.block().get());
    controller.block()->pickup_event(false);

    /*
     * Penalty served needs to be set here rather than in the free block pickup
     * event, because the penalty is generic, and the event handles concrete
     * classes--no clean way to mix the two.
     */
    controller.penalty_served(penalty.penalty());
    controller.visitor::template visitable_any<T>::accept(pickup_op);
    m_map->accept(pickup_op);
    controller.free_pickup_event(false);

    /* The floor texture must be updated */
    m_floor->SetChanged();
  }

  /**
   * @brief Determine if a robot is waiting to drop a block in the nest, and if
   * so send it the \ref nest_block_drop event.
   */
  void finish_nest_block_drop(T& controller, uint timestep) {
    ER_ASSERT(controller.in_nest() &&
              transport_goal_type::kNest == controller.block_transport_goal(),
              "FATAL: Controller not in nest/does not have nest as goal");
    /*
     * More than 1 robot can drop a block in a timestep, so we have to
     * search for this robot's controller.
     */
    const temporal_penalty<T>& p = *m_block_penalty_handler.find(controller);

    perform_nest_block_drop(controller, p, timestep);
  }

  /**
   * @brief Perform the actual picking up of a free block once all
   * preconditions have been satisfied.
   */
  void perform_nest_block_drop(T& controller,
                               const temporal_penalty<T>& penalty,
                               uint timestep) {
    /*
     * Gather block transport metrics before event processing and they get
     * reset.
     */
    controller.block()->nest_drop_time(timestep);

    /*
     * We are clearly performing a block drop in the nest, so mark it as
     * such. We have to do this manually, rather than letting it happen in
     * the nest block drop event, as we have already gathered metrics for
     * this timestep for this block at that point.
     */
    controller.block()->drop_event(true);
    m_metrics_agg->collect_from_block(controller.block().get());
    controller.block()->drop_event(false);

    /*
     * Penalty served needs to be set here rather than in the free block pickup
     * event, because the penalty is generic, and the event handles concrete
     * classes--no clean way to mix the two.
     */
    controller.penalty_served(penalty.penalty());

    events::nest_block_drop drop_op(rcppsw::er::g_server,
                                    controller.block(),
                                    timestep);

    /* Update arena map state due to a block nest drop */
    m_map->accept(drop_op);

    /* Actually drop the block */
    controller.visitor::template visitable_any<T>::accept(drop_op);

    /* The floor texture must be updated */
    m_floor->SetChanged();
  }

  representation::arena_map* map(void) { return m_map; }
  const representation::arena_map* map(void) const { return m_map; }
  const argos::CFloorEntity* floor(void) const { return m_floor; }
  argos::CFloorEntity* floor(void) { return m_floor; }

 private:
  // clang-format off
  argos::CFloorEntity*             const m_floor;
  stateless_metrics_aggregator*    const m_metrics_agg;
  representation::arena_map* const       m_map;
  block_op_penalty_handler<T>            m_block_penalty_handler;
  // clang-format on
};

NS_END(depth0, support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_DEPTH0_ARENA_INTERACTOR_HPP_ */
