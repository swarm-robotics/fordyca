/**
 * @file perception_parser.cpp
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
#include "fordyca/params/perception/perception_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, params, perception);

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
constexpr char perception_parser::kXMLRoot[];

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void perception_parser::parse(const ticpp::Element& node) {
  /*
   * Not all controllers use a perception subsystem
   */
  if (nullptr != node.FirstChild(kXMLRoot, false)) {
    ticpp::Element onode = get_node(const_cast<ticpp::Element&>(node), kXMLRoot);
    m_params =
        std::make_shared<std::remove_reference<decltype(*m_params)>::type>();

    m_occupancy.parse(onode);
    m_pheromone.parse(onode);
    m_params->occupancy_grid = *m_occupancy.parse_results();
    m_params->pheromone = *m_pheromone.parse_results();
    m_parsed = true;
  }
} /* parse() */

__rcsw_pure bool perception_parser::validate(void) const {
  if (m_parsed) {
    return m_occupancy.validate() && m_pheromone.validate();
  }
  return true;
} /* validate() */

NS_END(perception, params, fordyca);