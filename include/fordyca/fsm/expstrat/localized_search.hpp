/**
 * \file localized_search.hpp
 *
 * \copyright 2019 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_FSM_EXPSTRAT_LOCALIZED_SEARCH_HPP_
#define INCLUDE_FORDYCA_FSM_EXPSTRAT_LOCALIZED_SEARCH_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <memory>

#include "fordyca/fsm/expstrat/foraging_expstrat.hpp"
#include "rcppsw/math/vector2.hpp"
#include "cosm/fsm/vector_fsm.hpp"
#include "fordyca/fsm/expstrat/crw.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, fsm, expstrat);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class localized_search
 * \ingroup fsm expstrat
 *
 * \brief An exploration behavior in which robots vector to a specific location,
 * then begin correlated random walk exploration there via \ref crw. Falls back
 * to vanilla \ref crw if a specific location is not given during at task start.
 */
class localized_search : public foraging_expstrat,
                         public rer::client<localized_search> {
 public:
  localized_search(const foraging_expstrat::params* const c_params,
                   rmath::rng* rng)
      : localized_search(c_params->saa, rng) {}

  localized_search(crfootbot::footbot_saa_subsystem2D* saa, rmath::rng* rng)
      : foraging_expstrat(saa, rng),
        ER_CLIENT_INIT("fordyca.fsm.expstrat.localized_search"),
        m_vfsm(saa, rng),
        m_crw(saa, rng) {}

  ~localized_search(void) override = default;
  localized_search(const localized_search&) = delete;
  localized_search& operator=(const localized_search&) = delete;

  /* collision metrics */
  bool in_collision_avoidance(void) const override final RCSW_PURE;
  bool entered_collision_avoidance(void) const override final RCSW_PURE;
  bool exited_collision_avoidance(void) const override final RCSW_PURE;
  rtypes::timestep collision_avoidance_duration(void) const override final;
  rmath::vector2z avoidance_loc(void) const override final;

  /* taskable overrides */

  /**
   * \brief Start the targeted exploration by starting to vector to the starting
   * location.
   *
   * \param c_arg The starting location, or NULL if there is not a
   * location. This can happen if the robot doesn't know of a good starting
   * location (e.g. the location of the last known object of a specific type),
   * in which case we will just fall back to regular CRW.
   */
  void task_start(const cta::taskable_argument* c_arg) override {
    if (nullptr != c_arg) {
      m_vfsm.task_start(c_arg);
    }
  }
  void task_reset(void) override final {
    m_vfsm.task_reset();
    m_crw.task_reset();
  }
  bool task_running(void) const override final {
    return m_vfsm.task_running() || m_crw.task_running();
  }

  /**
   * \brief Since we are exploring for something we don't know about, we will
   * never finish (stopping exploration is handled at a higher level).
   */
  bool task_finished(void) const override final { return false; }
  void task_execute(void) override final;

  /* prototype overrides */
  std::unique_ptr<foraging_expstrat> clone(void) const override {
    return std::make_unique<localized_search>(saa(), rng());
  }

 private:
  /* clang-format off */
  cfsm::vector_fsm m_vfsm;
  crw               m_crw;
  /* clang-format on */
};

NS_END(expstrat, fsm, fordyca);

#endif /* INCLUDE_FORDYCA_FSM_EXPSTRAT_LOCALIZED_SEARCH_HPP_ */
