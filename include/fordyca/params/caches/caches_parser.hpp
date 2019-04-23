/**
 * @file caches_parser.hpp
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

#ifndef INCLUDE_FORDYCA_PARAMS_CACHES_CACHES_PARSER_HPP_
#define INCLUDE_FORDYCA_PARAMS_CACHES_CACHES_PARSER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>

#include "fordyca/nsalias.hpp"
#include "fordyca/params/caches/caches_params.hpp"
#include "rcppsw/params/xml_param_parser.hpp"
#include "fordyca/params/caches/static_cache_parser.hpp"
#include "fordyca/params/caches/dynamic_cache_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, params, caches);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class caches_parser
 * @ingroup fordyca params caches
 *
 * @brief Parses XML parameters for relating to cache into \ref caches_params.
 */
class caches_parser: public rcppsw::params::xml_param_parser {
 public:
  explicit caches_parser(uint level)
      : xml_param_parser(level),
        m_static(level + 1),
        m_dynamic(level + 1) {}

  /**
   * @brief The root tag that all static cache parameters should lie under in
   * the XML tree.
   */
  static constexpr char kXMLRoot[] = "caches";

  void parse(const ticpp::Element& node) override;
  bool validate(void) const override;

  std::string xml_root(void) const override { return kXMLRoot; }
  bool parsed(void) const override { return m_parsed; }

  std::shared_ptr<caches_params> parse_results(void) const {
    return m_params;
  }

 private:
  std::shared_ptr<rcppsw::params::base_params> parse_results_impl(void) const override {
    return m_params;
  }

  /* clang-format off */
  bool                           m_parsed{false};
  std::shared_ptr<caches_params> m_params{nullptr};
  static_cache_parser            m_static;
  dynamic_cache_parser           m_dynamic;
  /* clang-format on */
};

NS_END(caches, params, fordyca);

#endif /* INCLUDE_FORDYCA_PARAMS_CACHES_CACHES_PARSER_HPP_ */
