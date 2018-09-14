/**
 * @file cache_creator.cpp
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
#include "fordyca/support/depth1/cache_creator.hpp"
#include "fordyca/events/cell_cache_extent.hpp"
#include "fordyca/events/cell_empty.hpp"
#include "fordyca/events/free_block_drop.hpp"
#include "fordyca/representation/arena_cache.hpp"
#include "fordyca/representation/cell2D.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support, depth1);
using representation::arena_grid;

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
cache_creator::cache_creator(representation::arena_grid& grid,
                             double cache_size,
                             double resolution)
    : ER_CLIENT_INIT("fordyca.support.depth1.cache_creator"),
      m_cache_size(cache_size),
      m_resolution(resolution),
      m_grid(grid) {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
std::unique_ptr<representation::arena_cache> cache_creator::create_single(
    block_list blocks,
    const argos::CVector2& center) {
  /*
   * The cell that will be the location of the new cache may already contain a
   * block. If so, it should be added to the list of blocks for the cache.
   */
  rcppsw::math::dcoord2 d = math::rcoord_to_dcoord(center, m_resolution);
  representation::cell2D& cell = m_grid.access<arena_grid::kCell>(d);
  if (cell.state_has_block()) {
    ER_ASSERT(cell.block(), "Cell does not have block");

    /*
     * We use insert() instead of push_back() here so that it there was a
     * leftover block on the cell where a cache used to be that is also where
     * this cache is being created, it becomes the "front" of the cache, and
     * will be the first block picked up by a robot from the new cache. This
     * helps to ensure fairness/better statistics for the simulations.
     */
    blocks.insert(blocks.begin(), cell.block());
  }

  /*
   * The cells for all blocks that will comprise the cache should be set to
   * cache extent,
   * and all blocks be deposited in a single cell.
   */
  for (auto block : blocks) {
    events::cell_empty op(block->discrete_loc());
    m_grid.access<arena_grid::kCell>(op.x(), op.y()).accept(op);
  } /* for(block..) */

  for (auto block : blocks) {
    events::free_block_drop op(block, d, m_resolution);
    m_grid.access<arena_grid::kCell>(op.x(), op.y()).accept(op);
  } /* for(block..) */
  ER_INFO("Create cache at (%f, %f) -> (%u, %u) with  %zu blocks",
          center.GetX(),
          center.GetY(),
          d.first,
          d.second,
          blocks.size());

  block_vector block_vec(blocks.begin(), blocks.end());
  return rcppsw::make_unique<representation::arena_cache>(
      m_cache_size, m_grid.resolution(), center, block_vec, -1);
} /* create_single() */

void cache_creator::update_host_cells(cache_vector& caches) {
  /*
   * To reset all cells covered by a cache's extent, we simply send them a
   * CACHE_EXTENT event. EXCEPT for the cell that hosted the actual cache,
   * because it is currently in the HAS_CACHE state as part of the cache
   * creation process and setting it here will trigger an assert later.
   */
  for (auto& cache : caches) {
    m_grid.access<arena_grid::kCell>(cache->discrete_loc()).entity(cache);

    auto xspan = cache->xspan(cache->real_loc());
    auto yspan = cache->yspan(cache->real_loc());
    size_t xmin = std::ceil(xspan.get_min() / m_grid.resolution());
    size_t xmax = std::ceil(xspan.get_max() / m_grid.resolution());
    size_t ymin = std::ceil(yspan.get_min() / m_grid.resolution());
    size_t ymax = std::ceil(yspan.get_max() / m_grid.resolution());

    for (size_t i = xmin; i < xmax; ++i) {
      for (size_t j = ymin; j < ymax; ++j) {
        rcppsw::math::dcoord2 c = rcppsw::math::dcoord2(i, j);
        if (c != cache->discrete_loc()) {
          ER_ASSERT(
              cache->contains_point(
                  math::dcoord_to_rcoord(c, m_grid.resolution())),
              "Cache%d does not contain point (%zu, %zu) within its extent",
              cache->id(),
              i,
              j);
          auto& cell = m_grid.access<arena_grid::kCell>(i, j);
          ER_ASSERT(!cell.state_in_cache_extent(),
                    "cell(%zu, %zu) already in CACHE_EXTENT",
                    i,
                    j);
          events::cell_cache_extent e(c, cache);
          cell.accept(e);
        }
      } /* for(j..) */
    }   /* for(i..) */
  }     /* for(cache..) */
} /* update_host_cells() */

NS_END(depth1, support, fordyca);
