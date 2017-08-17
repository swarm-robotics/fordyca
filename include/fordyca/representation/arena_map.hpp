/**
 * @file arena_map.hpp
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

#ifndef INCLUDE_FORDYCA_REPRESENTATION_ARENA_MAP_HPP_
#define INCLUDE_FORDYCA_REPRESENTATION_ARENA_MAP_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <vector>
#include "fordyca/representation/grid2D.hpp"
#include "fordyca/representation/block.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, representation);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
class arena_map {
 public:
  explicit arena_map(const struct grid_params* params) :
      m_blocks(params->block.n_blocks, block(params->block.dimension)),
      m_grid(params) {}

  std::vector<block>& blocks(void) { return m_blocks; }
  cell2D& access(size_t i, size_t j) { return m_grid.access(i, j); }

 private:
  std::vector<block> m_blocks;
  grid2D m_grid;
};

NS_END(representation, fordyca);

#endif /* INCLUDE_FORDYCA_REPRESENTATION_ARENA_MAP_HPP_ */
