/**
 * @file block_sel_matrix_parser.hpp
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

#ifndef INCLUDE_FORDYCA_PARAMS_BLOCK_SEL_BLOCK_SEL_MATRIX_PARSER_HPP_
#define INCLUDE_FORDYCA_PARAMS_BLOCK_SEL_BLOCK_SEL_MATRIX_PARSER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>

#include "fordyca/params/block_sel/block_priorities_parser.hpp"
#include "fordyca/params/block_sel/block_sel_matrix_params.hpp"
#include "rcppsw/params/xml_param_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, params, block_sel);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class block_sel_matrix_parser
 * @ingroup fordyca params block_sel
 *
 * @brief Parses XML parameters for the \ref block_sel_matrix at the start
 * of simulation.
 */
class block_sel_matrix_parser : public rparams::xml_param_parser {
 public:
  explicit block_sel_matrix_parser(uint level)
      : xml_param_parser(level), m_priorities(level + 1) {}

  /**
   * @brief The root tag that all block sel matrix parameters should lie
   * under in the XML tree.
   */
  static constexpr char kXMLRoot[] = "block_sel_matrix";

  void parse(const ticpp::Element& node) override;

  std::string xml_root(void) const override { return kXMLRoot; }
  std::shared_ptr<block_sel_matrix_params> parse_results(void) const {
    return m_params;
  }

 private:
  std::shared_ptr<rparams::base_params> parse_results_impl(
      void) const override {
    return m_params;
  }

  /* clang-format off */
  std::shared_ptr<block_sel_matrix_params> m_params{nullptr};
  block_priorities_parser                  m_priorities;
  /* clang-format on */
};

NS_END(block_sel, params, fordyca);

#endif /* INCLUDE_FORDYCA_PARAMS_BLOCK_SEL_BLOCK_SEL_MATRIX_PARSER_HPP_ */
