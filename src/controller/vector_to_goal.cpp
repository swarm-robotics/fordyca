/**
 * @file vector_to_goal.cpp
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
#include <argos3/core/utility/datatypes/color.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include "fordyca/controller/vector_to_goal.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller);
namespace fsm = rcppsw::patterns::state_machine;

/*******************************************************************************
 * Constants
 ******************************************************************************/
uint vector_to_goal::kCOLLISION_RECOVERY_TIME = 12;
double vector_to_goal::kVECTOR_TO_GOAL_MIN_DIFF = 0.02;

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
vector_to_goal::vector_to_goal(double frequent_collision_thresh,
                               std::shared_ptr<rcppsw::common::er_server> server,
                               std::shared_ptr<sensor_manager> sensors,
                               std::shared_ptr<actuator_manager> actuators) :
    fsm::simple_fsm(server, ST_MAX_STATES),
    start(),
    vector(),
    collision_avoidance(),
    collision_recovery(),
    arrived(),
    entry_vector(),
    entry_collision_avoidance(),
    entry_collision_recovery(),
    m_rng(argos::CRandom::CreateRNG("argos")),
    m_state(),
    m_freq_collision_thresh(frequent_collision_thresh),
    m_collision_rec_count(0),
    m_sensors(sensors),
    m_actuators(actuators),
    m_goal(),
    m_ang_pid(5.0, 0, 0,
              1,
              -m_actuators->max_wheel_speed() * 0.5,
              m_actuators->max_wheel_speed() * 0.5),
    m_lin_pid(3.0, 0, 0.02,
              1,
              m_actuators->max_wheel_speed() * 0.1,
              m_actuators->max_wheel_speed() * 0.7) {
    insmod("vector_to_goal",
           rcppsw::common::er_lvl::DIAG,
           rcppsw::common::er_lvl::NOM);
    }

/*******************************************************************************
 * Events
 ******************************************************************************/
void vector_to_goal::event_start(const argos::CVector2& goal) {
  static const uint8_t kTRANSITIONS[] = {
    ST_VECTOR,                  /* start */
    ST_VECTOR,                  /* vector */
    fsm::event_signal::IGNORED,  /* collision avoidance */
    fsm::event_signal::IGNORED,  /* collision recovery */
    fsm::event_signal::IGNORED,  /* arrived */
  };
  FSM_VERIFY_TRANSITION_MAP(kTRANSITIONS);
  external_event(kTRANSITIONS[current_state()],
                 rcppsw::make_unique<struct goal_data>(goal));
}

/*******************************************************************************
 * States
 ******************************************************************************/
FSM_STATE_DEFINE(vector_to_goal, start, fsm::no_event_data) {
  return fsm::event_signal::HANDLED;
}

