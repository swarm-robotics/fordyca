/**
 * @file grp_mdpo_controller.cpp
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
#include "fordyca/controller/depth2/grp_mdpo_controller.hpp"
#include "fordyca/controller/mdpo_perception_subsystem.hpp"
#include "fordyca/controller/saa_subsystem.hpp"
#include "fordyca/ds/dpo_semantic_map.hpp"
#include "fordyca/params/depth2/controller_repository.hpp"
#include "fordyca/params/perception/perception_params.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller, depth2);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
grp_mdpo_controller::grp_mdpo_controller(void)
    : ER_CLIENT_INIT("fordyca.controller.depth2.grp_mdpo") {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void grp_mdpo_controller::Init(ticpp::Element& node) {
  base_controller::Init(node);
  ndc_push();
  ER_INFO("Initializing");

  params::depth2::controller_repository param_repo;
  param_repo.parse_all(node);
  if (!param_repo.validate_all()) {
    ER_FATAL_SENTINEL("Not all parameters were validated");
    std::exit(EXIT_FAILURE);
  }

  shared_init(param_repo);

  ER_INFO("Initialization finished");
  ndc_pop();
} /* Init() */

void grp_mdpo_controller::shared_init(
    const params::depth2::controller_repository& param_repo) {
  /* block/cache selection matrices, executive  */
  grp_dpo_controller::shared_init(param_repo);

  /* MDPO perception subsystem */
  params::perception::perception_params p =
      *param_repo.parse_results<params::perception::perception_params>();
  p.occupancy_grid.upper.x(p.occupancy_grid.upper.x() + 1);
  p.occupancy_grid.upper.y(p.occupancy_grid.upper.y() + 1);

  gp_dpo_controller::perception(
      rcppsw::make_unique<mdpo_perception_subsystem>(&p, GetId()));
} /* shared_init() */

__rcsw_pure mdpo_perception_subsystem* grp_mdpo_controller::mdpo_perception(void) {
  return static_cast<mdpo_perception_subsystem*>(dpo_controller::perception());
} /* mdpo_perception() */

using namespace argos; // NOLINT
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wglobal-constructors"
REGISTER_CONTROLLER(grp_mdpo_controller,
                    "grp_mdpo_controller"); // NOLINT
#pragma clang diagnostic pop

NS_END(depth2, controller, fordyca);