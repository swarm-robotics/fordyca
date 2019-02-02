/**
 * @file ee_max_fsm.cpp
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
#include "fordyca/fsm/depth0/ee_max_fsm.hpp"
#include "fordyca/controller/actuation_subsystem.hpp"
#include "fordyca/controller/foraging_signal.hpp"
#include "fordyca/controller/random_explore_behavior.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, fsm, depth0);
namespace state_machine = rcppsw::patterns::state_machine;
namespace ta = rcppsw:task_allocation;

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/

// get pointer to taskable object (crw_fsm)
ee_max_fsm::ee_max_fsm(controller::saa_subsystem* const saa, ta::taskable* const taskable)
    : base_foraging_fsm(saa, ST_MAX_STATES),
      ta::taskable* taskable_fsm(taskable),
      ER_CLIENT_INIT("fordyca.fsm.depth0.ee_max"),
      HFSM_CONSTRUCT_STATE(start, hfsm::top_state()),
      HFSM_CONSTRUCT_STATE(foraging, hfsm::top_state()),
      HFSM_CONSTRUCT_STATE(retreating, hfsm::top_state()),
      HFSM_CONSTRUCT_STATE(charging, hfsm::top_state()),
      m_explore_fsm(saa,
                    std::make_unique<controller::random_explore_behavior>(saa),
                    std::bind(&ee_max_fsm::block_detected, this)),
      mc_state_map{HFSM_STATE_MAP_ENTRY_EX(&start),
                   HFSM_STATE_MAP_ENTRY_EX(&foraging),
                   HFSM_STATE_MAP_ENTRY_EX(&retreating),
                   HFSM_STATE_MAP_ENTRY_EX(&charging)} {}

/*******************************************************************************
 * States
 ******************************************************************************/
HFSM_STATE_DEFINE(ee_max_fsm, start, state_machine::event_data) {
  /* first time running FSM */
  if (state_machine::event_type::NORMAL == data->type()) {
    internal_event(ST_ACQUIRE_BLOCK);
    return controller::foraging_signal::HANDLED;
  }
  if (state_machine::event_type::CHILD == data->type()) {
    if (controller::foraging_signal::LEFT_NEST == data->signal()) {
      m_explore_fsm.task_start(nullptr);
      internal_event(ST_ACQUIRE_BLOCK);
      return controller::foraging_signal::HANDLED;
    } else if (controller::foraging_signal::ENTERED_NEST == data->signal()) {
      internal_event(ST_WAIT_FOR_BLOCK_DROP);
      return controller::foraging_signal::HANDLED;
    }
  }
  ER_FATAL_SENTINEL("Unhandled signal");
  return controller::foraging_signal::HANDLED;
}

HFSM_STATE_DEFINE_ND(ee_max_fsm, foraging) {

}

HFSM_STATE_DEFINE_ND(ee_max_fsm, retreating) {

}

HFSM_STATE_DEFINE_ND(ee_max_fsm, charging) {
  
}

/*
HFSM_STATE_DEFINE_ND(ee_max_fsm, acquire_block) {
  if (m_explore_fsm.task_finished()) {
    internal_event(ST_WAIT_FOR_BLOCK_PICKUP);
  } else {
    m_explore_fsm.task_execute();
  }
  return controller::foraging_signal::HANDLED;
}

HFSM_STATE_DEFINE(ee_max_fsm, wait_for_block_pickup, state_machine::event_data) {
  if (controller::foraging_signal::BLOCK_PICKUP == data->signal()) {
    m_explore_fsm.task_reset();
    ER_INFO("Block pickup signal received");
    internal_event(ST_RETURN_TO_NEST);
    // Tony: robot will also enter this state when battery is low handled individually by robot
  } else if (controller::foraging_signal::BLOCK_VANISHED == data->signal()) {
    m_explore_fsm.task_reset();
    internal_event(ST_ACQUIRE_BLOCK);
  }
  return controller::foraging_signal::HANDLED;
}

HFSM_STATE_DEFINE(ee_max_fsm, wait_for_block_drop, state_machine::event_data) {
  if (controller::foraging_signal::BLOCK_DROP == data->signal()) {
    m_explore_fsm.task_reset();
    ER_INFO("Block drop signal received");
    internal_event(ST_LEAVING_NEST);
  }
  return controller::foraging_signal::HANDLED;

  FSM_STATE_DEFINE_ND(ee_max_fsm, charging) {
    if (m_explore_fsm.task_finished()) {
      internal_event(ST_CHARGING);
    }
    // Tony: robot will handle next when to exit nest aka charged, time spent, and P-val allowed
  }
}
*/

