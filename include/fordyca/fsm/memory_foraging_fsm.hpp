/**
 * @file memory_foraging_fsm.hpp
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

#ifndef INCLUDE_FORDYCA_FSM_MEMORY_FORAGING_FSM_HPP_
#define INCLUDE_FORDYCA_FSM_MEMORY_FORAGING_FSM_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/fsm/vector_fsm.hpp"
#include "fordyca/fsm/base_foraging_fsm.hpp"
#include "fordyca/fsm/block_to_nest_fsm.hpp"
#include "rcppsw/patterns/visitor/visitable.hpp"
#include "rcppsw/task_allocation/taskable.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace params {
struct fsm_params;
} /* namespace params */

namespace controller {
class sensor_manager;
class actuator_manager;
} /* namespace controller */

namespace representation {
class perceived_arena_map;
} /* namespace representation */

namespace visitor = rcppsw::patterns::visitor;
namespace task_allocation = rcppsw::task_allocation;

NS_START(fsm);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/

/**
 * @brief The FSM for an unpartitioned foraging task. Each robot executing this
 * FSM will locate for a block (either a known block or via random exploration),
 * pickup the block and bring it all the way back to the nest.
 */
class memory_foraging_fsm : public base_foraging_fsm,
                            public task_allocation::taskable,
                            public visitor::visitable<memory_foraging_fsm> {
 public:
  memory_foraging_fsm(
      const struct params::fsm_params* params,
      const std::shared_ptr<rcppsw::common::er_server>& server,
      const std::shared_ptr<controller::sensor_manager>& sensors,
      const std::shared_ptr<controller::actuator_manager>& actuators,
      const std::shared_ptr<const representation::perceived_arena_map>& map);

  /* taskable overrides */

  /**
   * @brief Reset the memory foraging task to a state where it can be restarted.
   */
  void task_reset(void) override { init(); }

  /**
   * @brief Run the memory foraging task.
   */
  void task_execute(void) override;

  /**
   * @brief Determine if a block has been retrieved, brought to the nest, and
   * the robot has left the nest, ready for its next task.
   *
   * @return \c TRUE if the condition is met, \c FALSE otherwise.
   */
  bool task_finished(void) const override { return ST_FINISHED == current_state(); }

  /**
   * @brief Reset the FSM.
   */
  void init(void) override;

  /**
   * @brief Get if the robot is currently searching for a block within the arena
   * (either vectoring towards a known block, or exploring for one).
   *
   * @return TRUE if the condition is met, FALSE otherwise.
   */
  bool is_searching_for_block(void) const { return m_block_fsm.is_searching_for_block(); }

  bool is_exploring(void) const { return m_block_fsm.is_exploring(); }
  bool is_vectoring(void) const { return m_block_fsm.is_vectoring(); }
  bool is_avoiding_collision(void) const {
    return m_block_fsm.is_avoiding_collision();
  }
  bool is_returning(void) const { return m_block_fsm.is_returning(); }

 protected:
  enum fsm_states {
    ST_START,
    ST_ACQUIRE_FREE_BLOCK,    /* superstate for finding a block */
    ST_LEAVING_NEST,          /* Block dropped in nest--time to go */
    ST_FINISHED,
    ST_MAX_STATES
  };

 private:
  /* inherited states */
  HFSM_STATE_INHERIT(base_foraging_fsm, leaving_nest,
                     state_machine::no_event_data);

  HFSM_ENTRY_INHERIT(base_foraging_fsm, entry_leaving_nest,
                     state_machine::no_event_data);

  /* memory foraging states */
  HFSM_STATE_DECLARE(memory_foraging_fsm, start, state_machine::no_event_data);
  HFSM_STATE_DECLARE(memory_foraging_fsm, block_to_nest,
                     state_machine::no_event_data);
  HFSM_STATE_DECLARE(memory_foraging_fsm, finished, state_machine::no_event_data);

  HFSM_DEFINE_STATE_MAP_ACCESSOR(state_map_ex, index) override {
  HFSM_DEFINE_STATE_MAP(state_map_ex, kSTATE_MAP) {
    HFSM_STATE_MAP_ENTRY_EX(&start, hfsm::top_state()),
        HFSM_STATE_MAP_ENTRY_EX(&block_to_nest, hfsm::top_state()),
        HFSM_STATE_MAP_ENTRY_EX_ALL(&leaving_nest, hfsm::top_state(),
                                    NULL,
                                    &entry_leaving_nest, NULL),
        HFSM_STATE_MAP_ENTRY_EX(&finished, hfsm::top_state()),
    };
  HFSM_VERIFY_STATE_MAP(state_map_ex, kSTATE_MAP);
  return &kSTATE_MAP[index];
  }

  memory_foraging_fsm(const memory_foraging_fsm& fsm) = delete;
  memory_foraging_fsm& operator=(const memory_foraging_fsm& fsm) = delete;

  /* data members */
  block_to_nest_fsm m_block_fsm;
};

NS_END(fsm, fordyca);

#endif /* INCLUDE_FORDYCA_FSM_MEMORY_FORAGING_FSM_HPP_ */
