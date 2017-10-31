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
#include "fordyca/support/cache_creator.hpp"
#include "fordyca/events/cell_empty.hpp"
#include "fordyca/events/free_block_drop.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
cache_creator::cache_creator(std::shared_ptr<rcppsw::common::er_server> server,
                             representation::grid2D<representation::cell2D>& grid,
                             std::vector<representation::block>& blocks,
                             double min_dist, double cache_size, double resolution) :
    er_client(server), m_min_dist(min_dist), m_cache_size(cache_size),
    m_resolution(resolution), m_blocks(blocks), m_grid(grid), m_server(server) {
      insmod("cache_creator",
           rcppsw::common::er_lvl::DIAG,
           rcppsw::common::er_lvl::NOM);
    }


/*******************************************************************************
 * Member Functions
 ******************************************************************************/
std::vector<representation::cache> cache_creator::create_all(void) {
  std::vector<representation::cache> caches;

  ER_NOM("Creating caches: %zu free blocks", m_blocks.size());

  for (size_t i = 0; i < m_blocks.size() - 1; ++i) {
    std::list<representation::block*> starter_blocks;
    for (size_t j = i + 1; j < m_blocks.size(); ++j) {
      if ((m_blocks[i].real_loc() - m_blocks[j].real_loc()).Length() <=
          m_min_dist) {
        if (std::find(starter_blocks.begin(),
                      starter_blocks.end(),
                      &m_blocks[i]) == starter_blocks.end()) {
          ER_DIAG("Add block %zu: (%f, %f)", i,m_blocks[i].real_loc().GetX(),
                  m_blocks[i].real_loc().GetY());
          starter_blocks.push_back(&m_blocks[i]);
        }
        starter_blocks.push_back(&m_blocks[j]);
        ER_DIAG("Add block %zu: (%f, %f)", j,m_blocks[j].real_loc().GetX(),
                m_blocks[j].real_loc().GetY());
      }
    } /* for(j..) */
    if (starter_blocks.size()) {
      caches.push_back(create_single(starter_blocks));
    }
  } /* for(i..) */
  return caches;
} /* create() */

representation::cache cache_creator::create_single(
    std::list<representation::block*> blocks) {

  argos::CVector2 center = calc_center(blocks);

  /*
   * The cells for all blocks that will comprise the cache should be emptied,
   * and all blocks be deposited in a single cell.
   */
  for (auto block : blocks) {
    events::cell_empty op(block->discrete_loc().first,
                          block->discrete_loc().second);
    m_grid.access(op.x(), op.y()).accept(op);
  } /* for(block..) */

  for (auto block : blocks) {
    events::free_block_drop op(m_server, block,
                               static_cast<size_t>(std::ceil(center.GetX()/ m_resolution)),
                               static_cast<size_t>(std::ceil(center.GetY()/ m_resolution)),
                               m_resolution);
    m_grid.access(op.x(), op.y()).accept(op);
  } /* for(block..) */
  ER_NOM("Create cache at (%f, %f) with  %zu blocks",
         center.GetX(), center.GetY(), blocks.size());

  return representation::cache(m_cache_size, center, blocks);
} /* create_single() */

argos::CVector2 cache_creator::calc_center(
    std::list<representation::block*> blocks) {
  double x = 0;
  double y = 0;
  for (auto block : blocks) {
    x += block->real_loc().GetX();
    y += block->real_loc().GetY();
  } /* for(block..) */
  return argos::CVector2(x / blocks.size(), y / blocks.size());
} /* calc_center() */

NS_END(support, fordyca);
