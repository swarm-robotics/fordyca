/**
 * @file tasks_fwd.hpp
 *
 * @copyright 2019 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_TASKS_TASKS_FWD_HPP_
#define INCLUDE_FORDYCA_TASKS_TASKS_FWD_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/common/common.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(fordyca, tasks);
class base_foraging_task;
namespace depth0 {
class generalist;
}
namespace depth1 {
class collector;
class harvester;
} // namespace depth1
namespace depth2 {
class cache_starter;
class cache_finisher;
class cache_transferer;
class cache_collector;
} // namespace depth2

NS_END(fordyca, tasks);

#endif /* INCLUDE_FORDYCA_TASKS_TASKS_FWD_HPP_ */
