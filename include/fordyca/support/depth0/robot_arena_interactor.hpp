/**
 * \file depth0/robot_arena_interactor.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_DEPTH0_ROBOT_ARENA_INTERACTOR_HPP_
#define INCLUDE_FORDYCA_SUPPORT_DEPTH0_ROBOT_ARENA_INTERACTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/support/free_block_pickup_interactor.hpp"
#include "fordyca/support/nest_block_drop_interactor.hpp"
#include "fordyca/support/mpl/free_block_pickup.hpp"
#include "fordyca/support/mpl/nest_block_drop.hpp"
#include "fordyca/support/tv/env_dynamics.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support, depth0);

/*******************************************************************************
 * Classes
 ******************************************************************************/

/**
 * \class robot_arena_interactor
 * \ingroup support depth0
 *
 * \brief Handle's a robot's interactions with the environment on each timestep.
 *
 * Including:
 *
 * - Picking up a free block.
 * - Dropping a carried block in the nest.
 */
template <typename TController, typename TArenaMap>
class robot_arena_interactor final : public rer::client<robot_arena_interactor<TController,
                                                                               TArenaMap>> {
 public:
  robot_arena_interactor(TArenaMap* const map,
                         depth0_metrics_aggregator *const metrics_agg,
                         argos::CFloorEntity* const floor,
                         tv::env_dynamics* const envd)
      : ER_CLIENT_INIT("fordyca.support.depth0.robot_arena_interactor"),
        m_free_pickup(map,
                      floor,
                      envd->penalty_handler(tv::block_op_src::ekFREE_PICKUP)),
        m_nest_drop(map, metrics_agg, floor, envd) {}

  robot_arena_interactor(robot_arena_interactor&&) = default;

  /* Not copy-constructible/assignable by default. */
  robot_arena_interactor(const robot_arena_interactor&) = delete;
  robot_arena_interactor& operator=(const robot_arena_interactor&) = delete;

  /**
   * \brief The actual handling function for the interactions.
   *
   * \param controller The controller to handle interactions for.
   * \param t The current timestep.
   */
  interactor_status operator()(TController& controller,
                               const rtypes::timestep& t) {
    if (controller.is_carrying_block()) {
      return m_nest_drop(controller, t);
    } else { /* The foot-bot has no block item */
      return m_free_pickup(controller, t);
    }
  }

 private:
  /* clang-format off */
  free_block_pickup_interactor<TController,
                               mpl::free_block_pickup_map<controller::depth0::typelist>> m_free_pickup;
  nest_block_drop_interactor<TController,
                             mpl::nest_block_drop_map<controller::depth0::typelist>>     m_nest_drop;
  /* clang-format on */
};

NS_END(depth0, support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_DEPTH0_ROBOT_ARENA_INTERACTOR_HPP_ */
