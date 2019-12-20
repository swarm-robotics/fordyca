/**
 * \file cache_collector.hpp
 *
 * \copyright 2018 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_TASKS_DEPTH2_CACHE_COLLECTOR_HPP_
#define INCLUDE_FORDYCA_TASKS_DEPTH2_CACHE_COLLECTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <utility>
#include <memory>

#include "fordyca/tasks/depth1/collector.hpp"
#include "fordyca/tasks/depth2/foraging_task.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, tasks, depth2);

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * \class cache_collector
 * \ingroup tasks depth2
 *
 * \brief Task in which robots locate a cache and bring a block from it to the
 * nest. It is abortable, and has one task interface.
 */
class cache_collector final : public depth1::collector {
 public:
  cache_collector(const cta::config::task_alloc_config* config,
                  std::unique_ptr<cta::taskable> mechanism) :
      collector(config,
                depth2::foraging_task::kCacheCollectorName,
                std::move(mechanism)) {}
};

NS_END(depth2, tasks, fordyca);

#endif /* INCLUDE_FORDYCA_TASKS_DEPTH2_CACHE_COLLECTOR_HPP_ */
