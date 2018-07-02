/**
 * @file immovable_cell_entity.hpp
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

#ifndef INCLUDE_FORDYCA_REPRESENTATION_IMMOVABLE_CELL_ENTITY_HPP_
#define INCLUDE_FORDYCA_REPRESENTATION_IMMOVABLE_CELL_ENTITY_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <argos3/core/utility/math/vector2.h>
#include "fordyca/math/utils.hpp"
#include "rcppsw/math/dcoord.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, representation);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class immovable_cell_entity
 * @ingroup representation
 *
 * @brief A class representing objects that reside within one or more squares
 * within a 2D grid whose position CANNOT change during the lifetime of the
 * object.
 */
class immovable_cell_entity {
 public:
  /**
   * @brief Initialize a immovable entity with an initial location in the arena.
   */
  immovable_cell_entity(const argos::CVector2& loc, double resolution)
      : m_real_loc(loc),
        m_discrete_loc(math::rcoord_to_dcoord(loc, resolution)) {}

  virtual ~immovable_cell_entity(void) = default;

  /**
   * @brief Get the real location (center) of the object.
   */
  const argos::CVector2& real_loc(void) const { return m_real_loc; }

  /**
   * @brief Get the discretized coordinates of the center of the object, which
   * can be used to index into an arena_map.
   *
   */
  const rcppsw::math::dcoord2& discrete_loc(void) const {
    return m_discrete_loc;
  }

 private:
  // clang-format off
  argos::CVector2       m_real_loc;
  rcppsw::math::dcoord2 m_discrete_loc;
  // clang-format on
};

NS_END(representation, fordyca);

#endif /* INCLUDE_FORDYCA_REPRESENTATION_IMMOVABLE_CELL_ENTITY_HPP_ */
