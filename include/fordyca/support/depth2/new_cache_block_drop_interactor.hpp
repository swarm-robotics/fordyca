/**
 * \file new_cache_block_drop_interactor.hpp
 *
 * \copyright 2018 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_SUPPORT_DEPTH2_NEW_CACHE_BLOCK_DROP_INTERACTOR_HPP_
#define INCLUDE_FORDYCA_SUPPORT_DEPTH2_NEW_CACHE_BLOCK_DROP_INTERACTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <argos3/core/simulator/entity/floor_entity.h>

#include "fordyca/support/tv/env_dynamics.hpp"
#include "cosm/arena/operations/free_block_drop.hpp"
#include "fordyca/events/cache_proximity.hpp"
#include "fordyca/events/dynamic_cache_interactor.hpp"
#include "fordyca/support/depth2/dynamic_cache_manager.hpp"
#include "cosm/arena/caching_arena_map.hpp"
#include "fordyca/support/interactor_status.hpp"
#include "fordyca/events/robot_free_block_drop.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support, depth2);

/*******************************************************************************
 * Classes
 ******************************************************************************/
/**
 * \class new_cache_block_drop_interactor
 * \ingroup support depth2
 *
 * \brief Handles a robot's (possible) \ref free_block_drop event at a new cache
 * on a given timestep.
 */
template <typename T>
class new_cache_block_drop_interactor : public rer::client<new_cache_block_drop_interactor<T>> {
 public:
  new_cache_block_drop_interactor(carena::caching_arena_map* const map_in,
                                   argos::CFloorEntity* const floor_in,
                                  tv::env_dynamics* const envd,
                                   dynamic_cache_manager* const cache_manager)
      : ER_CLIENT_INIT("fordyca.support.depth2.new_cache_block_drop_interactor"),
        m_floor(floor_in),
        m_map(map_in),
        m_cache_manager(cache_manager),
        m_penalty_handler(envd->penalty_handler(
            tv::block_op_src::ekNEW_CACHE_DROP)) {}

  /**
   * \brief Interactors should generally NOT be copy constructable/assignable,
   * but is needed to use these classes with boost::variant.
   *
   * \todo Supposedly in recent versions of boost you can use variants with
   * move-constructible-only types (which is what this class SHOULD be), but I
   * cannot get this to work (the default move constructor needs to be noexcept
   * I think, and is not being interpreted as such).
   */
  new_cache_block_drop_interactor(
      const new_cache_block_drop_interactor& other) = default;
  new_cache_block_drop_interactor& operator=(
      const new_cache_block_drop_interactor&) = delete;

  /**
   * \brief The actual handling function for interactions.
   *
   * \param controller The controller to handle interactions for.
   * \param t   The current timestep.
   *
   * \return \c TRUE if a block was dropped in a new cache, \c FALSE otherwise.
   */
  interactor_status operator()(T& controller, const rtypes::timestep& t) {
    if (m_penalty_handler->is_serving_penalty(controller)) {
      if (m_penalty_handler->is_penalty_satisfied(controller, t)) {
        return finish_new_cache_block_drop(controller);
      }
    } else {
      /*
       * If we failed initialize a penalty because there is another
       * block/cache too close, then that probably means that the robot is not
       * aware of said block, so we should send an event to fix that. Better
       * to do this here AND after serving the penalty rather than always just
       * waiting until after the penalty is served to figure out that the
       * robot is too close to a block/cache.
       */
      auto status = m_penalty_handler->penalty_init(controller,
                                                   t,
                                                    tv::block_op_src::ekNEW_CACHE_DROP,
                                                    boost::make_optional(m_cache_manager->cache_proximity_dist()));
      if (tv::op_filter_status::ekCACHE_PROXIMITY == status) {
        auto prox_status = utils::new_cache_cache_proximity(controller,
                                                            *m_map,
                                                            m_cache_manager->cache_proximity_dist());
        /*
         * This is a check, not an assert, because we are not holding the cache
         * mutex between the first and second checks, and the cache could have
         * become depleted in between. See FORDYCA#633.
         */
        if (rtypes::constants::kNoUUID != prox_status.entity_id) {
          cache_proximity_notify(controller, prox_status);
        }
      }
    }
    return interactor_status::ekNO_EVENT;
  }

