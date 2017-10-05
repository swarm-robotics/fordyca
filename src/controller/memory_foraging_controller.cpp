/**
 * @file memory_foraging_controller.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/controller/memory_foraging_controller.hpp"
#include "fordyca/params/memory_foraging_repository.hpp"
#include "fordyca/representation/line_of_sight.hpp"
#include "fordyca/events/block_pickup.hpp"
#include "fordyca/params/fsm_params.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller);

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void memory_foraging_controller::ControlStep(void) {
  /*
   * Update the perceived arena map with the current line-of-sight, and update
   * the relevance of information within it.
   */
  if (m_map->event_new_los(sensors()->los())) {
    publish_fsm_event(foraging_signal::BLOCK_LOCATED);
    m_map->update_density();
  } else {
    m_map->update_density();
    m_fsm->run();
  }
} /* ControlStep() */

void memory_foraging_controller::publish_fsm_event(foraging_signal::type signal) {
  switch (signal) {
    case foraging_signal::BLOCK_LOCATED:
      m_fsm->inject_event(foraging_signal::BLOCK_LOCATED,
                          rcppsw::patterns::state_machine::event_type::NORMAL);
      break;
    case foraging_signal::BLOCK_ACQUIRED:
      m_fsm->inject_event(foraging_signal::BLOCK_ACQUIRED,
                          rcppsw::patterns::state_machine::event_type::NORMAL);
      break;
    default:
      break;
  }
} /* publish_event() */

void memory_foraging_controller::Init(argos::TConfigurationNode& node) {
  params::memory_foraging_repository param_repo;

  random_foraging_controller::Init(node);
  ER_NOM("Initializing memory_foraging controller");
  param_repo.parse_all(node);
  param_repo.show_all(server_handle()->log_stream());

  m_map.reset(new representation::perceived_arena_map(
      server(),
      static_cast<const struct params::perceived_grid_params*>(
          param_repo.get_params("perceived_grid")),
      GetId()));

  m_fsm.reset(
      new fsm::memory_foraging_fsm(static_cast<const struct params::fsm_params*>(
          param_repo.get_params("fsm")),
                       server(),
                       sensors(),
                       actuators(),
                       m_map));
ER_NOM("memory_foraging controller initialization finished");
} /* Init() */

/*
 * This statement notifies ARGoS of the existence of the controller.  It binds
 * the class passed as first argument to the string passed as second argument,
 * which is then available in the XML.
 */
using namespace argos;
REGISTER_CONTROLLER(memory_foraging_controller, "memory_foraging_controller")

NS_END(controller, fordyca);
