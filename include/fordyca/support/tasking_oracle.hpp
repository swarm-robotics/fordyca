/**
 * @file tasking_oracle.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_TASKING_ORACLE_HPP_
#define INCLUDE_FORDYCA_SUPPORT_TASKING_ORACLE_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <boost/variant.hpp>
#include <map>
#include <string>

#include "rcppsw/common/common.hpp"
#include "rcppsw/er/client.hpp"
#include "rcppsw/task_allocation/time_estimate.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace rcppsw { namespace task_allocation {
class bifurcating_tdgraph;
class bifurcating_tdgraph_executive;
class polled_task;
}} // namespace rcppsw::task_allocation

NS_START(fordyca, support);
namespace ta = rcppsw::task_allocation;
namespace er = rcppsw::er;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class tasking_oracle
 * @ingroup support
 *
 * @brief Repository of perfect knowledge about swarm level task
 * allocation. Used to provide an upper bound on the performance of different
 * allocation methods.
 */
class tasking_oracle : public er::client<tasking_oracle> {
 public:
  using mapped_type = boost::variant<ta::time_estimate>;

  explicit tasking_oracle(const ta::bifurcating_tdgraph* graph);

  /**
   * @brief Ask the oracle something.
   *
   * @param query The question to ask. Currently supports:
   *
   *              exec_est.<task name>
   *
   * @return The answer to the query. Empty answer if query was ill-formed.
   */
  mapped_type ask(const std::string& query) const;

  /**
   * @brief Adds the oracle to the task finish and task abort callback lists for
   * the specified executive. Should be called once during initialization to
   * attach the oracle to each robot so that it can build a perfect map of task
   * allocation information as the simulation progresses.
   */
  void listener_add(ta::bifurcating_tdgraph_executive* executive);

  void task_abort_cb(const ta::polled_task* task);
  void task_finish_cb(const ta::polled_task* task);

 private:
  // clang-format off
  std::map<std::string, mapped_type> m_map{};
  // clang-format on
};

NS_END(support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_TASKING_ORACLE_HPP_ */
