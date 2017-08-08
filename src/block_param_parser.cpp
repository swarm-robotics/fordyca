/**
 * @file block_param_parser.cpp
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
#include "fordyca/params/block_param_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, params);

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void block_param_parser::parse(argos::TConfigurationNode& node) {
  m_params.reset(new struct block_params);
  argos::GetNodeAttribute(node, "n_blocks", m_params->n_blocks);
  argos::GetNodeAttribute(node, "radius", m_params->square_radius);
  argos::GetNodeAttribute(node, "dist_model", m_params->dist_model);
  m_params->square_radius *= m_params->square_radius;
} /* parse() */

void block_param_parser::show(std::ostream& stream) {
  stream << "Block params\n====================" << std::endl;
  stream << "n_items=" << m_params->n_blocks << std::endl;
  stream << "square_radius=" << m_params->square_radius << std::endl;
  stream << "dist_model=" << m_params->dist_model << std::endl;
} /* show() */

NS_END(params, fordyca);
