/**
 * @file memory_foraging_loop_functions.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_MEMORY_FORAGING_LOOP_FUNCTIONS_HPP_
#define INCLUDE_FORDYCA_SUPPORT_MEMORY_FORAGING_LOOP_FUNCTIONS_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <vector>
#include "fordyca/support/random_foraging_loop_functions.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);

/*******************************************************************************
 * Classes
 ******************************************************************************/
class memory_foraging_loop_functions : public random_foraging_loop_functions {
 public:
  memory_foraging_loop_functions() {}
  virtual ~memory_foraging_loop_functions(void) {}

  void Init(argos::TConfigurationNode& node) override;
  void PreStep() override;

 protected:
  void pre_step_iter(argos::CFootBotEntity& robot) override;

 private:
  argos::CColor GetFloorColor(const argos::CVector2& plane_pos) override;
  void set_robot_los(argos::CFootBotEntity& robot);
  void set_robot_tick(argos::CFootBotEntity& robot);

  memory_foraging_loop_functions(const memory_foraging_loop_functions& s) = delete;
  memory_foraging_loop_functions& operator=(const memory_foraging_loop_functions& s) = delete;
};

NS_END(support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_MEMORY_FORAGING_LOOP_FUNCTIONS_HPP_ */