/*******************************************************************************
 * Metrics
 ******************************************************************************/
bool ee_max_fsm::is_exploring_for_goal(void) const {
  return current_state() == ST_ACQUIRE_BLOCK;
} /* is_exploring_for_goal() */

bool ee_max_fsm::goal_acquired(void) const {
  if (acquisition_goal_type::kBlock == acquisition_goal()) {
    return current_state() == ST_WAIT_FOR_BLOCK_PICKUP;
  } else if (transport_goal_type::kNest == block_transport_goal()) {
    return current_state() == ST_WAIT_FOR_BLOCK_DROP;
  }
  return false;
} /* goal_acquired() */

/*******************************************************************************
 * Collision Metrics
 ******************************************************************************/
__rcsw_pure bool ee_max_fsm::in_collision_avoidance(void) const {
  return (m_explore_fsm.task_running() &&
          m_explore_fsm.in_collision_avoidance()) ||
         base_foraging_fsm::in_collision_avoidance();
} /* in_collision_avoidance() */

__rcsw_pure bool ee_max_fsm::entered_collision_avoidance(void) const {
  return (m_explore_fsm.task_running() &&
          m_explore_fsm.entered_collision_avoidance()) ||
         base_foraging_fsm::entered_collision_avoidance();
} /* entered_collision_avoidance() */

__rcsw_pure bool ee_max_fsm::exited_collision_avoidance(void) const {
  return (m_explore_fsm.task_running() &&
          m_explore_fsm.exited_collision_avoidance()) ||
         base_foraging_fsm::exited_collision_avoidance();
} /* exited_collision_avoidance() */

__rcsw_pure uint ee_max_fsm::collision_avoidance_duration(void) const {
  if (m_explore_fsm.task_running()) {
    return m_explore_fsm.collision_avoidance_duration();
  } else {
    return base_foraging_fsm::collision_avoidance_duration();
  }
  return 0;
} /* collision_avoidance_duration() */

/*******************************************************************************
 * General Member Functions
 ******************************************************************************/
void ee_max_fsm::init(void) {
  base_foraging_fsm::init();
  m_explore_fsm.init();
} /* init() */

void ee_max_fsm::run(void) {
  inject_event(controller::foraging_signal::FSM_RUN,
               state_machine::event_type::NORMAL);
} /* run() */

bool ee_max_fsm::block_detected(void) const {
  return saa_subsystem()->sensing()->block_detected();
} /* block_detected() */

transport_goal_type ee_max_fsm::block_transport_goal(void) const {
  if (ST_RETURN_TO_NEST == current_state() ||
      ST_WAIT_FOR_BLOCK_DROP == current_state()) {
    return transport_goal_type::kNest;
  }
  return transport_goal_type::kNone;
} /* block_transport_goal() */

acquisition_goal_type ee_max_fsm::acquisition_goal(void) const {
  if (ST_ACQUIRE_BLOCK == current_state() ||
      ST_WAIT_FOR_BLOCK_PICKUP == current_state()) {
    return acquisition_goal_type::kBlock;
  }
  return acquisition_goal_type::kNone;
} /* block_transport_goal() */

NS_END(depth0, fsm, fordyca);
