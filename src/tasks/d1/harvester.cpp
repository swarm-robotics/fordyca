/**
 * \file harvester.cpp
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
#include "fordyca/tasks/d1/harvester.hpp"

#include "cosm/subsystem/sensing_subsystemQ3D.hpp"
#include "cosm/ta/config/task_alloc_config.hpp"

#include "fordyca/events/block_found.hpp"
#include "fordyca/events/block_vanished.hpp"
#include "fordyca/events/cache_found.hpp"
#include "fordyca/events/cache_vanished.hpp"
#include "fordyca/events/robot_cache_block_drop.hpp"
#include "fordyca/events/robot_cached_block_pickup.hpp"
#include "fordyca/events/robot_free_block_pickup.hpp"
#include "fordyca/fsm/d1/block_to_existing_cache_fsm.hpp"
#include "fordyca/tasks/argument.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, tasks, d1);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
harvester::harvester(const struct cta::config::task_alloc_config* config,
                     std::unique_ptr<cta::taskable> mechanism)
    : foraging_task(kHarvesterName, config, std::move(mechanism)),
      ER_CLIENT_INIT("fordyca.tasks.d1.harvester") {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void harvester::task_start(cta::taskable_argument* const) {
  foraging_signal_argument a(fsm::foraging_signal::ekACQUIRE_FREE_BLOCK);
  cta::polled_task::mechanism()->task_start(&a);
} /* task_start() */

double harvester::abort_prob_calc(void) {
  /*
   * Harvesters always have a small chance of aborting their task when not at a
   * task interface. Having the harvester task un-abortable until AFTER it
   * acquires a block can cause it to get stuck and not switch to another task
   * if it cannot find a block anywhere. See FORDYCA#232.
   */
  if (-1 == active_interface()) {
    return cta::abort_probability::kMIN_ABORT_PROB;
  } else {
    return executable_task::abort_prob();
  }
} /* calc_abort_prob() */

rtypes::timestep
harvester::interface_time_calc(size_t interface,
                               const rtypes::timestep& start_time) {
  ER_ASSERT(0 == interface, "Bad interface ID: %zu", interface);
  return current_time() - start_time;
} /* interface_time_calc() */

void harvester::active_interface_update(int) {
  auto* fsm = static_cast<fsm::d1::block_to_existing_cache_fsm*>(mechanism());

  if (fsm->goal_acquired() && fsm::foraging_transport_goal::ekEXISTING_CACHE ==
                                  fsm->block_transport_goal()) {
    if (interface_in_prog(0)) {
      interface_exit(0);
      interface_time_mark_finish(0);
      ER_DEBUG("Interface finished at timestep %zu", current_time().v());
    }
    ER_TRACE("Interface time: %zu", interface_time(0).v());
  } else if (fsm::foraging_transport_goal::ekEXISTING_CACHE ==
             fsm->block_transport_goal()) {
    if (!interface_in_prog(0)) {
      interface_enter(0);
      interface_time_mark_start(0);
      ER_DEBUG("Interface start at timestep %zu", current_time().v());
    }
  }
} /* active_interface_update() */

/*******************************************************************************
 * Event Handling
 ******************************************************************************/
void harvester::accept(events::detail::robot_cache_block_drop& visitor) {
  visitor.visit(*this);
}
void harvester::accept(events::detail::robot_free_block_pickup& visitor) {
  visitor.visit(*this);
}
void harvester::accept(events::detail::cache_vanished& visitor) {
  visitor.visit(*this);
}

void harvester::accept(events::detail::block_vanished& visitor) {
  visitor.visit(*this);
}

/*******************************************************************************
 * Block Acquisition Metrics
 ******************************************************************************/
RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    is_exploring_for_goal,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);
RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    is_vectoring_to_goal,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    goal_acquired,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    acquisition_goal,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    block_transport_goal,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    acquisition_loc3D,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    explore_loc3D,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    vector_loc3D,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

RCPPSW_WRAP_DEF_OVERRIDE(
    harvester,
    entity_acquired_id,
    *static_cast<fsm::d1::block_to_existing_cache_fsm*>(polled_task::mechanism()),
    const);

/*******************************************************************************
 * Task Metrics
 ******************************************************************************/
bool harvester::task_at_interface(void) const {
  auto* fsm = static_cast<fsm::d1::block_to_existing_cache_fsm*>(mechanism());
  return fsm::foraging_transport_goal::ekEXISTING_CACHE ==
         fsm->block_transport_goal();
} /* task_at_interface()() */

NS_END(d1, tasks, fordyca);
