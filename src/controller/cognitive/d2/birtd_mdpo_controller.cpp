/**
 * \file birtd_mdpo_controller.cpp
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
#include "fordyca/controller/cognitive/d2/birtd_mdpo_controller.hpp"

#include "cosm/arena/repr/base_cache.hpp"
#include "cosm/fsm/supervisor_fsm.hpp"
#include "cosm/repr/base_block3D.hpp"
#include "cosm/subsystem/perception/config/perception_config.hpp"

#include "fordyca/config/d2/controller_repository.hpp"
#include "fordyca/controller/cognitive/mdpo_perception_subsystem.hpp"
#include "fordyca/ds/dpo_semantic_map.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller, cognitive, d2);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
birtd_mdpo_controller::birtd_mdpo_controller(void)
    : ER_CLIENT_INIT("fordyca.controller.d2.birtd_mdpo") {}

birtd_mdpo_controller::~birtd_mdpo_controller(void) = default;

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void birtd_mdpo_controller::init(ticpp::Element& node) {
  foraging_controller::init(node);
  ndc_push();
  ER_INFO("Initializing");

  config::d2::controller_repository config_repo;
  config_repo.parse_all(node);
  if (!config_repo.validate_all()) {
    ER_FATAL_SENTINEL("Not all parameters were validated");
    std::exit(EXIT_FAILURE);
  }

  shared_init(config_repo);

  ER_INFO("Initialization finished");
  ndc_pop();
} /* init() */

void birtd_mdpo_controller::shared_init(
    const config::d2::controller_repository& config_repo) {
  /* block/cache selection matrices, executive  */
  birtd_dpo_controller::shared_init(config_repo);

  /* MDPO perception subsystem */
  auto p = *config_repo.config_get<cspconfig::perception_config>();
  rmath::vector2d padding(p.occupancy_grid.resolution.v() * 5,
                          p.occupancy_grid.resolution.v() * 5);
  p.occupancy_grid.dims += padding;

  bitd_dpo_controller::perception(
      std::make_unique<mdpo_perception_subsystem>(&p, GetId()));
} /* shared_init() */

mdpo_perception_subsystem* birtd_mdpo_controller::mdpo_perception(void) {
  return static_cast<mdpo_perception_subsystem*>(dpo_controller::perception());
} /* mdpo_perception() */

const mdpo_perception_subsystem*
birtd_mdpo_controller::mdpo_perception(void) const {
  return static_cast<const mdpo_perception_subsystem*>(
      dpo_controller::perception());
} /* mdpo_perception() */

using namespace argos; // NOLINT

RCPPSW_WARNING_DISABLE_PUSH()
RCPPSW_WARNING_DISABLE_MISSING_VAR_DECL()
RCPPSW_WARNING_DISABLE_MISSING_PROTOTYPE()
RCPPSW_WARNING_DISABLE_GLOBAL_CTOR()

REGISTER_CONTROLLER(birtd_mdpo_controller,
                    "birtd_mdpo_controller"); // NOLINT

RCPPSW_WARNING_DISABLE_POP()

NS_END(cognitive, d2, controller, fordyca);
