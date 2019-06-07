/**
 * @file exploration_config.hpp
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

#ifndef INCLUDE_FORDYCA_CONFIG_EXPLORATION_CONFIG_HPP_
#define INCLUDE_FORDYCA_CONFIG_EXPLORATION_CONFIG_HPP_

/*******************************************************************************
  * Includes
******************************************************************************/
#include <string>
#include "fordyca/nsalias.hpp"
#include "rcppsw/config/base_config.hpp"

/*******************************************************************************
  * Namespaces
******************************************************************************/
NS_START(fordyca, config);

/*******************************************************************************
  * Structure Definitions
******************************************************************************/
/**
  * @struct exploration_config
  * @ingroup fordyca config
  */
struct exploration_config : public rconfig::base_config {
  std::string block_strategy{};
  std::string cache_strategy{};
};

NS_END(config, fordyca);

#endif /* INCLUDE_FORDYCA_CONFIG_EXPLORATION_CONFIG_HPP_ */
