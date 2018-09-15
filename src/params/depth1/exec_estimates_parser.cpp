/**
 * @file exec_estimates_parser.cpp
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
#include "fordyca/params/depth1/exec_estimates_parser.hpp"
#include "rcppsw/utils/line_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, params, depth1);

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
constexpr char exec_estimates_parser::kXMLRoot[];

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void exec_estimates_parser::parse(const ticpp::Element& node) {
  depth0::exec_estimates_parser::parse(node);
  if (nullptr != node.FirstChild(kXMLRoot, false)) {
    ticpp::Element enode = get_node(const_cast<ticpp::Element&>(node), kXMLRoot);
    m_params =
        std::make_shared<std::remove_reference<decltype(*m_params)>::type>();
    *std::static_pointer_cast<depth0::exec_estimates_params>(m_params) =
        *depth0::exec_estimates_parser::parse_results();
    if (m_params->enabled) {
      XML_PARSE_PARAM(enode, m_params, harvester_range);
      XML_PARSE_PARAM(enode, m_params, collector_range);
    }
    parsed(true);
  }
} /* parse() */

void exec_estimates_parser::show(std::ostream& stream) const {
  if (!parsed()) {
    stream << build_header() << "<< Not Parsed >>" << std::endl
           << build_footer();
    return;
  }
  depth0::exec_estimates_parser::show(stream);
  stream << XML_PARAM_STR(m_params, harvester_range) << std::endl
         << XML_PARAM_STR(m_params, collector_range) << std::endl
         << build_footer();
} /* show() */

NS_END(depth1, params, fordyca);
