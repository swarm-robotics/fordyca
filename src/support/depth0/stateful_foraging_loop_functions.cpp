/**
 * @file stateful_foraging_loop_functions.cpp
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
#include "fordyca/support/depth0/stateful_foraging_loop_functions.hpp"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>

#include "fordyca/controller/depth0/stateful_foraging_controller.hpp"
#include "fordyca/controller/depth1/foraging_controller.hpp"
#include "fordyca/events/free_block_pickup.hpp"
#include "fordyca/events/nest_block_drop.hpp"
#include "fordyca/params/loop_function_repository.hpp"
#include "fordyca/params/loop_functions_params.hpp"
#include "fordyca/params/output_params.hpp"
#include "fordyca/representation/line_of_sight.hpp"
#include "fordyca/support/loop_functions_utils.hpp"
#include "rcppsw/er/server.hpp"

#include "fordyca/metrics/block_metrics_collector.hpp"
#include "fordyca/metrics/fsm/distance_metrics_collector.hpp"
#include "fordyca/metrics/fsm/stateful_metrics_collector.hpp"
#include "fordyca/metrics/fsm/stateless_metrics_collector.hpp"
#include "fordyca/metrics/fsm/stateless_metrics.hpp"
#include "fordyca/tasks/foraging_task.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support, depth0);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
stateful_foraging_loop_functions::stateful_foraging_loop_functions(void)
    : m_collector() {}
stateful_foraging_loop_functions::~stateful_foraging_loop_functions(void) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void stateful_foraging_loop_functions::Init(argos::TConfigurationNode& node) {
  stateless_foraging_loop_functions::Init(node);

  ER_NOM("Initializing depth0_foraging loop functions");
  params::loop_function_repository repo;

  repo.parse_all(node);

  /* initialize stat collecting */
  auto* p_output = static_cast<const struct params::output_params*>(
      repo.get_params("output"));
  m_collector.reset(new metrics::fsm::stateful_metrics_collector(
      metrics_path() + "/" + p_output->metrics.stateful_fname,
      p_output->metrics.collect_cum,
      p_output->metrics.collect_interval));
  m_collector->reset();

  /* configure robots */
  for (auto& entity_pair : GetSpace().GetEntitiesByType("foot-bot")) {
    argos::CFootBotEntity& robot =
        *argos::any_cast<argos::CFootBotEntity*>(entity_pair.second);
    auto& controller =
        dynamic_cast<controller::depth0::stateful_foraging_controller&>(
            robot.GetControllableEntity().GetController());
    auto* l_params = static_cast<const struct params::loop_functions_params*>(
        repo.get_params("loop_functions"));

    controller.display_los(l_params->display_robot_los);
    utils::set_robot_los<controller::depth0::stateful_foraging_controller>(
        robot, *map());
  } /* for(entity..) */
  ER_NOM("stateful_foraging loop functions initialization finished");
}

void stateful_foraging_loop_functions::pre_step_iter(
    argos::CFootBotEntity& robot) {
  auto& controller =
      static_cast<controller::depth0::stateful_foraging_controller&>(
          robot.GetControllableEntity().GetController());

  /* get stats from this robot before its state changes */
  distance_collector()->collect(
      static_cast<metrics::fsm::distance_metrics&>(controller));
  if (controller.current_task()) {
    stateless_collector()->collect(
        static_cast<metrics::fsm::stateless_metrics&>(*controller.current_task()));
    m_collector->collect(static_cast<metrics::fsm::stateful_metrics&>(*controller.current_task()));
  }

  /* Send the robot its new line of sight */
  utils::set_robot_pos<controller::depth0::stateful_foraging_controller>(robot);
  utils::set_robot_los<controller::depth0::stateful_foraging_controller>(robot,
                                                                         *map());
  set_robot_tick<controller::depth0::stateful_foraging_controller>(robot);

  if (controller.is_carrying_block()) {
    handle_nest_block_drop<controller::depth0::stateful_foraging_controller>(
        robot, *map(), *block_collector());
  } else { /* The foot-bot has no block item */
    handle_free_block_pickup<controller::depth0::stateful_foraging_controller>(
        robot, *map());
  }
} /* pre_step_iter() */

argos::CColor stateful_foraging_loop_functions::GetFloorColor(
    const argos::CVector2& plane_pos) {
  /* The nest is a light gray */
  if (nest_xrange().WithinMinBoundIncludedMaxBoundIncluded(plane_pos.GetX()) &&
      nest_yrange().WithinMinBoundIncludedMaxBoundIncluded(plane_pos.GetY())) {
    return argos::CColor::GRAY70;
  }

  for (size_t i = 0; i < map()->blocks().size(); ++i) {
    if (map()->blocks()[i].contains_point(plane_pos)) {
      return argos::CColor::BLACK;
    }
  } /* for(i..) */

  return argos::CColor::WHITE;
} /* GetFloorColor() */

void stateful_foraging_loop_functions::Destroy(void) {
  stateless_foraging_loop_functions::Destroy();
  m_collector->finalize();
}

void stateful_foraging_loop_functions::Reset(void) {
  stateless_foraging_loop_functions::Reset();
  m_collector->reset();
}

void stateful_foraging_loop_functions::pre_step_final(void) {
  stateless_foraging_loop_functions::pre_step_final();

  m_collector->csv_line_write(GetSpace().GetSimulationClock());
  m_collector->timestep_reset();
  m_collector->interval_reset();
  m_collector->timestep_inc();
} /* pre_step_final() */

void stateful_foraging_loop_functions::PreStep() {
  for (auto& entity_pair : GetSpace().GetEntitiesByType("foot-bot")) {
    argos::CFootBotEntity& robot =
        *argos::any_cast<argos::CFootBotEntity*>(entity_pair.second);
    pre_step_iter(robot);
  } /* for(&entity..) */
  pre_step_final();
} /* PreStep() */

__pure metrics::fsm::stateful_metrics_collector* stateful_foraging_loop_functions::stateful_collector(void) const {
  return m_collector.get();
} /* depth0_collector() */

using namespace argos;
REGISTER_LOOP_FUNCTIONS(stateful_foraging_loop_functions,
                        "stateful_foraging_loop_functions"); // NOLINT

NS_END(depth0, support, fordyca);
