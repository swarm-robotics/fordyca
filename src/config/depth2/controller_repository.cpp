/**
 * \file controller_repository.cpp
 *
 * \copyright 2018 John Harwell, All rights reserved.
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
#include "fordyca/config/depth2/controller_repository.hpp"

#include "rcppsw/ta/config/xml/task_alloc_parser.hpp"

#include "fordyca/fordyca.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, config, depth2);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
controller_repository::controller_repository(void) {
  parser_find<rta::config::xml::task_alloc_parser>(
      rta::config::xml::task_alloc_parser::kXMLRoot)
      ->exec_est_task_add("cache_starter");
  parser_find<rta::config::xml::task_alloc_parser>(
      rta::config::xml::task_alloc_parser::kXMLRoot)
      ->exec_est_task_add("cache_finisher");
  parser_find<rta::config::xml::task_alloc_parser>(
      rta::config::xml::task_alloc_parser::kXMLRoot)
      ->exec_est_task_add("cache_transferer");
  parser_find<rta::config::xml::task_alloc_parser>(
      rta::config::xml::task_alloc_parser::kXMLRoot)
      ->exec_est_task_add("cache_collector");
}

NS_END(depth2, config, fordyca);
