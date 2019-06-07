/**
 * @file base_foraging_fsm.hpp
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

#ifndef INCLUDE_FORDYCA_FSM_BASE_FORAGING_FSM_HPP_
#define INCLUDE_FORDYCA_FSM_BASE_FORAGING_FSM_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <argos3/core/utility/math/rng.h>
#include <memory>
#include <string>

#include "fordyca/fsm/collision_tracker.hpp"
#include "fordyca/fsm/new_direction_data.hpp"
#include "fordyca/metrics/fsm/collision_metrics.hpp"
#include "rcppsw/math/vector2.hpp"
#include "rcppsw/patterns/state_machine/hfsm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace controller {
class saa_subsystem;
class sensing_subsystem;
class actuation_subsystem;
} // namespace controller

NS_START(fsm);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class base_foraging_fsm
 * @ingroup fordyca fsm
 *
 * @brief A collection of base states/common functionality that foraging FSMs
 * can use if they choose.
 *
 * This class cannot be instantiated on its own, as does not define an FSM
 * per-se.
 */
class base_foraging_fsm : public rpfsm::hfsm,
                          public rer::client<base_foraging_fsm>,
                          public metrics::fsm::collision_metrics {
 public:
  base_foraging_fsm(controller::saa_subsystem* saa, uint8_t max_states);

  ~base_foraging_fsm(void) override = default;

  base_foraging_fsm(const base_foraging_fsm& fsm) = delete;
  base_foraging_fsm& operator=(const base_foraging_fsm& fsm) = delete;

  /**
   * @brief (Re)-initialize the FSM.
   */
  void init(void) override;

  /**
   * @brief Get a reference to the \ref sensing_subsystem.
   *
   * Derived classes needing to reference these sensors should use this function
   * rather than maintaining their own std::shared_ptr copy of things, as that
   * can cause nasty bugs involving things set by the arena loop functions such
   * as tick, location, etc., and that do not get propagated down the
   * composition/inheritance hierarchy of robot controllers properly.
   */
  const std::shared_ptr<const controller::sensing_subsystem> sensors(void) const;
  const std::shared_ptr<controller::sensing_subsystem> sensors(void);

  const std::shared_ptr<const controller::actuation_subsystem> actuators(
      void) const;
  const std::shared_ptr<controller::actuation_subsystem> actuators(void);

 protected:
  /**
   * @brief Randomize the angle of a vector, for use in change robot heading
   *
   * @param vector The input vector
   *
   * @return The same vector, but with a new angle.
   */
  rmath::vector2d randomize_vector_angle(const rmath::vector2d& vector);

  const controller::saa_subsystem* saa_subsystem(void) const { return m_saa; }
  controller::saa_subsystem* saa_subsystem(void) { return m_saa; }
  collision_tracker* ca_tracker(void) { return &m_tracker; }
  const collision_tracker* ca_tracker(void) const { return &m_tracker; }

  /**
   * @brief Robots entering this state will return to the nest.
   *
   * This state MUST have a parent state defined that is not
   * \ref rcppsw::patterns::state_machine::hfsm::top_state().
   *
   * Upon return to the nest, a \ref foraging_signal::kBLOCK_DROP
   * signal will be returned to the parent state. No robot should return to the
   * nest unless it has a block (duh).
   */
  HFSM_STATE_DECLARE(base_foraging_fsm, transport_to_nest, rpfsm::event_data);

  /**
   * @brief Robots entering this state will leave the nest (they are assumed to
   * already be in the nest when this state is entered).
   *
   * This state MUST have a parent state defined that is not \ref
   * rcppsw::patterns::state_machine::hfsm::top_state().
   *
   * When the robot has actually left the nest, according to sensor readings, a
   * \ref foraging_signal::kLEFT_NEST signal is returned to the
   * parent state.
   */
  HFSM_STATE_DECLARE(base_foraging_fsm, leaving_nest, rpfsm::event_data);

  /**
   * @brief Robots entering this state will randomly change their exploration
   * direction to the specified direction. All signals are ignored in this
   * state. Once the direction change has been accomplished, the robot will
   * transition back to its previous state.
   */
  HFSM_STATE_DECLARE(base_foraging_fsm, new_direction, rpfsm::event_data);

  /**
   * @brief A simple entry state for returning to nest, used to set LED colors
   * for visualization purposes.
   */
  HFSM_ENTRY_DECLARE_ND(base_foraging_fsm, entry_transport_to_nest);

  /**
   * @brief A simple entry state for leaving nest, used to set LED colors for
   * visualization purposes.
   */
  HFSM_ENTRY_DECLARE_ND(base_foraging_fsm, entry_leaving_nest);

  /**
   * @brief Simple state for entry into the new direction state, used to change
   * LED color for visualization purposes.
   */
  HFSM_ENTRY_DECLARE_ND(base_foraging_fsm, entry_new_direction);

  /**
   * @brief Simple state for entry into the "wait for signal" state, used to
   * change LED color for visualization purposes.
   */
  HFSM_ENTRY_DECLARE_ND(base_foraging_fsm, entry_wait_for_signal);

 private:
  /**
   * @brief When changing direction, a robot is spinning at such a speed that it
   * may overshoot its desired new direction, but as long as it does not
   * overshoot by more than this tolerance, the direction change will still be
   * considered to have occurred successfully.
   */
  static constexpr double kDIR_CHANGE_TOL = 0.25;

  /**
   * @brief When changing direction, it may not be enough to have an arrival
   * tolerance for the new heading; it is possible that given the new direction,
   * the robot's initial heading, and the spinning speed, and it is impossible
   * for the robot to arrive within tolerance of the desired new direction. So,
   * I also define a max number of steps that the robot will spin as a secondary
   * safeguard.
   *
   * Not doing this leads to robots that will spin more or less indefinitely
   * when changing direction on occasion.
   */
  static constexpr uint kDIR_CHANGE_MAX_STEPS = 10;

  /**
   * @brief When entering the nest, you want to continue to wander a bit before
   * signaling upper FSMs that you are in the nest, so that there is (slightly)
   * less congestion by the edge. This is a stopgap solution; a more elegant fix
   * may be forthcoming in the future if warranted.
   */
  static constexpr uint kNEST_COUNT_MAX_STEPS = 25;

  /* clang-format off */
  uint                             m_nest_count{0};
  uint                             m_new_dir_count{0};
  rmath::radians                   m_new_dir{};
  argos::CRandom::CRNG*            m_rng;
  controller::saa_subsystem* const m_saa;
  collision_tracker                m_tracker;
  /* clang-format on */

 public:
  /* collision metrics */
  RCPPSW_DECLDEF_OVERRIDE_WRAP(in_collision_avoidance, m_tracker, const)
  RCPPSW_DECLDEF_OVERRIDE_WRAP(entered_collision_avoidance, m_tracker, const)
  RCPPSW_DECLDEF_OVERRIDE_WRAP(exited_collision_avoidance, m_tracker, const)
  RCPPSW_DECLDEF_OVERRIDE_WRAP(collision_avoidance_duration, m_tracker, const)
  RCPPSW_DECLDEF_OVERRIDE_WRAP(avoidance_loc, m_tracker, const)
};

NS_END(fsm, fordyca);

#endif /* INCLUDE_FORDYCA_FSM_BASE_FORAGING_FSM_HPP_ */
