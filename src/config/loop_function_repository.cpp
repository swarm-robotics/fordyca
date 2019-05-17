/**
 * @file loop_function_repository.cpp
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
#include "fordyca/config/loop_function_repository.hpp"
#include "fordyca/config/arena/arena_map_parser.hpp"
#include "fordyca/config/battery_parser.hpp"
#include "fordyca/config/caches/caches_parser.hpp"
#include "fordyca/config/oracle/oracle_manager_parser.hpp"
#include "fordyca/config/output_parser.hpp"
#include "fordyca/config/tv/tv_manager_parser.hpp"
#include "fordyca/config/visualization_parser.hpp"
#include "rcppsw/control/config/xml/waveform_parser.hpp"
#include "rcppsw/swarm/convergence/config/xml/convergence_parser.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, config);
namespace rswc = rswarm::convergence;

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
loop_function_repository::loop_function_repository(void) noexcept {
  parser_register<output_parser, output_config>(
      output_parser::kXMLRoot, rconfig::xml::xml_config_parser::kHeader1);
  parser_register<arena::arena_map_parser, arena::arena_map_config>(
      arena::arena_map_parser::kXMLRoot,
      rconfig::xml::xml_config_parser::kHeader1);
  parser_register<tv::tv_manager_parser, tv::tv_manager_config>(
      tv::tv_manager_parser::kXMLRoot,
      rconfig::xml::xml_config_parser::kHeader1);
  parser_register<visualization_parser, visualization_config>(
      visualization_parser::kXMLRoot, rconfig::xml::xml_config_parser::kHeader1);
  parser_register<battery_parser, battery_config>(
      battery_parser::kXMLRoot, rconfig::xml::xml_config_parser::kHeader1);
  parser_register<oracle::oracle_manager_parser, oracle::oracle_manager_config>(
      oracle::oracle_manager_parser::kXMLRoot,
      rconfig::xml::xml_config_parser::kHeader1);
  parser_register<caches::caches_parser, caches::caches_config>(
      caches::caches_parser::kXMLRoot,
      rconfig::xml::xml_config_parser::kHeader1);
  parser_register<rswc::config::xml::convergence_parser,
                  rswc::config::convergence_config>(
      rswc::config::xml::convergence_parser::kXMLRoot,
      rconfig::xml::xml_config_parser::kHeader1);
}

NS_END(config, fordyca);
