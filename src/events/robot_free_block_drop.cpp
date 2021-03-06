/**
 * \file robot_free_block_drop.cpp
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
#include "fordyca/events/robot_free_block_drop.hpp"

#include "cosm/ds/cell2D.hpp"
#include "cosm/repr/base_block3D.hpp"

#include "fordyca/controller/cognitive/block_sel_matrix.hpp"
#include "fordyca/controller/cognitive/d1/bitd_dpo_controller.hpp"
#include "fordyca/controller/cognitive/d1/bitd_mdpo_controller.hpp"
#include "fordyca/controller/cognitive/d1/bitd_odpo_controller.hpp"
#include "fordyca/controller/cognitive/d1/bitd_omdpo_controller.hpp"
#include "fordyca/controller/cognitive/d2/birtd_dpo_controller.hpp"
#include "fordyca/controller/cognitive/d2/birtd_mdpo_controller.hpp"
#include "fordyca/controller/cognitive/d2/birtd_odpo_controller.hpp"
#include "fordyca/controller/cognitive/d2/birtd_omdpo_controller.hpp"
#include "fordyca/ds/dpo_semantic_map.hpp"
#include "fordyca/fsm/block_to_goal_fsm.hpp"
#include "fordyca/fsm/foraging_signal.hpp"
#include "fordyca/tasks/d1/foraging_task.hpp"
#include "fordyca/tasks/d2/cache_finisher.hpp"
#include "fordyca/tasks/d2/cache_starter.hpp"
#include "fordyca/tasks/d2/foraging_task.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, events, detail);
using ds::occupancy_grid;

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
robot_free_block_drop::robot_free_block_drop(
    std::unique_ptr<crepr::base_block3D> block,
    const rmath::vector2z& coord,
    const rtypes::discretize_ratio& resolution)
    : ER_CLIENT_INIT("fordyca.events.robot_free_block_drop"),
      cell2D_op(coord),
      mc_resolution(resolution),
      m_block(std::move(block)) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
bool robot_free_block_drop::dispatch_free_block_interactor(
    tasks::base_foraging_task* const task,
    controller::cognitive::block_sel_matrix* const bsel_matrix) {
  auto* polled = dynamic_cast<cta::polled_task*>(task);
  auto* interactor = dynamic_cast<events::free_block_interactor*>(task);
  bool ret = false;

  if (nullptr != interactor) {
    /*
     * If this is true then we know the current task must be a cache starter or
     * a cache finisher, so we need to make a note of the block we are dropping
     * so we don't just turn around and pick it up when we start our next task.
     *
     * If we are performing a free block drop because we have just aborted our
     * task, then obviously no need to do that.
     */
    if (tasks::d2::foraging_task::task_in_d2(polled) && !polled->task_aborted()) {
      ER_INFO("Added block%d@%s/%s to exception list,task='%s'",
              m_block->id().v(),
              rcppsw::to_string(m_block->ranchor2D()).c_str(),
              rcppsw::to_string(m_block->danchor2D()).c_str(),
              polled->name().c_str());
      bsel_matrix->sel_exception_add(m_block->id());
      ret = true;
    }
    interactor->accept(*this);
  }
  return ret;
} /* dispatch_free_block_interactor() */

/*******************************************************************************
 * Depth2
 ******************************************************************************/
void robot_free_block_drop::visit(
    controller::cognitive::d2::birtd_mdpo_controller& controller) {
  controller.ndc_pusht();

  if (dispatch_free_block_interactor(controller.current_task(),
                                     controller.block_sel_matrix())) {
    controller.bsel_exception_added(true);
  }

  controller.ndc_pop();
} /* visit() */

void robot_free_block_drop::visit(
    controller::cognitive::d2::birtd_dpo_controller& controller) {
  controller.ndc_pusht();

  if (dispatch_free_block_interactor(controller.current_task(),
                                     controller.block_sel_matrix())) {
    controller.bsel_exception_added(true);
  }

  controller.ndc_pop();
} /* visit() */

void robot_free_block_drop::visit(
    controller::cognitive::d2::birtd_omdpo_controller& controller) {
  controller.ndc_pusht();

  if (dispatch_free_block_interactor(controller.current_task(),
                                     controller.block_sel_matrix())) {
    controller.bsel_exception_added(true);
  }

  controller.ndc_pop();
} /* visit() */

void robot_free_block_drop::visit(
    controller::cognitive::d2::birtd_odpo_controller& controller) {
  controller.ndc_pusht();

  if (dispatch_free_block_interactor(controller.current_task(),
                                     controller.block_sel_matrix())) {
    controller.bsel_exception_added(true);
  }

  controller.ndc_pop();
} /* visit() */

void robot_free_block_drop::visit(tasks::d2::cache_starter& task) {
  visit(*static_cast<fsm::block_to_goal_fsm*>(task.mechanism()));
} /* visit() */

void robot_free_block_drop::visit(tasks::d2::cache_finisher& task) {
  visit(*static_cast<fsm::block_to_goal_fsm*>(task.mechanism()));
} /* visit() */

void robot_free_block_drop::visit(fsm::block_to_goal_fsm& fsm) {
  fsm.inject_event(fsm::foraging_signal::ekBLOCK_DROP,
                   rpfsm::event_type::ekNORMAL);
} /* visit() */

NS_END(detail, events, fordyca);
