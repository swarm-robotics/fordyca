/**
v * \file generalist.cpp
 *
 * \copyright 2017 John Harwell, All rights reserved.
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/tasks/d0/generalist.hpp"

#include "cosm/subsystem/sensing_subsystemQ3D.hpp"

#include "fordyca/events/block_vanished.hpp"
#include "fordyca/events/robot_free_block_pickup.hpp"
#include "fordyca/events/robot_nest_block_drop.hpp"
#include "fordyca/fsm/d0/free_block_to_nest_fsm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, tasks, d0);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
generalist::generalist(const cta::config::task_alloc_config* const config,
                       std::unique_ptr<cta::taskable> mechanism)
    : foraging_task(kGeneralistName, config, std::move(mechanism)) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
double generalist::abort_prob_calc(void) {
  return executable_task::abort_prob();
} /* abort_prob_calc() */

rtypes::timestep generalist::current_time(void) const {
  return dynamic_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism())
      ->sensing()
      ->tick();
} /* current_time() */

/*******************************************************************************
 * Event Handling
 ******************************************************************************/
void generalist::accept(events::detail::robot_nest_block_drop& visitor) {
  visitor.visit(*this);
}
void generalist::accept(events::detail::robot_free_block_pickup& visitor) {
  visitor.visit(*this);
}
void generalist::accept(events::detail::block_vanished& visitor) {
  visitor.visit(*this);
}

/*******************************************************************************
 * Goal Acquisition Metrics
 ******************************************************************************/
RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    is_exploring_for_goal,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);
RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    is_vectoring_to_goal,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    goal_acquired,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    acquisition_goal,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    block_transport_goal,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    acquisition_loc3D,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    explore_loc3D,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    vector_loc3D,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    generalist,
    entity_acquired_id,
    *static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

/*******************************************************************************
 * Block Transport Metrics
 ******************************************************************************/
bool generalist::is_phototaxiing_to_goal(bool include_ca) const {
  return static_cast<fsm::d0::free_block_to_nest_fsm*>(polled_task::mechanism())
      ->is_phototaxiing_to_goal(include_ca);
} /* is_phototaxiing_to_goal() */

NS_END(d0, tasks, fordyca);
