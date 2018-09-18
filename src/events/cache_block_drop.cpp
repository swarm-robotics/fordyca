/**
 * @file cache_block_drop.cpp
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
#include "fordyca/events/cache_block_drop.hpp"
#include "fordyca/controller/base_perception_subsystem.hpp"
#include "fordyca/controller/depth1/foraging_controller.hpp"
#include "fordyca/ds/arena_map.hpp"
#include "fordyca/ds/cell2D.hpp"
#include "fordyca/ds/perceived_arena_map.hpp"
#include "fordyca/events/free_block_drop.hpp"
#include "fordyca/fsm/depth1/block_to_goal_fsm.hpp"
#include "fordyca/representation/arena_cache.hpp"
#include "fordyca/representation/base_block.hpp"
#include "fordyca/tasks/depth1/foraging_task.hpp"
#include "fordyca/tasks/depth1/harvester.hpp"
#include "fordyca/tasks/depth2/cache_transferer.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, events);
using ds::arena_grid;
using ds::occupancy_grid;

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
cache_block_drop::cache_block_drop(
    const std::shared_ptr<representation::base_block>& block,
    const std::shared_ptr<representation::arena_cache>& cache,
    double resolution)
    : cell_op(cache->discrete_loc().first, cache->discrete_loc().second),
      ER_CLIENT_INIT("fordyca.events.cache_block_drop"),
      m_resolution(resolution),
      m_block(block),
      m_cache(cache) {}

/*******************************************************************************
 * Depth1 Foraging
 ******************************************************************************/
void cache_block_drop::visit(ds::cell2D& cell) {
  ER_ASSERT(0 != cell.loc().first && 0 != cell.loc().second,
            "Cell does not have coordinates");

  cell.fsm().accept(*this);
  ER_ASSERT(m_cache->n_blocks() == cell.block_count(),
            "Cache/cell disagree on # of blocks: cache=%u/cell=%zu",
            m_cache->n_blocks(),
            cell.block_count());
} /* visit() */

void cache_block_drop::visit(fsm::cell2D_fsm& fsm) {
  ER_ASSERT(fsm.state_has_cache(), "cell does not contain a cache");
  fsm.event_block_drop();
} /* visit() */

void cache_block_drop::visit(ds::arena_map& map) {
  ER_ASSERT(-1 != m_block->robot_id(), "undefined robot index");
  __rcsw_unused int index = m_block->robot_id();
  m_block->accept(*this);
  m_cache->accept(*this);
  map.access<arena_grid::kCell>(cell_op::x(), cell_op::y()).accept(*this);
  ER_INFO("arena_map: fb%d dropped block%d in cache%d [%u blocks total]",
          index,
          m_block->id(),
          m_cache->id(),
          m_cache->n_blocks());
} /* visit() */

void cache_block_drop::visit(ds::perceived_arena_map& map) {
  map.access<occupancy_grid::kCell>(x(), y()).accept(*this);
} /* visit() */

void cache_block_drop::visit(representation::base_block& block) {
  events::free_block_drop e(m_block, /* OK because we only have 1 block */
                            rcppsw::math::dcoord2(cell_op::x(), cell_op::y()),
                            m_resolution);
  block.accept(e);
} /* visit() */

void cache_block_drop::visit(representation::arena_cache& cache) {
  cache.block_add(m_block);
  cache.has_block_drop();
} /* visit() */

void cache_block_drop::visit(controller::depth1::foraging_controller& controller) {
  controller.ndc_push();
  controller.block(nullptr);
  controller.perception()->map()->accept(*this);
  dynamic_cast<tasks::depth1::existing_cache_interactor*>(
      controller.current_task())
      ->accept(*this);

  ER_INFO("Dropped block%d in cache%d", m_block->id(), m_cache->id());
  controller.ndc_pop();
} /* visit() */

void cache_block_drop::visit(fsm::depth1::block_to_goal_fsm& fsm) {
  fsm.inject_event(controller::foraging_signal::BLOCK_DROP,
                   state_machine::event_type::NORMAL);
} /* visit() */

void cache_block_drop::visit(tasks::depth1::harvester& task) {
  static_cast<fsm::depth1::block_to_goal_fsm*>(task.mechanism())->accept(*this);
} /* visit() */

/*******************************************************************************
 * Depth2 Foraging
 ******************************************************************************/
void cache_block_drop::visit(controller::depth2::foraging_controller& controller) {
  ER_ASSERT(false, "Not implemented");
} /* visit() */

void cache_block_drop::visit(tasks::depth2::cache_transferer& task) {
  static_cast<fsm::depth1::block_to_goal_fsm*>(task.mechanism())->accept(*this);
} /* visit() */

NS_END(events, fordyca);
