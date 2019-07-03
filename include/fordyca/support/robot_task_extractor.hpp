/**
 * @file robot_task_extractor.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_ROBOT_TASK_EXTRACTOR_HPP_
#define INCLUDE_FORDYCA_SUPPORT_ROBOT_TASK_EXTRACTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/common/common.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(fordyca, support);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @struct robot_task_extractor
 * @ingroup fordyca support
 *
 * @brief Given a robot controller of type T, extract the ID of its current
 * task. This is used in computing task distribution entropy in depth1, depth2
 * in conjunction with boost::variant.
 */
template <class ControllerType>
struct robot_task_extractor {
  robot_task_extractor(void) = default;

  int operator()(const ControllerType* const c) const {
    return c->current_task_id();
  }
};

NS_END(support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_ROBOT_TASK_EXTRACTOR_HPP_ */