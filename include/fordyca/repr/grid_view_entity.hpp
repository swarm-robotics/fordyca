/**
 * @file grid_view_entity.hpp
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

#ifndef INCLUDE_FORDYCA_REPR_GRID_VIEW_ENTITY_HPP_
#define INCLUDE_FORDYCA_REPR_GRID_VIEW_ENTITY_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "fordyca/ds/arena_grid.hpp"
#include "fordyca/nsalias.hpp"
#include "fordyca/repr/base_entity.hpp"
#include "rcppsw/math/range.hpp"
#include "rcppsw/math/vector2.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, repr);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class grid_view_entity
 * @ingroup fordyca repr
 *
 * @brief Representation of an entity in the arena that:
 *
 * - Spans multiple cells in the arena.
 * - Does not "exist" in the sense that it is not detectable by robots. It lives
 *   on the same level of abstraction as the arena grid (hence the class name).
 * - Has no concept of movability/immovability (again, it is abstract).
 */
template <class T>
class grid_view_entity : public base_entity {
 public:
  explicit grid_view_entity(const T& view) : grid_view_entity{view, -1} {}

  grid_view_entity(const T& view, int id) : base_entity(id), m_view(view) {}

  ~grid_view_entity(void) override = default;

  const rmath::vector2u& anchor(void) const { return m_view.origin()->loc(); }

  /**
   * @brief Get the 2D space spanned by the entity in absolute
   * coordinates in the arena in X.
   */
  rmath::ranged xspan(void) const override {
    return rmath::ranged(m_view.origin()->loc().x() - 0.5 * m_view.shape()[0],
                         m_view.origin()->loc().x() + 0.5 * m_view.shape()[0]);
  }

  /**
   * @brief Get the 2D space spanned by the grid_cell entity in absolute
   * coordinates in the arena in Y.
   */
  rmath::ranged yspan(void) const override {
    return rmath::ranged(m_view.origin()->loc().y() - 0.5 * m_view.shape()[1],
                         m_view.origin()->loc().y() + 0.5 * m_view.shape()[1]);
  }

  /**
   * @brief Determine if a real-valued point lies within the extent of the
   * entity.
   *
   * @return \c TRUE if the condition is met, and \c FALSE otherwise.
   */
  bool contains_point(const rmath::vector2d& point) const {
    return xspan().contains(point.x()) && yspan().contains(point.y());
  }

  double xdim(void) const override { return m_view.shape()[0]; }
  double ydim(void) const override { return m_view.shape()[1]; }

  /**
   * @brief Get the cell associated with a particular grid location within the
   * LOS. Asserts that both coordinates are within the bounds of the grid
   * underlying the LOS.
   *
   * @param i The RELATIVE X coord within the LOS.
   * @param j The RELATIVE Y coord within the LOS.
   *
   * @return A reference to the cell.
   */
  const ds::cell2D& cell(uint i, uint j) const { return m_view[i][j]; }

 private:
  /* clang-format off */
  T m_view;
  /* clang-format on */
};

NS_END(repr, fordyca);

#endif /* INCLUDE_FORDYCA_REPR_GRID_VIEW_ENTITY_HPP_ */