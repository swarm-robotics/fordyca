/**
 * @file cache_finisher.hpp
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

#ifndef INCLUDE_FORDYCA_TASKS_DEPTH2_CACHE_FINISHER_HPP_
#define INCLUDE_FORDYCA_TASKS_DEPTH2_CACHE_FINISHER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/tasks/depth2/foraging_task.hpp"
#include "rcppsw/patterns/visitor/visitable.hpp"
#include "rcppsw/task_allocation/abort_probability.hpp"
#include "rcppsw/task_allocation/polled_task.hpp"
#include "fordyca/tasks/depth2/new_cache_interactor.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, tasks, depth2);

namespace task_allocation = rcppsw::task_allocation;

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * @class cache_finisher
 * @ingroup tasks depth2
 *
 * @brief Task in which robots locate a free block and drop it next to/on top of
 * a free block in the arena to finish the creation of a new cache. It is
 * abortable, and has one task interface.
 */
class cache_finisher : public task_allocation::polled_task,
                      public foraging_task,
                      public new_cache_interactor {
 public:
  cache_finisher(const struct task_allocation::task_params* params,
            std::unique_ptr<task_allocation::taskable>& mechanism);

  /* event handling */
  void accept(events::free_block_drop& visitor) override;

  /* base FSM metrics */
  bool is_avoiding_collision(void) const override;

  /* FSM goal acquisition metrics */
  goal_acquisition_metrics::goal_type goal(void) const override {
    return goal_acquisition_metrics::goal_type::kNewCache;
  }
  bool is_exploring_for_goal(void) const override;
  bool is_vectoring_to_goal(void) const override;
  bool goal_acquired(void) const override;

  /* Block transportation status */
  bool is_transporting_to_nest(void) const override { return false; }
  bool is_transporting_to_cache(void) const override;

  /* task metrics */
  bool at_interface(void) const override;

  void task_start(const task_allocation::taskable_argument*) override;
  double current_time(void) const override;
  double calc_abort_prob(void) override;
  double calc_interface_time(double start_time) override;

 private:
  // clang-format off
  bool                               m_interface_complete{false};
  task_allocation::abort_probability m_abort_prob;
  // clang-format on
};

NS_END(depth2, tasks, fordyca);

#endif /* INCLUDE_FORDYCA_TASKS_DEPTH2_CACHE_FINISHER_HPP_ */