 private:
  void cache_proximity_notify(T& controller,
                              const utils::proximity_status_t& status) {
    ER_WARN("%s@%s cannot drop block in new cache: Cache%d@%s too close (%f <= %f)",
            controller.GetId().c_str(),
            controller.pos2D().to_str().c_str(),
            status.entity_id.v(),
            status.entity_loc.to_str().c_str(),
            status.distance.length(),
            m_cache_manager->cache_proximity_dist().v());
    /*
     * Because caches can be dynamically created/destroyed, we cannot rely on
     * the index position of cache i to be the same as its ID, so we need to
     * search for the correct cache.
     */
    m_map->cache_mtx()->lock();
    auto it = std::find_if(m_map->caches().begin(),
                           m_map->caches().end(),
                           [&](const auto& c) {
                             return c->id() == status.entity_id;
                           });
    /*
     * After verifying that (1) we are too close to a cache, (2) that cache
     * still exists (it could have been depleted by another robot between when
     * we checked the first time and now because we were not holding the cache
     * mutex), we visit the robot.
     *
     * This is necessary because if the cache we are referencing is depleted and
     * deleted while we are in the middle of updating the robot's DPO store, we
     * will get a segfault/memory error of some kind. See FORDYCA#633.
     */
    if (it != m_map->caches().end()) {
      events::cache_proximity_visitor prox_op(*it);
      prox_op.visit(controller);
    }
    m_map->cache_mtx()->unlock();
  }

  /**
   * \brief Handles handshaking between cache, robot, and arena if the robot is
   * has acquired a cache site and is looking to drop an object on it.
   */
  interactor_status finish_new_cache_block_drop(T& controller) {
    const ctv::temporal_penalty& p = m_penalty_handler->penalty_next();
    ER_ASSERT(p.controller() == &controller,
              "Out of order cache penalty handling");
    ER_ASSERT(nullptr != dynamic_cast<events::dynamic_cache_interactor*>(
        controller.current_task()), "Non-cache interface task!");
    ER_ASSERT(controller.current_task()->goal_acquired() &&
              fsm::foraging_acq_goal::ekNEW_CACHE == controller.current_task()->acquisition_goal(),
              "Controller not waiting for new cache block drop");
    auto status = utils::new_cache_cache_proximity(controller,
                                                       *m_map,
                                                       m_cache_manager->cache_proximity_dist());

    if (rtypes::constants::kNoUUID != status.entity_id) {
      /*
     * If there is another cache nearby that the robot is unaware of, and if
     * that cache is close enough to the robot's current location that a block
     * drop would result in the creation of a new cache which would overlap with
     * said cache, then abort the drop and tell the robot about the undiscovered
     * cache so that it will update its state and pick a different new cache.
     */
      ER_WARN("%s cannot drop block in new cache %s: Cache%d too close (%f <= %f)",
              controller.GetId().c_str(),
              controller.pos2D().to_str().c_str(),
              status.entity_id.v(),
              status.distance.length(),
              m_cache_manager->cache_proximity_dist().v());

      /*
       * We need to perform the proxmity check again after serving our block
       * drop penalty, because a cache might have been created nearby while we
       * were waiting, rendering our chosen location invalid.
       */
      auto it = std::find_if(m_map->caches().begin(),
                             m_map->caches().end(),
                             [&](const auto& c) {
                               return c->id() == status.entity_id; });
      ER_ASSERT(m_map->caches().end() != it,
                "FATAL: Cache%d does not exist?",
                status.entity_id.v());
      events::cache_proximity_visitor prox_op(*it);
      prox_op.visit(controller);
      return interactor_status::ekNO_EVENT;
    } else {
      perform_new_cache_block_drop(controller, p);
      m_penalty_handler->penalty_remove(p);
      ER_ASSERT(!m_penalty_handler->is_serving_penalty(controller),
                "Multiple instances of same controller serving cache penalty");
      return interactor_status::ekNEW_CACHE_BLOCK_DROP;
    }
  }

  /**
   * \brief Perform the actual dropping of a block in the cache once all
   * preconditions have been satisfied.
   */
  void perform_new_cache_block_drop(T& controller,
                                    const ctv::temporal_penalty& penalty) {
    auto loc = rmath::dvec2zvec(controller.pos2D(),
                                m_map->grid_resolution().v());

    caops::free_block_drop_visitor<crepr::base_block2D> adrop_op(
        m_map->blocks()[penalty.id().v()],
        loc,
        m_map->grid_resolution(),
        carena::arena_map_locking::ekNONE_HELD);
    events::robot_free_block_drop_visitor rdrop_op(controller.block_release(),
                                                   loc,
                                                   m_map->grid_resolution());

    controller.block_manip_recorder()->record(metrics::blocks::block_manip_events::ekFREE_DROP,
                                              penalty.penalty());

    rdrop_op.visit(controller);
    adrop_op.visit(*m_map);
    m_floor->SetChanged();
  }

  /* clang-format off */
  argos::CFloorEntity*  const         m_floor;
  carena::caching_arena_map* const    m_map;
  dynamic_cache_manager*const         m_cache_manager;
  tv::block_op_penalty_handler* const m_penalty_handler;
  /* clang-format on */
};

NS_END(depth2, support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_DEPTH2_NEW_CACHE_BLOCK_DROP_INTERACTOR_HPP_ */
