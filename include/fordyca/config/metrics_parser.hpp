/**
 * @file metrics_parser.hpp
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

#ifndef INCLUDE_FORDYCA_CONFIG_METRICS_PARSER_HPP_
#define INCLUDE_FORDYCA_CONFIG_METRICS_PARSER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <list>
#include <memory>
#include <string>

#include "fordyca/config/metrics_config.hpp"
#include "fordyca/nsalias.hpp"
#include "rcppsw/config/xml/xml_config_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, config);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class metrics_parser
 * @ingroup fordyca config
 *
 * @brief Parses XML parameters related to metric collection into
 * \ref metrics_config.
 */
class metrics_parser final : public rconfig::xml::xml_config_parser {
 public:
  using config_type = metrics_config;

  /**
   * @brief The XML attributes specifying supported metric collectors.
   */
  static const std::list<std::string> xml_attr;

  ~metrics_parser(void) override = default;

  /**
   * @brief The root tag that all loop functions relating to metrics parameters
   * should lie under in the XML tree.
   */
  static constexpr char kXMLRoot[] = "metrics";

  bool validate(void) const override;
  void parse(const ticpp::Element& node) override;

  std::string xml_root(void) const override { return kXMLRoot; }

 private:
  const rconfig::base_config* config_get_impl(void) const override {
    return m_config.get();
  }

  /* clang-format off */
  std::unique_ptr<config_type> m_config{nullptr};
  /* clang-format on */
};

NS_END(config, fordyca);

#endif /* INCLUDE_FORDYCA_CONFIG_METRICS_PARSER_HPP_ */
