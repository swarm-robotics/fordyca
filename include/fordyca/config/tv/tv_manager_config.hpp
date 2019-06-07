/**
 * @file tv_manager_config.hpp
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

#ifndef INCLUDE_FORDYCA_CONFIG_TV_TV_MANAGER_CONFIG_HPP_
#define INCLUDE_FORDYCA_CONFIG_TV_TV_MANAGER_CONFIG_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include "rcppsw/config/base_config.hpp"
#include "rcppsw/control/config/waveform_config.hpp"
#include "fordyca/nsalias.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, config, tv);

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * @struct tv_manager_config
 * @ingroup fordyca config tv
 */
struct tv_manager_config : public rconfig::base_config {
  rct::config::waveform_config block_manipulation_penalty{};
  rct::config::waveform_config block_carry_throttle{};
  rct::config::waveform_config cache_usage_penalty{};
};

NS_END(tv, config, fordyca);

#endif /* INCLUDE_FORDYCA_CONFIG_TV_TV_MANAGER_CONFIG_HPP_ */
