/**
 * @file foraging_task.hpp
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

#ifndef INCLUDE_FORDYCA_TASKS_FORAGING_TASK_HPP_
#define INCLUDE_FORDYCA_TASKS_FORAGING_TASK_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>

#include "fordyca/metrics/fsm/block_acquisition_metrics.hpp"
#include "fordyca/metrics/fsm/block_transport_metrics.hpp"
#include "fordyca/tasks/argument.hpp"
#include "rcppsw/metrics/tasks/execution_metrics.hpp"
#include "rcppsw/patterns/visitor/polymorphic_visitable.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace events {
class free_block_pickup;
class nest_block_drop;
} // namespace events

namespace visitor = rcppsw::patterns::visitor;

NS_START(tasks);

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * @class foraging_task
 * @ingroup tasks
 *
 * @brief Interface specifying the visit set common to all foraging tasks in
 * FORDYCA.
 *
 * While it is true that some derived tasks don't use/need some of the metrics
 * they have to define, NOT putting them all here in the base class making
 * collection much more awkward.
 */
class foraging_task
    : public rcppsw::metrics::tasks::execution_metrics,
      public metrics::fsm::block_acquisition_metrics,
      public metrics::fsm::block_transport_metrics,
      public visitor::polymorphic_visitable<foraging_task,
                                            events::free_block_pickup,
                                            events::nest_block_drop> {
 public:
  static constexpr char kCollectorName[] = "Collector";
  static constexpr char kHarvesterName[] = "Harvester";
  static constexpr char kGeneralistName[] = "Generalist";
  static constexpr char kCacheStarterName[] = "Cache Starter";

  explicit foraging_task(const std::string& name) : mc_name(name) {}

  std::string name(void) const { return mc_name; }

 private:
  const std::string mc_name;
};

NS_END(tasks, fordyca);

#endif /* INCLUDE_FORDYCA_TASKS_FORAGING_TASK_HPP_ */
