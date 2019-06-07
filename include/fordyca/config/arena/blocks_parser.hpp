/**
 * @file blocks_parser.hpp
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

#ifndef INCLUDE_FORDYCA_CONFIG_ARENA_BLOCKS_PARSER_HPP_
#define INCLUDE_FORDYCA_CONFIG_ARENA_BLOCKS_PARSER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <memory>
#include <string>

#include "fordyca/config/arena/blocks_config.hpp"
#include "fordyca/nsalias.hpp"
#include "rcppsw/config/xml/xml_config_parser.hpp"
#include "rcppsw/control/config/xml/waveform_parser.hpp"
#include "fordyca/config/arena/block_dist_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, config, arena);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class blocks_parser
 * @ingroup fordyca config arena
 *
 * @brief Parses XML parameters related to blocks in the arena into \ref
 * blocks_config.
 */
class blocks_parser : public rconfig::xml::xml_config_parser {
 public:
  explicit blocks_parser(uint level)
      : xml_config_parser(level),
        m_dist(level + 1) {}

  /**
   * @brief The root tag that all block parameters should lie under in the
   * XML tree.
   */
  static constexpr char kXMLRoot[] = "blocks";

  void parse(const ticpp::Element& node) override;
  bool validate(void) const override;

  std::string xml_root(void) const override { return kXMLRoot; }
  std::shared_ptr<blocks_config> config_get(void) const { return m_config; }

 private:
  std::shared_ptr<rconfig::base_config> config_get_impl(
      void) const override {
    return m_config;
  }

  /* clang-format off */
  block_dist_parser              m_dist;
  std::shared_ptr<blocks_config> m_config{nullptr};
  /* clang-format on */
};

NS_END(arena, config, fordyca);

#endif /* INCLUDE_FORDYCA_CONFIG_ARENA_BLOCKS_PARSER_HPP_ */
