/**
 * @file cache_sel_matrix.hpp
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

#ifndef INCLUDE_FORDYCA_CONTROLLER_CACHE_SEL_MATRIX_HPP_
#define INCLUDE_FORDYCA_CONTROLLER_CACHE_SEL_MATRIX_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <boost/variant.hpp>
#include <map>
#include <string>
#include <vector>


#include "rcppsw/common/common.hpp"
#include "rcppsw/math/range.hpp"
#include "rcppsw/math/vector2.hpp"
#include "fordyca/controller/cache_sel_exception.hpp"
#include "rcppsw/er/client.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);
namespace params {
struct cache_sel_matrix_params;
}
NS_START(controller);
namespace rmath = rcppsw::math;
namespace er = rcppsw::er;
using cache_sel_variant = boost::variant<double,
                                         rmath::vector2d,
                                         rmath::rangeu,
                                         std::vector<int>>;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/

/**
 * @class cache_sel_matrix
 * @ingroup controller
 *
 * @brief A dictionary of information needed by robots using various utility
 * functions to calculate the best:
 *
 * - existing cache
 * - new cache
 * - cache site
 *
 * This class may be separated into those components in the future if it makes
 * sense. For now, it is cleaner to have all three uses be in the same class.
 */
class cache_sel_matrix : public er::client<cache_sel_matrix>,
                         private std::map<std::string, cache_sel_variant> {
 public:
  static constexpr char kNestLoc[] = "nest_loc";
  static constexpr char kCacheProxDist[] = "cache_prox_dist";
  static constexpr char kBlockProxDist[] = "block_prox_dist";
  static constexpr char kNestProxDist[] = "nest_prox_dist";
  static constexpr char kSiteXRange[] = "site_xrange";
  static constexpr char kSiteYRange[] = "site_yrange";
  static constexpr char kPickupExceptions[] = "pickup_exceptions";
  static constexpr char kDropExceptions[] = "drop_exceptions";

  using std::map<std::string, cache_sel_variant>::find;
  cache_sel_matrix(const struct params::cache_sel_matrix_params* params,
                   const rmath::vector2d& nest_loc);
  ~cache_sel_matrix(void) override = default;

  /**
   * @brief Add a cache to the exception list, disqualifying it from being
   * selected as a cache to pick up a block from/drop a block in, regardless of
   * its utility value, the next time the robot runs the existing cache
   * selection algorithm to select a cache IF the previous usage of that cache
   * was not the same as the current usage (i.e. if they picked from the cache
   * last time but want to drop into it this time). Only needed for cache
   * transferer tasks.
   */
  void sel_exception_add(const cache_sel_exception& ex);

  /**
   * @brief Clear the exceptions list. This happens after a robot has executed
   * the task AFTER the task that dropped a block in/picked up a block from an
   * existing cache (i.e. there is a 1 task buffer between usages of the same
   * existing cache).
   */
  void sel_exceptions_clear(void);
};

NS_END(controller, fordyca);

#endif /* INCLUDE_FORDYCA_CONTROLLER_CACHE_SEL_MATRIX_HPP_ */
