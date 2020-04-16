/**
 * \file collector.hpp
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

#ifndef INCLUDE_FORDYCA_TASKS_DEPTH1_COLLECTOR_HPP_
#define INCLUDE_FORDYCA_TASKS_DEPTH1_COLLECTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <memory>

#include "cosm/ta/abort_probability.hpp"
#include "cosm/ta/polled_task.hpp"

#include "fordyca/tasks/depth1/foraging_task.hpp"
#include "fordyca/events/existing_cache_interactor.hpp"
#include "fordyca/events/nest_interactor.hpp"
#include "rcppsw/er/client.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, tasks, depth1);

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * \class collector
 * \ingroup tasks depth1
 *
 * \brief Task in which robots locate a cache and bring a block from it to the
 * nest. It is abortable, and has one task interface.
 */
class collector : public foraging_task,
                  public events::existing_cache_interactor,
                  public events::nest_interactor,
                  public rer::client<collector> {
 public:
  collector(const struct cta::config::task_alloc_config* config,
            const std::string& name,
            std::unique_ptr<cta::taskable> mechanism);
  collector(const struct cta::config::task_alloc_config* config,
            std::unique_ptr<cta::taskable> mechanism);

  /*
   * Event handling. This CANNOT be done using the regular visitor pattern,
   * because when visiting a \ref existing_cache_interactor, you have no way to
   * way which task the object ACTUALLY is without using a set of if()
   * statements, which is a brittle design. This is not the cleanest, but is
   * still more elegant than the alternative.
   */
  void accept(events::detail::robot_cached_block_pickup& visitor) override;
  void accept(events::detail::robot_nest_block_drop& visitor) override;
  void accept(events::detail::cache_vanished& visitor) override;
  void accept(events::detail::robot_cache_block_drop&) override {}

  /* goal acquisition metrics */
  RCPPSW_WRAP_OVERRIDE_DECL(bool, goal_acquired, const);
  RCPPSW_WRAP_OVERRIDE_DECL(exp_status, is_exploring_for_goal, const);
  RCPPSW_WRAP_OVERRIDE_DECL(bool, is_vectoring_to_goal, const);
  RCPPSW_WRAP_OVERRIDE_DECL(cfsm::metrics::goal_acq_metrics::goal_type,
                            acquisition_goal,
                            const);
  RCPPSW_WRAP_OVERRIDE_DECL(rmath::vector2z, acquisition_loc, const);
  RCPPSW_WRAP_OVERRIDE_DECL(rmath::vector2z, current_explore_loc, const);
  RCPPSW_WRAP_OVERRIDE_DECL(rmath::vector2z, current_vector_loc, const);
  RCPPSW_WRAP_OVERRIDE_DECL(rtypes::type_uuid, entity_acquired_id, const);

  /* block transportation */
  RCPPSW_WRAP_OVERRIDE_DECL(fsm::foraging_transport_goal,
                            block_transport_goal,
                            const);

  /* task metrics */
  bool task_at_interface(void) const override RCSW_PURE;
  bool task_completed(void) const override { return task_finished(); }

  void task_start(const cta::taskable_argument*) override;
  double abort_prob_calc(void) override RCSW_PURE;
  rtypes::timestep interface_time_calc(uint interface,
                                       const rtypes::timestep& start_time) override;
  void active_interface_update(int) override;
};

NS_END(depth1, tasks, fordyca);

#endif /* INCLUDE_FORDYCA_TASKS_DEPTH1_COLLECTOR_HPP_ */
