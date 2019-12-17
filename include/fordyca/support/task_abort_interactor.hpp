/**
 * \file task_abort_interactor.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_TASK_ABORT_INTERACTOR_HPP_
#define INCLUDE_FORDYCA_SUPPORT_TASK_ABORT_INTERACTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <list>

#include <argos3/core/simulator/entity/floor_entity.h>

#include "rcppsw/ta/logical_task.hpp"
#include "rcppsw/ta/polled_task.hpp"

#include "fordyca/ds/arena_map.hpp"
#include "fordyca/support/tv/env_dynamics.hpp"
#include "fordyca/tasks/task_status.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);

/*******************************************************************************
 * Classes
 ******************************************************************************/
/**
 * \class task_abort_interactor
 * \ingroup fordyca support
 *
 * \brief Handles a robot's (possible) aborting of its current task on a given
 * timestep.
 */
template <typename T>
class task_abort_interactor : public rer::client<task_abort_interactor<T>> {
 public:
  task_abort_interactor(ds::arena_map* const map,
                        tv::env_dynamics* envd,
                        argos::CFloorEntity* const floor)
      : ER_CLIENT_INIT("fordyca.support.task_abort_interactor"),
        m_map(map),
        m_envd(envd),
        m_floor(floor) {}

  /**
   * \brief Interactors should generally NOT be copy constructable/assignable,
   * but is needed to use these classes with boost::variant.
   *
   * \todo Supposedly in recent versions of boost you can use variants with
   * move-constructible-only types (which is what this class SHOULD be), but I
   * cannot get this to work (the default move constructor needs to be noexcept
   * I think, and is not being interpreted as such).
   */
  task_abort_interactor(const task_abort_interactor& other) = default;
  task_abort_interactor& operator=(const task_abort_interactor&) = delete;

  /**
   * \brief Handle cases in which a robot aborts its current task, and perform
   * any necessary cleanup, such as dropping/distributing a carried block, etc.
   *
   * \param controller The robot to handle task abort for.
   * \param penalty_handlers List of all penalty handlers in the arena that the
   *                         robot may be serving a penalty with that may need
   *                         to be updated.
   *
   * \return \c TRUE if the robot aborted is current task, \c FALSE otherwise.
   */
  bool operator()(T& controller) {
    if (nullptr == controller.current_task() ||
        tasks::task_status::ekABORT_PENDING != controller.task_status()) {
      return false;
    }
    RCSW_UNUSED auto polled =
        dynamic_cast<const rta::polled_task*>(controller.current_task());
    /*
     * If a robot aborted its task and was carrying a block, it needs to (1)
     * drop it so that the block is not left dangling and unusable for the rest
     * of the simulation, (2) update its own internal state.
     */
    if (controller.is_carrying_block()) {
      ER_INFO("%s aborted task '%s' while carrying block%d",
              controller.GetId().c_str(),
              polled->name().c_str(),
              controller.block()->id().v());
      task_abort_with_block(controller);
    } else {
      ER_INFO("%s aborted task '%s' (no block)",
              controller.GetId().c_str(),
              polled->name().c_str());
    }

    m_envd->penalties_flush(controller);
    return true;
  }

 private:
  void task_abort_with_block(T& controller) {
    /*
     * The robot owns a unique copy of a block originally from the arena, so we
     * need to look it up rather than implicitly converting its unique_ptr to a
     * shared_ptr and distributing it--this will cause lots of problems later.
     * This needs to be *BEFORE* releasing the robot's owned block.
     *
     * Holding the block mutex here is not necessary.
     */
    auto it = std::find_if(m_map->blocks().begin(),
                           m_map->blocks().end(),
                           [&](const auto& b) {
                             return controller.block()->id() == b->id();
                           });
    ER_ASSERT(m_map->blocks().end() != it,
              "Block%d carried by %s not found in arena map blocks",
              controller.block()->id().v(),
              controller.GetId().c_str());

    events::free_block_drop_visitor drop_op(
        *it,
        rmath::dvec2uvec(controller.position2D(), m_map->grid_resolution().v()),
        m_map->grid_resolution(),
        true);

    bool conflict = utils::free_block_drop_conflict(*m_map,
                                                    it->get(),
                                                    controller.position2D());
    utils::handle_arena_free_block_drop(drop_op, *m_map, conflict);

    drop_op.visit(controller);
    m_floor->SetChanged();
  } /* perform_block_drop() */

  /* clang-format off */
  ds::arena_map* const       m_map;
  tv::env_dynamics* const    m_envd;
  argos::CFloorEntity* const m_floor;
  /* clang-format on */
};

NS_END(support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_TASK_ABORT_INTERACTOR_HPP_ */