FSM_STATE_DEFINE(vector_to_goal, collision_avoidance, fsm::no_event_data) {
  argos::CVector2 vector;
  if (ST_COLLISION_AVOIDANCE != last_state()) {
    ER_DIAG("Executing ST_COLLIISION_AVOIDANCE");
  }

  /*
   * We stay in collision avoidance until we are sufficiently distant/heading
   * away from the obstacle. We do collision recovery ONLY if we came from the
   * vector_to_target state to get back on trajectory, but not if we are
   * randomly exploring or doing something else.
   */
  if (m_sensors->calc_diffusion_vector(&vector)) {
    if (m_sensors->tick() - m_state.last_collision_time <
        m_freq_collision_thresh) {
      ER_DIAG("Frequent collision: last=%u curr=%u",
              m_state.last_collision_time, m_sensors->tick());
      vector = randomize_vector_angle(vector);
    }
    m_actuators->set_heading(vector);
  } else {
    internal_event(ST_COLLISION_RECOVERY);
  }
  return fsm::event_signal::HANDLED;
}
FSM_STATE_DEFINE(vector_to_goal, collision_recovery, fsm::no_event_data) {
  if (ST_COLLISION_RECOVERY != last_state()) {
    ER_DIAG("Executing ST_COLLISION_RECOVERY");
  }

  if (m_collision_rec_count++ >= kCOLLISION_RECOVERY_TIME) {
    m_collision_rec_count = 0;
    internal_event(ST_VECTOR);
  }
  return fsm::event_signal::HANDLED;
}
FSM_STATE_DEFINE(vector_to_goal, vector, goal_data) {
  if (ST_VECTOR != last_state()) {
    ER_DIAG("Executing ST_VECTOR");
  }

  double ang_speed = 0;
  double lin_speed = 0;
  if (data) {
    m_goal = data->goal;
  }

  if (m_sensors->calc_diffusion_vector(NULL)) {
    internal_event(ST_COLLISION_AVOIDANCE);
  }
  if ((m_goal - m_sensors->robot_loc()).Length() <=
      kVECTOR_TO_GOAL_MIN_DIFF) {
    lin_speed = 0;
    ang_speed = 0;
    m_ang_pid.reset();
    m_lin_pid.reset();
    internal_event(ST_ARRIVED, rcppsw::make_unique<struct goal_data>(m_goal));
  }
  argos::CVector2 robot_to_goal = calc_vector_to_goal(m_goal);
  argos::CVector2 heading = m_sensors->robot_heading();
  double angle_diff = heading.Angle().GetValue() -
                      robot_to_goal.Angle().GetValue();

  ang_speed = m_ang_pid.calculate(0, angle_diff);
  lin_speed = m_lin_pid.calculate(0, -0.1/std::fabs(angle_diff));

  ER_VER("target: (%f, %f)@%f", m_goal.GetX(), m_goal.GetY(),
         m_goal.Angle().GetValue());
  ER_VER("robot_to_target: vector=(%f, %f)@%f, len=%f",
         robot_to_goal.GetX(), robot_to_goal.GetY(),
         robot_to_goal.Angle().GetValue(), robot_to_goal.Length());
  ER_VER("robot_heading=(%f, %f)@%f ang_speed=%f lin_speed=%f",
         heading.GetX(), heading.GetY(), heading.Angle().GetValue(),
         ang_speed, lin_speed);
  m_actuators->set_wheel_speeds(lin_speed, ang_speed);
  return fsm::event_signal::HANDLED;
}
FSM_STATE_DEFINE(vector_to_goal, arrived, struct goal_data) {
  if (ST_VECTOR != last_state()) {
    ER_DIAG("Executing ST_ARRIVED: target (%f, %f) within %f tolerance",
            data->goal.GetX(), data->goal.GetY(),
            kVECTOR_TO_GOAL_MIN_DIFF);
  }
  return fsm::event_signal::HANDLED;
}

FSM_ENTRY_DEFINE(vector_to_goal, entry_vector, fsm::no_event_data) {
  ER_DIAG("Entering ST_VECTOR");
  m_actuators->leds_set_color(argos::CColor::BLUE);
}
FSM_ENTRY_DEFINE(vector_to_goal, entry_collision_avoidance, fsm::no_event_data) {
  ER_DIAG("Entering ST_COLLISION_AVOIDANCE");
  m_actuators->leds_set_color(argos::CColor::RED);
  m_state.last_collision_time = m_sensors->tick();
}
FSM_ENTRY_DEFINE(vector_to_goal, entry_collision_recovery, fsm::no_event_data) {
  ER_DIAG("Entering ST_COLLISION_RECOVERY");
  m_actuators->leds_set_color(argos::CColor::YELLOW);
}
/*******************************************************************************
 * General Member Functions
 ******************************************************************************/
void vector_to_goal::init(void) {
  m_state.time_exploring_unsuccessfully = 0;
  m_actuators->reset();
  simple_fsm::init();
} /* init() */

argos::CVector2 vector_to_goal::calc_vector_to_goal(const argos::CVector2& goal) {
  return goal - m_sensors->robot_loc();
} /* calc_vector_to_goal() */

argos::CVector2 vector_to_goal::randomize_vector_angle(argos::CVector2 vector) {
  argos::CRange<argos::CRadians> range(argos::CRadians(0.0),
                                       argos::CRadians(1.0));
  vector.Rotate(m_rng->Uniform(range));
  return vector;
} /* randomize_vector_angle() */


NS_END(controller, fordyca);
