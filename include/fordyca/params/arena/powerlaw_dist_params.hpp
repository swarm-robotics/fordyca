/**
 * @file powerlaw_dist_params.hpp
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

#ifndef INCLUDE_FORDYCA_PARAMS_ARENA_POWERLAW_DIST_PARAMS_HPP_
#define INCLUDE_FORDYCA_PARAMS_ARENA_POWERLAW_DIST_PARAMS_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/params/base_params.hpp"
#include "fordyca/nsalias.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, params, arena);

/*******************************************************************************
 * Structure Definitions
 ******************************************************************************/
/**
 * @struct powerlaw_dist_params
 * @ingroup fordyca params arena
 */
struct powerlaw_dist_params : public rparams::base_params {
  /**
   * @brief Min power of 2 for distribution.
   */
  uint pwr_min{0};

  /**
   * @brief Max power of 2 for distribution.
   */
  uint pwr_max{0};

  /**
   * @brief How many clusters to allocate in the arena.
   */
  uint n_clusters{0};
};

NS_END(arena, params, fordyca);

#endif /* INCLUDE_FORDYCA_PARAMS_ARENA_POWERLAW_DIST_PARAMS_HPP_ */
