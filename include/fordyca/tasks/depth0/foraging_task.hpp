/**
 * @file depth0/foraging_task.hpp
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

#ifndef INCLUDE_FORDYCA_TASKS_DEPTH0_FORAGING_TASK_HPP_
#define INCLUDE_FORDYCA_TASKS_DEPTH0_FORAGING_TASK_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <memory>
#include <boost/variant.hpp>

#include "fordyca/tasks/base_foraging_task.hpp"
#include "rcppsw/patterns/visitor/polymorphic_visitable.hpp"
#include "fordyca/events/nest_interactor.hpp"
#include "fordyca/events/free_block_interactor.hpp"
#include "rcppsw/ta/polled_task.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace rcppsw { namespace ta { namespace config { struct task_alloc_config; }}}

NS_START(fordyca, tasks, depth0);
class generalist;

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * @class foraging_task
 * @ingroup fordyca tasks depth0
 *
 * @brief Interface specifying the visit set for all depth0 foraging tasks in
 * FORDYCA.
 *
 * Not all tasks need all events, but it is convenient both from a design point
 * of view as well as not having to fight with the compiler as much if you do it
 * this way.
 */
class foraging_task
    : public base_foraging_task,
      public events::nest_interactor,
      public events::free_block_interactor,
      public rta::polled_task {
 public:
  using variant_type = boost::variant<generalist*>;

  static constexpr char kGeneralistName[] = "Generalist";

  foraging_task(const std::string& name,
                const rta::config::task_alloc_config* config,
                std::unique_ptr<rta::taskable> mechanism);

  ~foraging_task(void) override = default;

  static bool task_in_depth0(const rta::polled_task* task);
};

NS_END(depth0, tasks, fordyca);

#endif /* INCLUDE_FORDYCA_TASKS_DEPTH0_FORAGING_TASK_HPP_ */
