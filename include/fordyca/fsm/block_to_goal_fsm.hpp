/**
 * \file block_to_goal_fsm.hpp
 *
 * \copyright 2018 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_FSM_BLOCK_TO_GOAL_FSM_HPP_
#define INCLUDE_FORDYCA_FSM_BLOCK_TO_GOAL_FSM_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/er/client.hpp"

#include "cosm/fsm/metrics/goal_acq_metrics.hpp"
#include "cosm/fsm/util_hfsm.hpp"
#include "cosm/robots/footbot/footbot_subsystem_fwd.hpp"
#include "cosm/ta/taskable.hpp"

#include "fordyca/fordyca.hpp"
#include "fordyca/fsm/block_transporter.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace cosm::fsm {
class acquire_goal_fsm;
} /* namespace cosm::fsm */

NS_START(fordyca, fsm);

class acquire_free_block_fsm;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class block_to_goal_fsm
 * \ingroup fsm depth1
 *
 * \brief Base FSM for acquiring, picking up a block, and then bringing it
 * somewhere and dropping it.
 *
 * Each robot executing this FSM will locate a free block (either a known block
 * or via random exploration), pickup the block and bring it to its chosen
 * goal. Once it has done that it will signal that its task is complete.
 */
class block_to_goal_fsm : public rer::client<block_to_goal_fsm>,
                          public cfsm::util_hfsm,
                          public cta::taskable,
                          public cfsm::metrics::goal_acq_metrics,
                          public fsm::block_transporter {
 public:
  block_to_goal_fsm(cfsm::acquire_goal_fsm* goal_fsm,
                    cfsm::acquire_goal_fsm* block_fsm,
                    crfootbot::footbot_saa_subsystem2D* saa,
                    rmath::rng* rng);
  ~block_to_goal_fsm(void) override = default;

  block_to_goal_fsm(const block_to_goal_fsm&) = delete;
  block_to_goal_fsm& operator=(const block_to_goal_fsm&) = delete;

  /* taskable overrides */
  void task_execute(void) override;
  void task_start(const cta::taskable_argument* arg) override;
  bool task_finished(void) const override {
    return ekST_FINISHED == current_state();
  }
  bool task_running(void) const override {
    return !(ekST_FINISHED == current_state() || ekST_START == current_state());
  }
  void task_reset(void) override { init(); }

  /* collision metrics */
  bool in_collision_avoidance(void) const override final RCSW_PURE;
  bool entered_collision_avoidance(void) const override final RCSW_PURE;
  bool exited_collision_avoidance(void) const override final RCSW_PURE;
  rtypes::timestep collision_avoidance_duration(
      void) const override final RCSW_PURE;
  rmath::vector2z avoidance_loc(void) const override final RCSW_PURE;

  /* goal acquisition metrics */
  rmath::vector2z acquisition_loc(void) const override final;
  bool is_vectoring_to_goal(void) const override final RCSW_PURE;
  exp_status is_exploring_for_goal(void) const override final RCSW_PURE;
  bool goal_acquired(void) const override RCSW_PURE;
  cfsm::metrics::goal_acq_metrics::goal_type acquisition_goal(void) const override;
  rmath::vector2z current_explore_loc(void) const override final;
  rmath::vector2z current_vector_loc(void) const override final;

  /**
   * \brief Reset the FSM
   */
  void init(void) override final;

 protected:
  enum fsm_states {
    ekST_START,
    /**
     * Superstate for acquiring a block (free or from a cache).
     */
    ekST_ACQUIRE_BLOCK,

    /**
     * A block has been acquired--wait for area to send the block pickup signal.
     */
    ekST_WAIT_FOR_BLOCK_PICKUP,

    /**
     * We are transporting a carried block to our goal.
     */
    ekST_TRANSPORT_TO_GOAL,

    /**
     * We have acquired our goal--wait for arena to send the block drop signal.
     */
    ekST_WAIT_FOR_BLOCK_DROP,

    /**
     * Block has been successfully dropped at our goal/in our goal.
     */
    ekST_FINISHED,
    ekST_MAX_STATES,
  };

  const cfsm::acquire_goal_fsm* goal_fsm(void) const { return m_goal_fsm; }
  const cfsm::acquire_goal_fsm* block_fsm(void) const { return m_block_fsm; }

 private:
  /* inherited states */
  HFSM_ENTRY_INHERIT_ND(cfsm::util_hfsm, entry_wait_for_signal);

  /* block to goal states */
  HFSM_STATE_DECLARE(block_to_goal_fsm, start, rpfsm::event_data);
  HFSM_STATE_DECLARE_ND(block_to_goal_fsm, acquire_block);
  HFSM_STATE_DECLARE(block_to_goal_fsm, wait_for_block_pickup, rpfsm::event_data);
  HFSM_STATE_DECLARE_ND(block_to_goal_fsm, transport_to_goal);
  HFSM_STATE_DECLARE(block_to_goal_fsm, wait_for_block_drop, rpfsm::event_data);
  HFSM_STATE_DECLARE_ND(block_to_goal_fsm, finished);

  /**
   * \brief Defines the state map for the FSM.
   *
   * Note that the order of the states in the map MUST match the order of the
   * states in \enum fsm_states, or things will not work correctly.
   */
  HFSM_DEFINE_STATE_MAP_ACCESSOR(state_map_ex, index) override {
    return &mc_state_map[index];
  }

  HFSM_DECLARE_STATE_MAP(state_map_ex, mc_state_map, ekST_MAX_STATES);

  /* clang-format off */
  cfsm::acquire_goal_fsm* const  m_goal_fsm;
  cfsm::acquire_goal_fsm * const m_block_fsm;
  /* clang-format on */
};

NS_END(fsm, fordyca);

#endif /* INCLUDE_FORDYCA_FSM_BLOCK_TO_GOAL_FSM_HPP_ */
