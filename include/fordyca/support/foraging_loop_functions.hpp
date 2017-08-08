/**
 * @file foraging_loop_functions.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_FORAGING_LOOP_FUNCTIONS_HPP_
#define INCLUDE_FORDYCA_SUPPORT_FORAGING_LOOP_FUNCTIONS_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <vector>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/core/utility/math/range.h>
#include "rcppsw/common/common.hpp"
#include "fordyca/params/repository.hpp"
#include "fordyca/support/block_distributor.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);

/*******************************************************************************
 * Classes
 ******************************************************************************/
class foraging_loop_functions : public argos::CLoopFunctions {
 public:
  foraging_loop_functions();
  virtual ~foraging_loop_functions(void) {}

  virtual void Init(argos::TConfigurationNode& node);
  virtual void Reset();
  virtual void Destroy();
  virtual argos::CColor GetFloorColor(const argos::CVector2& plane_pos);
  virtual void PreStep();

 private:
  foraging_loop_functions(const foraging_loop_functions& s) = delete;
  foraging_loop_functions& operator=(const foraging_loop_functions& s) = delete;

  argos::CRange<argos::Real> m_arena_x;
  argos::CRange<argos::Real> m_arena_y;
  argos::CRange<argos::Real> m_nest_x;
  argos::CRange<argos::Real> m_nest_y;
  argos::CFloorEntity* m_floor;
  std::string m_ofname;
  std::ofstream m_ofile;
  uint m_total_collected_blocks;
  std::shared_ptr<const struct logging_params> m_logging_params;
  std::shared_ptr<const struct block_params> m_block_params;
  params::repository m_param_manager;
  std::unique_ptr<block_distributor> m_distributor;
  std::shared_ptr<std::vector<argos::CVector2>> m_blocks;
};

NS_END(support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_FORAGING_LOOP_FUNCTIONS_HPP_ */
