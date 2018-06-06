/**
 * @file base_block_to_cache_fsm.hpp
 *
 * @copyright 2018 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_FSM_DEPTH1_BASE_BLOCK_TO_CACHE_FSM_HPP_
#define INCLUDE_FORDYCA_FSM_DEPTH1_BASE_BLOCK_TO_CACHE_FSM_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/task_allocation/taskable.hpp"
#include "rcppsw/patterns/visitor/visitable.hpp"
#include "fordyca/fsm/vector_fsm.hpp"
#include "fordyca/fsm/base_foraging_fsm.hpp"
#include "fordyca/fsm/acquire_block_fsm.hpp"
#include "fordyca/metrics/fsm/block_acquisition_metrics.hpp"
#include "fordyca/metrics/fsm/cache_acquisition_metrics.hpp"
#include "fordyca/metrics/fsm/block_transport_metrics.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace task_allocation = rcppsw::task_allocation;
namespace visitor = rcppsw::patterns::visitor;
namespace params { struct fsm_params; }
namespace representation { class perceived_arena_map; class block; }
NS_START(fsm, depth1);
class base_acquire_cache_fsm;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class base_block_to_cache_fsm
 * @ingroup fsm depth1
 *
 * @brief The FSM for the block-to-cache subtask.
 *
 * Each robot executing this FSM will locate a free block (either a known block
 * or via random exploration), pickup the block and bring it to the best cache
 * it knows about. Once it has done that it will signal that its task is
 * complete.
 */
class base_block_to_cache_fsm : public base_foraging_fsm,
                                public metrics::fsm::block_acquisition_metrics,
                                public metrics::fsm::cache_acquisition_metrics,
                                public metrics::fsm::block_transport_metrics,
                                public task_allocation::taskable,
                                public visitor::visitable_any<base_block_to_cache_fsm> {
 public:
  base_block_to_cache_fsm(
      const struct params::fsm_params* params,
      const std::shared_ptr<rcppsw::er::server>& server,
      const std::shared_ptr<controller::saa_subsystem>& saa,
      const std::shared_ptr<representation::perceived_arena_map>& map);

  base_block_to_cache_fsm(const base_block_to_cache_fsm& fsm) = delete;
  base_block_to_cache_fsm& operator=(const base_block_to_cache_fsm& fsm) = delete;

  /* taskable overrides */
  void task_execute(void) override;
  void task_start(const task_allocation::taskable_argument * arg) override;
  bool task_finished(void) const override { return ST_FINISHED == current_state(); }
  bool task_running(void) const override {
    return !(ST_FINISHED == current_state() || ST_START == current_state());
  }
  void task_reset(void) override { init(); }

  /* base FSM metrics */
  bool is_avoiding_collision(void) const override;

  /* block acquisition metrics */
  bool is_exploring_for_block(void) const override;
  bool is_acquiring_block(void) const override;
  bool is_vectoring_to_block(void) const override;
  bool block_acquired(void) const override;

  /* cache acquisition metrics */
  bool is_exploring_for_cache(void) const override;
  bool is_acquiring_cache(void) const override;
  bool is_vectoring_to_cache(void) const override;
  bool cache_acquired(void) const override;

  /* block transport metrics */
  bool is_transporting_to_nest(void) const override { return false; }
  bool is_transporting_to_cache(void) const override;

  /**
   * @brief Reset the FSM
   */
  void init(void) override;

 protected:
  enum fsm_states {
    ST_START,
    /**
     * Superstate for acquiring a free block.
     */
    ST_ACQUIRE_FREE_BLOCK,

    /**
     * A block has been acquired--wait for area to send the block pickup signal.
     */
    ST_WAIT_FOR_BLOCK_PICKUP,

    /**
     * We are transporting a carried block to a cache.
     */
    ST_TRANSPORT_TO_CACHE,

    /**
     * We have acquired a cache--wait for arena to send the block drop signal.
     */
    ST_WAIT_FOR_CACHE_DROP,

    /**
     * Block has been successfully dropped in a cache.
     */
    ST_FINISHED,
    ST_MAX_STATES,
  };

  virtual base_acquire_cache_fsm& cache_fsm(void) = 0;
  const base_acquire_cache_fsm& cache_fsm(void) const {
    return const_cast<base_block_to_cache_fsm*>(this)->cache_fsm(); }

 private:
  constexpr static uint kPICKUP_TIMEOUT = 100;

  /* inherited states */
  HFSM_ENTRY_INHERIT_ND(base_foraging_fsm, entry_wait_for_signal);

  /* block to cache states */
  HFSM_STATE_DECLARE(base_block_to_cache_fsm, start, state_machine::event_data);
  HFSM_STATE_DECLARE_ND(base_block_to_cache_fsm, acquire_free_block);
  HFSM_STATE_DECLARE(base_block_to_cache_fsm, wait_for_block_pickup,
                     state_machine::event_data);
  HFSM_STATE_DECLARE_ND(base_block_to_cache_fsm, transport_to_cache);
  HFSM_STATE_DECLARE(base_block_to_cache_fsm, wait_for_cache_drop,
                     state_machine::event_data);
  HFSM_STATE_DECLARE_ND(base_block_to_cache_fsm, finished);

  /**
   * @brief Defines the state map for the FSM.
   *
   * Note that the order of the states in the map MUST match the order of the
   * states in \enum fsm_states, or things will not work correctly.
   */
  HFSM_DEFINE_STATE_MAP_ACCESSOR(state_map_ex, index) override {
  return &mc_state_map[index];
  }

  // clang-format off
  uint              m_pickup_count;
  acquire_block_fsm m_block_fsm;
  // clang-format on

  HFSM_DECLARE_STATE_MAP(state_map_ex, mc_state_map, ST_MAX_STATES);
};

NS_END(depth1, fsm, fordyca);

#endif /* INCLUDE_FORDYCA_FSM_DEPTH1_BASE_BLOCK_TO_CACHE_FSM_HPP_ */