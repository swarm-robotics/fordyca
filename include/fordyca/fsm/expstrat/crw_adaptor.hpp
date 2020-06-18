/**
 * \file crw_adaptor.hpp
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

#ifndef INCLUDE_FORDYCA_FSM_EXPSTRAT_CRW_ADAPTOR_HPP_
#define INCLUDE_FORDYCA_FSM_EXPSTRAT_CRW_ADAPTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <memory>
#include "rcppsw/patterns/decorator/decorator.hpp"

#include "cosm/spatial/expstrat/crw.hpp"
#include "fordyca/fsm/expstrat/foraging_expstrat.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, fsm, expstrat);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class crw_adaptor
 * \ingroup fsm expstrat
 *
 * \brief Adaptor for the CRW exploration strategy to enable it as an
 * exploration factory output within FORDYCA. Uses the decorator pattern on the
 * parent CRW class in order to avoid diamond inheritance with \ref
 * csexpstrat::base_expstrat.
 */
class crw_adaptor final : public foraging_expstrat,
                          public rpdecorator::decorator<csexpstrat::crw> {
 public:
  crw_adaptor(const fsm::expstrat::foraging_expstrat::params* c_params,
              rmath::rng* rng);
  crw_adaptor(crfootbot::footbot_saa_subsystem* saa, rmath::rng* rng);

  ~crw_adaptor(void) override = default;
  crw_adaptor(const crw_adaptor&) = delete;
  crw_adaptor& operator=(const crw_adaptor&) = delete;

  /* interference metrics */
  RCPPSW_DECLDEF_WRAP_OVERRIDE(exp_interference, decoratee(), const);
  RCPPSW_DECLDEF_WRAP_OVERRIDE(entered_interference, decoratee(), const);
  RCPPSW_DECLDEF_WRAP_OVERRIDE(exited_interference, decoratee(), const);
  RCPPSW_DECLDEF_WRAP_OVERRIDE(interference_duration, decoratee(), const);
  RCPPSW_DECLDEF_WRAP_OVERRIDE(interference_loc3D, decoratee(), const);

  RCPPSW_DECLDEF_WRAP_OVERRIDE(task_reset, decoratee());
  RCPPSW_DECLDEF_WRAP_OVERRIDE(task_running, decoratee(), const);
  RCPPSW_DECLDEF_WRAP_OVERRIDE(task_finished, decoratee(), const);
  RCPPSW_DECLDEF_WRAP_OVERRIDE(task_execute, decoratee());
  void task_start(cta::taskable_argument* arg) override { decoratee().task_start(arg); }

  /* prototype overrides */
  RCPPSW_DECLDEF_WRAP_OVERRIDE(clone, decoratee(), const);
};

NS_END(expstrat, fsm, fordyca);

#endif /* INCLUDE_FORDYCA_FSM_EXPSTRAT_CRW_ADAPTOR_HPP_ */