/**
 * @file caches_parser.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/config/caches/caches_parser.hpp"
#include "rcppsw/utils/line_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, config, caches);

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
constexpr char caches_parser::kXMLRoot[];

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void caches_parser::parse(const ticpp::Element& node) {
  if (nullptr != node.FirstChild(kXMLRoot, false)) {
    ticpp::Element cnode = node_get(node, kXMLRoot);
    m_config =
        std::make_shared<std::remove_reference<decltype(*m_config)>::type>();

    m_static.parse(cnode);
    m_dynamic.parse(cnode);
    m_config->static_ = *m_static.config_get();
    m_config->dynamic = *m_dynamic.config_get();

    XML_PARSE_ATTR(cnode, m_config, dimension);
    m_parsed = true;
  }
} /* parse() */

__rcsw_pure bool caches_parser::validate(void) const {
  if (m_parsed) {
    CHECK(m_config->dimension > 0.0);
    return m_dynamic.validate() && m_static.validate();
  }

error:
  return false;
} /* validate() */

NS_END(caches, config, fordyca);
