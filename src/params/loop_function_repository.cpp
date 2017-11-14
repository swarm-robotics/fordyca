/**
 * @file loop_function_repository.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/params/loop_function_repository.hpp"
#include "fordyca/params/arena_map_parser.hpp"
#include "fordyca/params/metrics_parser.hpp"
#include "fordyca/params/loop_functions_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, params);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
loop_function_repository::loop_function_repository(void) {
  factory().register_type<arena_map_parser>("arena_map");
  factory().register_type<metrics_parser>("metrics");
  factory().register_type<loop_functions_parser>("loop_functions");
  parsers()["arena_map"]        = factory().create("arena_map").get();
  parsers()["metrics"]          = factory().create("metrics").get();
  parsers()["loop_functions"]   = factory().create("loop_functions").get();
}

NS_END(params, fordyca);
