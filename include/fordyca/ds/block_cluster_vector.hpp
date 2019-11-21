/**
 * \file block_cluster_vector.hpp
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

#ifndef INCLUDE_FORDYCA_DS_BLOCK_CLUSTER_VECTOR_HPP_
#define INCLUDE_FORDYCA_DS_BLOCK_CLUSTER_VECTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <vector>

#include "fordyca/fordyca.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);
namespace repr {
class block_cluster;
} // namespace repr

NS_START(ds);

using block_cluster_vector_type = const repr::block_cluster*;

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/
using block_cluster_vector = std::vector<block_cluster_vector_type>;

NS_END(ds, fordyca);

#endif /* INCLUDE_FORDYCA_DS_BLOCK_CLUSTER_VECTOR_HPP_ */
