/**
 * \file mech_repair.cpp
 *
 * \copyright 2020 John Harwell, All rights reserved.
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
#include "fordyca/events/mech_repair.hpp"

#include "fordyca/controller/foraging_controller.hpp"
#include "fordyca/fsm/foraging_signal.hpp"
#include "cosm/fsm/supervisor_fsm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, events, detail);

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void mech_repair::visit(controller::foraging_controller& controller) {
  controller.ndc_pusht();

  visit(*controller.supervisor());

  ER_INFO("Robot %s repaired", controller.GetId());
  controller.ndc_pop();
} /* visit() */


void mech_repair::visit(cfsm::supervisor_fsm& fsm) {
  fsm.inject_event(fsm::foraging_signal::ekMECHANICAL_REPAIR,
                   rpfsm::event_type::ekNORMAL);
} /* visit() */

NS_END(detail, events, fordyca);
