/**
 * @file block.cpp
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
#include <assert.h>
#include "fordyca/representation/block.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, representation);

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
bool block::contains_point(const argos::CVector2& point) {
  double x = real_loc().GetX();
  double y = real_loc().GetY();
  if (point.GetX() < (x + (.5 * xsize())) &&
      point.GetX() > (x - (.5 * xsize())) &&
      point.GetY() < (y + (.5 * xsize())) &&
      point.GetY() > (y - (.5 * xsize()))) {
    return true;
  }
  return false;
} /* contains_point() */

NS_END(representation, fordyca);
