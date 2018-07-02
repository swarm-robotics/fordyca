/**
 * @file foraging_controller.cpp
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
#include "fordyca/controller/depth0/stateful_foraging_controller.hpp"
#include <fstream>

#include "fordyca/controller/actuation_subsystem.hpp"
#include "fordyca/controller/base_perception_subsystem.hpp"
#include "fordyca/controller/depth0/sensing_subsystem.hpp"
#include "fordyca/controller/depth0/stateful_tasking_initializer.hpp"
#include "fordyca/controller/saa_subsystem.hpp"
#include "fordyca/params/depth0/stateful_param_repository.hpp"
#include "fordyca/params/sensing_params.hpp"

#include "rcppsw/er/server.hpp"
#include "rcppsw/task_allocation/executive_params.hpp"
#include "rcppsw/task_allocation/polled_executive.hpp"
#include "rcppsw/task_allocation/task_decomposition_graph.hpp"
#include "rcppsw/task_allocation/task_params.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller, depth0);
namespace ta = rcppsw::task_allocation;

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
stateful_foraging_controller::stateful_foraging_controller(void)
    : stateless_foraging_controller(),
      m_light_loc(),
      m_perception(),
      m_executive() {}

stateful_foraging_controller::~stateful_foraging_controller(void) = default;

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void stateful_foraging_controller::perception(
    std::unique_ptr<base_perception_subsystem> perception) {
  m_perception = std::move(perception);
}
__rcsw_pure std::shared_ptr<tasks::base_foraging_task> stateful_foraging_controller::
    current_task(void) const {
  return std::dynamic_pointer_cast<tasks::base_foraging_task>(
      m_executive->current_task());
} /* current_task() */

__rcsw_pure const representation::line_of_sight* stateful_foraging_controller::los(
    void) const {
  return stateful_sensors()->los();
}
void stateful_foraging_controller::los(
    std::unique_ptr<representation::line_of_sight>& new_los) {
  stateful_sensors()->los(new_los);
}

__rcsw_pure const depth0::sensing_subsystem* stateful_foraging_controller::
    stateful_sensors(void) const {
  return static_cast<const depth0::sensing_subsystem*>(
      saa_subsystem()->sensing().get());
}

__rcsw_pure depth0::sensing_subsystem* stateful_foraging_controller::stateful_sensors(
    void) {
  return static_cast<depth0::sensing_subsystem*>(
      saa_subsystem()->sensing().get());
}

void stateful_foraging_controller::ControlStep(void) {
  /*
   * Update the robot's model of the world with the current line-of-sight, and
   * update the relevance of information within it. Then, you can run the main
   * FSM loop.
   */
  m_perception->update(stateful_sensors()->los());

  saa_subsystem()->actuation()->block_throttle_toggle(is_carrying_block());
  saa_subsystem()->actuation()->block_throttle_update();

  m_executive->run();
} /* ControlStep() */

void stateful_foraging_controller::Init(ticpp::Element& node) {
  params::depth0::stateful_param_repository param_repo(server_ref());

  /*
   * Note that we do not call \ref stateless_foraging_controller::Init()--there
   * is nothing in there that we need.
   */
  base_foraging_controller::Init(node);

  ER_NOM("Initializing stateful_foraging controller");

  /* parse and validate parameters */
  param_repo.parse_all(node);
  server_ptr()->log_stream() << param_repo;
  ER_ASSERT(param_repo.validate_all(),
            "FATAL: Not all parameters were validated");

  /* initialize subsystems and perception */
  m_perception = rcppsw::make_unique<base_perception_subsystem>(
      client::server_ref(),
      param_repo.parse_results<params::perception_params>(),
      GetId());

  saa_subsystem()->sensing(std::make_shared<depth0::sensing_subsystem>(
      param_repo.parse_results<struct params::sensing_params>(),
      &saa_subsystem()->sensing()->sensor_list()));

  /* initialize tasking */
  m_executive = stateful_tasking_initializer(client::server_ref(),
                                             saa_subsystem(),
                                             perception())(&param_repo);

  ER_NOM("stateful_foraging controller initialization finished");
} /* Init() */

void stateful_foraging_controller::Reset(void) {
  stateless_foraging_controller::Reset();
  m_perception->reset();
} /* Reset() */

FSM_WRAPPER_DEFINE_PTR(transport_goal_type,
                       stateful_foraging_controller,
                       block_transport_goal,
                       current_task());

/*******************************************************************************
 * FSM Metrics
 ******************************************************************************/
FSM_WRAPPER_DEFINE_PTR(bool,
                       stateful_foraging_controller,
                       is_avoiding_collision,
                       current_task());
FSM_WRAPPER_DEFINE_PTR(bool,
                       stateful_foraging_controller,
                       is_exploring_for_goal,
                       current_task());

FSM_WRAPPER_DEFINE_PTR(bool,
                       stateful_foraging_controller,
                       is_vectoring_to_goal,
                       current_task());

FSM_WRAPPER_DEFINE_PTR(bool,
                       stateful_foraging_controller,
                       goal_acquired,
                       current_task());

FSM_WRAPPER_DEFINE_PTR(acquisition_goal_type,
                       stateful_foraging_controller,
                       acquisition_goal,
                       current_task());

/*******************************************************************************
 * World Model Metrics
 ******************************************************************************/
uint stateful_foraging_controller::cell_state_inaccuracies(uint state) const {
  return m_perception->cell_state_inaccuracies(state);
} /* cell_state_inaccuracies() */

using namespace argos;
REGISTER_CONTROLLER(stateful_foraging_controller,
                    "stateful_foraging_controller"); // NOLINT

NS_END(depth0, controller, fordyca);
