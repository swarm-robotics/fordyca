/**
 * @file perceived_cell2D.cpp
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
#include "fordyca/representation/perceived_cell2D.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, representation);

/*******************************************************************************
 * Constants
 ******************************************************************************/
const double perceived_cell2D::kEpsilon = 0.001;

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void perceived_cell2D::update_relevance(void) {
  if (m_cell.state_is_known()) {
    m_relevance = std::max(0.0, m_relevance - m_delta);
    if (m_relevance < kEpsilon) {
      m_cell.event_unknown();
    }
  }
} /* update_relevance() */

void perceived_cell2D::event_encounter(cell2D_fsm::state state, block* block) {
  switch (state) {
    case cell2D_fsm::ST_UNKNOWN:
      m_cell.event_unknown();
      break;
    case cell2D_fsm::ST_EMPTY:
      m_cell.event_empty();
      break;
    case cell2D_fsm::ST_HAS_BLOCK:
      m_cell.event_has_block(block);
      break;
    default:
      break;
  } /* switch() */
  m_relevance += 1.0;
} /* encounter() */

NS_END(representation, fordyca);
