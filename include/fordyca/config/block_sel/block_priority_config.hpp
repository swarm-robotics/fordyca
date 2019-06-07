/**
 * @file block_priority_config.hpp
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

#ifndef INCLUDE_FORDYCA_CONFIG_BLOCK_SEL_BLOCK_PRIORITY_CONFIG_HPP_
#define INCLUDE_FORDYCA_CONFIG_BLOCK_SEL_BLOCK_PRIORITY_CONFIG_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include "rcppsw/config/base_config.hpp"
#include "fordyca/nsalias.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, config);

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * @struct block_priority_config
 * @ingroup fordyca config block_sel
 */
struct block_priority_config : public rconfig::base_config {
  int cube{-1};
  int ramp{-1};
};

NS_END(config, fordyca);

#endif /* INCLUDE_FORDYCA_CONFIG_BLOCK_SEL_BLOCK_PRIORITY_CONFIG_HPP_ */
