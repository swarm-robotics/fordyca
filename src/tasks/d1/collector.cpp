/**
 * \file collector.cpp
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
#include "fordyca/tasks/d1/collector.hpp"

#include "cosm/subsystem/sensing_subsystemQ3D.hpp"

#include "fordyca/events/cache_vanished.hpp"
#include "fordyca/events/robot_cached_block_pickup.hpp"
#include "fordyca/events/robot_nest_block_drop.hpp"
#include "fordyca/fsm/d1/cached_block_to_nest_fsm.hpp"
#include "fordyca/fsm/foraging_acq_goal.hpp"
#include "fordyca/tasks/argument.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, tasks, d1);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
collector::collector(const cta::config::task_alloc_config* const config,
                     std::unique_ptr<cta::taskable> mechanism)
    : collector(config, kCollectorName, std::move(mechanism)) {}

collector::collector(const cta::config::task_alloc_config* const config,
                     const std::string& name,
                     std::unique_ptr<cta::taskable> mechanism)
    : foraging_task(name, config, std::move(mechanism)),
      ER_CLIENT_INIT("fordyca.tasks.d1.collector") {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void collector::task_start(cta::taskable_argument* const) {
  foraging_signal_argument a(fsm::foraging_signal::ekACQUIRE_CACHED_BLOCK);
  cta::polled_task::mechanism()->task_start(&a);
} /* task_start() */

double collector::abort_prob_calc(void) {
  /*
   * Collectors always have a small chance of aborting their task when not at a
   * task interface. Not strictly necessary at least for now, but it IS
   * necessary for foragers and so it seems like a good idea to add this to all
   * tasks.
   */
  if (-1 == active_interface()) {
    return cta::abort_probability::kMIN_ABORT_PROB;
  } else {
    return executable_task::abort_prob();
  }
} /* abort_prob_calc() */

rtypes::timestep
collector::interface_time_calc(size_t interface,
                               const rtypes::timestep& start_time) {
  ER_ASSERT(0 == interface, "Bad interface ID: %zu", interface);
  return current_time() - start_time;
} /* interface_time_calc() */

void collector::active_interface_update(int) {
  auto* fsm = static_cast<fsm::d1::cached_block_to_nest_fsm*>(mechanism());
  if (fsm::foraging_acq_goal::ekEXISTING_CACHE != fsm->acquisition_goal()) {
    return;
  }

  if (!fsm->goal_acquired()) {
    if (!interface_in_prog(0)) {
      interface_enter(0);
      interface_time_mark_start(0);
      ER_TRACE("Interface start at timestep %zu", current_time().v());
    }
  } else if (fsm->goal_acquired()) {
    if (interface_in_prog(0)) {
      interface_exit(0);
      interface_time_mark_finish(0);
      ER_TRACE("Interface finished at timestep %zu", current_time().v());
      ER_DEBUG("Interface time: %zu", interface_time(0).v());
    }
  }
} /* active_interface_update() */

/*******************************************************************************
 * Event Handling
 ******************************************************************************/
void collector::accept(events::detail::robot_cached_block_pickup& visitor) {
  visitor.visit(*this);
}
void collector::accept(events::detail::robot_nest_block_drop& visitor) {
  visitor.visit(*this);
}
void collector::accept(events::detail::cache_vanished& visitor) {
  visitor.visit(*this);
}

/*******************************************************************************
 * Block Acquisition Metrics
 ******************************************************************************/
RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    is_exploring_for_goal,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);
RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    is_vectoring_to_goal,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    goal_acquired,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    acquisition_goal,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    block_transport_goal,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    acquisition_loc3D,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    vector_loc3D,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    explore_loc3D,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    collector,
    entity_acquired_id,
    *static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism()),
    const);

/*******************************************************************************
 * Block Transport Metrics
 ******************************************************************************/
bool collector::is_phototaxiing_to_goal(bool include_ca) const {
  return static_cast<fsm::d1::cached_block_to_nest_fsm*>(polled_task::mechanism())
      ->is_phototaxiing_to_goal(include_ca);
} /* is_phototaxiing_to_goal() */

/*******************************************************************************
 * Task Metrics
 ******************************************************************************/
bool collector::task_at_interface(void) const {
  auto* fsm = static_cast<fsm::d1::cached_block_to_nest_fsm*>(mechanism());
  return !(fsm::foraging_transport_goal::ekNEST == fsm->block_transport_goal());
} /* task_at_interface() */

NS_END(d1, tasks, fordyca);
