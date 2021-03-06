/**
 * \file crw_controller.cpp
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
#include "fordyca/controller/reactive/d0/crw_controller.hpp"

#include <fstream>

#include "cosm/fsm/supervisor_fsm.hpp"
#include "cosm/repr/base_block3D.hpp"
#include "cosm/repr/config/nest_config.hpp"
#include "cosm/spatial/strategy/nest_acq/factory.hpp"
#include "cosm/subsystem/saa_subsystemQ3D.hpp"

#include "fordyca/config/foraging_controller_repository.hpp"
#include "fordyca/fsm/d0/crw_fsm.hpp"
#include "fordyca/strategy/explore/block_factory.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller, reactive, d0);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
crw_controller::crw_controller(void)
    : ER_CLIENT_INIT("fordyca.controller.d0.crw"), m_fsm() {}

crw_controller::~crw_controller(void) = default;

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void crw_controller::init(ticpp::Element& node) {
  foraging_controller::init(node);
  ndc_push();
  ER_INFO("Initializing...");

  config::foraging_controller_repository repo;
  repo.parse_all(node);

  if (!repo.validate_all()) {
    ER_FATAL_SENTINEL("Not all parameters were validated");
    std::exit(EXIT_FAILURE);
  }

  fstrategy::foraging_strategy::params p(
      saa(), nullptr, nullptr, nullptr, rutils::color());
  const auto* nest = repo.config_get<crepr::config::nest_config>();
  const auto* strat_config = repo.config_get<fcstrategy::strategy_config>();

  m_fsm = std::make_unique<fsm::d0::crw_fsm>(
      saa(),
      fsexplore::block_factory().create(fsexplore::block_factory::kCRW, &p, rng()),
      csstrategy::nest_acq::factory().create(
          strat_config->nest_acq.strategy, saa(), rng()),
      nest->center,
      rng());
  /* Set CRW FSM supervision */
  supervisor()->supervisee_update(m_fsm.get());
  ER_INFO("Initialization finished");
  ndc_pop();
} /* init() */

void crw_controller::reset(void) {
  foraging_controller::reset();
  if (nullptr != m_fsm) {
    m_fsm->init();
  }
} /* reset() */

void crw_controller::control_step(void) {
  ndc_pusht();
  ER_ASSERT(!(nullptr != block() && !block()->is_carried_by_robot()),
            "Carried block%d has robot id=%d",
            block()->id().v(),
            block()->md()->robot_id().v());

  /*
   * Run the FSM and apply steering forces if normal operation, otherwise handle
   * abnormal operation state.
   */
  supervisor()->run();
  ndc_pop();
} /* control_step() */

/*******************************************************************************
 * Goal Acquisition Metrics
 ******************************************************************************/
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, goal_acquired, *m_fsm, const);
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, entity_acquired_id, *m_fsm, const);
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, is_exploring_for_goal, *m_fsm, const);
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, acquisition_goal, *m_fsm, const);
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, block_transport_goal, *m_fsm, const);
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, acquisition_loc3D, *m_fsm, const);
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, vector_loc3D, *m_fsm, const);
RCPPSW_WRAP_DEF_OVERRIDE(crw_controller, explore_loc3D, *m_fsm, const);

/*******************************************************************************
 * Block Transportation Metrics
 ******************************************************************************/
bool crw_controller::is_phototaxiing_to_goal(bool include_ca) const {
  return m_fsm->is_phototaxiing_to_goal(include_ca);
} /* is_phototaxiing_to_goal() */

using namespace argos; // NOLINT

RCPPSW_WARNING_DISABLE_PUSH()
RCPPSW_WARNING_DISABLE_MISSING_VAR_DECL()
RCPPSW_WARNING_DISABLE_MISSING_PROTOTYPE()
RCPPSW_WARNING_DISABLE_GLOBAL_CTOR()

REGISTER_CONTROLLER(crw_controller, "crw_controller");

RCPPSW_WARNING_DISABLE_POP()

NS_END(reactive, d0, controller, fordyca);
