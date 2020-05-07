/**
 * \file free_block_pickup_interactor.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_FREE_BLOCK_PICKUP_INTERACTOR_HPP_
#define INCLUDE_FORDYCA_SUPPORT_FREE_BLOCK_PICKUP_INTERACTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <boost/none.hpp>

#include "cosm/tv/temporal_penalty.hpp"
#include "cosm/interactors/base_free_block_pickup.hpp"

#include "fordyca/metrics/blocks/block_manip_events.hpp"
#include "fordyca/fsm/foraging_acq_goal.hpp"
#include "fordyca/support/tv/block_op_src.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);

/*******************************************************************************
 * Classes
 ******************************************************************************/

/**
 * \class free_block_pickup_interactor
 * \ingroup support
 *
 * \brief Handle's a robot's (possible) free block pickup event on a given
 * timestep.
 */
template <typename TController, typename TControllerSpecMap>
class free_block_pickup_interactor final :

    public cinteractors::base_free_block_pickup<TController,
                                                                                       TControllerSpecMap> {
 public:
  using typename cinteractors::base_free_block_pickup<TController,
                                                      TControllerSpecMap>::arena_map_type;
  using typename cinteractors::base_free_block_pickup<TController,
                                                      TControllerSpecMap>::penalty_handler_type;

free_block_pickup_interactor(arena_map_type* const map,
                             argos::CFloorEntity* const floor,
                             penalty_handler_type* const handler)
      : cinteractors::base_free_block_pickup<TController, TControllerSpecMap>(map,
                                                                              floor,
                                                                              handler) {}

  free_block_pickup_interactor(free_block_pickup_interactor&&) = default;

  /* Not copy-constructible/assignable by default. */
  free_block_pickup_interactor(const free_block_pickup_interactor&) = delete;
  free_block_pickup_interactor& operator=(const free_block_pickup_interactor&) = delete;

  void robot_penalty_init(const TController& controller,
                          const rtypes::timestep& t,
                          penalty_handler_type* handler) override {
    handler->penalty_init(controller,
                          t,
                          tv::block_op_src::ekFREE_PICKUP,
                          boost::none);
  }

  bool robot_goal_acquired(const TController& controller) const override {
    return controller.goal_acquired() &&
        fsm::foraging_acq_goal::ekBLOCK == controller.acquisition_goal();
  }

  void robot_previsit_hook(TController& controller,
                           const ctv::temporal_penalty& penalty) const override {
    /*
     * Penalty served needs to be set here rather than in the free block pickup
     * event, because the penalty is generic, and the event handles concrete
     * classes--no clean way to mix the two.
     */
    controller.block_manip_recorder()->record(
        metrics::blocks::block_manip_events::ekFREE_PICKUP, penalty.penalty());
  }
};

NS_END(support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_FREE_BLOCK_PICKUP_INTERACTOR_HPP_ */
