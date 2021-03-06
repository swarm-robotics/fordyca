/**
 * \file d0_qt_user_functions.cpp
 *
 * \copyright 2017 John Harwell, All rights reserved.
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
#include "rcppsw/common/common.hpp"
RCPPSW_WARNING_DISABLE_PUSH()
RCPPSW_WARNING_DISABLE_OVERLOADED_VIRTUAL()
#include "fordyca/support/d0/d0_qt_user_functions.hpp"
RCPPSW_WARNING_DISABLE_POP()

#include <argos3/core/simulator/entity/controllable_entity.h>

#include "cosm/vis/block_carry_visualizer.hpp"
#include "cosm/vis/polygon2D_visualizer.hpp"

#include "fordyca/controller/cognitive/d0/mdpo_controller.hpp"
#include "fordyca/controller/cognitive/mdpo_perception_subsystem.hpp"
#include "fordyca/ds/dpo_semantic_map.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support, d0);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
d0_qt_user_functions::d0_qt_user_functions(void) {
  RegisterUserFunction<d0_qt_user_functions, chal::robot>(
      &d0_qt_user_functions::Draw);
}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void d0_qt_user_functions::Draw(chal::robot& c_entity) {
  const auto* mdpo =
      dynamic_cast<const controller::cognitive::d0::mdpo_controller*>(
          &c_entity.GetControllableEntity().GetController());
  const auto* base = dynamic_cast<const controller::foraging_controller*>(
      &c_entity.GetControllableEntity().GetController());

  if (base->display_id()) {
    DrawText(argos::CVector3(0.0, 0.0, 0.5), c_entity.GetId());
  }

  if (base->is_carrying_block()) {
    cvis::block_carry_visualizer(this, kBLOCK_VIS_OFFSET, kTEXT_VIS_OFFSET)
        .draw(base->block(), base->GetId().size());
  }
  if (nullptr != mdpo && mdpo->display_los()) {
    const auto* los = mdpo->perception()->los();
    auto res = mdpo->mdpo_perception()->map()->resolution();
    std::vector<rmath::vector2d> points = {
      rmath::zvec2dvec(los->abs_ll(), res.v()) - mdpo->rpos2D(),
      rmath::zvec2dvec(los->abs_ul(), res.v()) - mdpo->rpos2D(),
      rmath::zvec2dvec(los->abs_ur(), res.v()) - mdpo->rpos2D(),
      rmath::zvec2dvec(los->abs_lr(), res.v()) - mdpo->rpos2D()
    };
    cvis::polygon2D_visualizer(this).relative_draw(
        rmath::vector3d(base->rpos2D()), points, rutils::color::kBLUE);
  }
}

using namespace argos; // NOLINT

RCPPSW_WARNING_DISABLE_PUSH()
RCPPSW_WARNING_DISABLE_MISSING_VAR_DECL()
RCPPSW_WARNING_DISABLE_MISSING_PROTOTYPE()
RCPPSW_WARNING_DISABLE_GLOBAL_CTOR()
REGISTER_QTOPENGL_USER_FUNCTIONS(d0_qt_user_functions,
                                 "d0_qt_user_functions"); // NOLINT
RCPPSW_WARNING_DISABLE_POP()

NS_END(d0, support, fordyca);
