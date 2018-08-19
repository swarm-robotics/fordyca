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
#include "fordyca/controller/depth1/foraging_controller.hpp"
#include <fstream>

#include "fordyca/controller/actuation_subsystem.hpp"
#include "fordyca/controller/cache_selection_matrix.hpp"
#include "fordyca/controller/block_selection_matrix.hpp"
#include "fordyca/controller/depth1/perception_subsystem.hpp"
#include "fordyca/controller/depth1/sensing_subsystem.hpp"
#include "fordyca/controller/depth1/tasking_initializer.hpp"
#include "fordyca/controller/saa_subsystem.hpp"
#include "fordyca/params/depth1/param_repository.hpp"
#include "fordyca/params/sensing_params.hpp"

#include "rcppsw/er/server.hpp"
#include "rcppsw/task_allocation/bifurcating_tdgraph_executive.hpp"
#include "rcppsw/task_allocation/executive_params.hpp"
#include "rcppsw/task_allocation/partitionable_task.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller, depth1);
using representation::occupancy_grid;

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
foraging_controller::foraging_controller(void)
    : depth0::stateful_foraging_controller(),
      m_cache_sel_matrix() {}

foraging_controller::~foraging_controller(void) = default;

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void foraging_controller::ControlStep(void) {
  perception()->update(depth0::stateful_foraging_controller::los());

  saa_subsystem()->actuation()->block_carry_throttle(is_carrying_block());
  saa_subsystem()->actuation()->throttling_update(
      saa_subsystem()->sensing()->tick());

  m_task_aborted = false;
  executive()->run();
} /* ControlStep() */

void foraging_controller::Init(ticpp::Element& node) {
  params::depth1::param_repository param_repo(client::server_ref());

  /*
   * Note that we do not call \ref stateful_foraging_controller::Init()--there
   * is nothing in there that we need.
   */
  base_foraging_controller::Init(node);
  ER_NOM("Initializing depth1 foraging controller");

  param_repo.parse_all(node);
#ifndef ER_NREPORT
  client::server_ptr()->log_stream() << param_repo;
#endif

  ER_ASSERT(param_repo.validate_all(),
            "FATAL: Not all task parameters were validated");

  /* Put in new depth1 sensors and perception, ala strategy pattern */
  saa_subsystem()->sensing(std::make_shared<depth1::sensing_subsystem>(
      param_repo.parse_results<struct params::sensing_params>(),
      &saa_subsystem()->sensing()->sensor_list()));

  perception(rcppsw::make_unique<perception_subsystem>(
      client::server_ref(),
      param_repo.parse_results<params::perception_params>(),
      GetId()));

  /*
   * Initialize tasking by overriding stateful controller executive via
   * strategy pattern.
   */
  auto* ogrid = param_repo.parse_results<params::occupancy_grid_params>();
  block_sel_matrix(rcppsw::make_unique<block_selection_matrix>(
      ogrid->nest,
      &ogrid->priorities));
  m_cache_sel_matrix = rcppsw::make_unique<cache_selection_matrix>(ogrid->nest);
  executive(tasking_initializer(client::server_ref(),
                                block_sel_matrix(),
                                m_cache_sel_matrix.get(),
                                saa_subsystem(),
                                perception())(&param_repo));
  ER_NOM("Depth1 foraging controller initialization finished");
  executive()->task_abort_notify(std::bind(&foraging_controller::task_abort_cb,
                                           this,
                                           std::placeholders::_1));
} /* Init() */

__rcsw_pure tasks::base_foraging_task* foraging_controller::current_task(void) {
  return dynamic_cast<tasks::base_foraging_task*>(executive()->current_task());
} /* current_task() */

__rcsw_pure const tasks::base_foraging_task* foraging_controller::current_task(
    void) const {
  return const_cast<foraging_controller*>(this)->current_task();
} /* current_task() */

void foraging_controller::task_abort_cb(const ta::polled_task*) {
  m_task_aborted = true;
} /* task_abort_cb() */

/*
 * Work around argos' REGISTER_LOOP_FUNCTIONS() macro which does not support
 * namespaces, so if you have two classes of the same name in two different
 * namespaces, the macro will create the same class definition, giving a linker
 * error.
 */
using namespace argos;
using depth1_foraging_controller = foraging_controller;

REGISTER_CONTROLLER(depth1_foraging_controller,
                    "depth1_foraging_controller"); // NOLINT

NS_END(depth1, controller, fordyca);
