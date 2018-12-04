/**
 * @file arena_map.hpp
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

#ifndef INCLUDE_FORDYCA_DS_ARENA_MAP_HPP_
#define INCLUDE_FORDYCA_DS_ARENA_MAP_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <vector>

#include "fordyca/ds/arena_grid.hpp"
#include "fordyca/ds/block_vector.hpp"
#include "fordyca/ds/cache_vector.hpp"
#include "fordyca/metrics/robot_occupancy_metrics.hpp"
#include "fordyca/representation/arena_cache.hpp"
#include "fordyca/representation/base_block.hpp"
#include "fordyca/representation/nest.hpp"
#include "fordyca/support/block_dist/dispatcher.hpp"

#include "rcppsw/er/client.hpp"
#include "rcppsw/patterns/decorator/decorator.hpp"
#include "rcppsw/patterns/visitor/visitable.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);
namespace params { namespace arena {
struct arena_map_params;
}} // namespace params::arena

namespace representation {
class arena_cache;
}
namespace support {
class base_loop_functions;
}
NS_START(ds);

class cell2D;
namespace visitor = rcppsw::patterns::visitor;
namespace decorator = rcppsw::patterns::decorator;
namespace er = rcppsw::er;
namespace rmath = rcppsw::math;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class arena_map
 * @ingroup ds
 *
 * @brief Combines a 2D grid with sets of objects (blocks, caches, nests, etc.)
 * that populate the grid and move around as the state of the arena
 * changes. The idea is that the arena map should be as simple as possible,
 * providing accessors and mutators, but not more complex logic, separating the
 * data in manages from the algorithms that operate on that data.
 */
class arena_map : public er::client<arena_map>,
                  public metrics::robot_occupancy_metrics,
                  public visitor::visitable_any<arena_map>,
                  public decorator::decorator<arena_grid> {
 public:
  explicit arena_map(const struct params::arena::arena_map_params* params);

  /* robot occupancy metrics */
  bool has_robot(uint i, uint j) const override;

  /**
   * @brief Reset the # of caches that have been removed on a single timestep
   * from the arena due to depletion.
   */
  void caches_removed_reset(void) { m_caches_removed = 0; }

  void caches_removed(uint b) { m_caches_removed += b; }
  uint caches_removed(void) const { return m_caches_removed; }

  /**
   * @brief Get the list of all the blocks currently present in the arena.
   *
   * Some blocks may not be visible on the arena_map, as they are being carried
   * by robots.
   */
  block_vector& blocks(void) { return m_blocks; }
  const block_vector& blocks(void) const { return m_blocks; }

  /**
   * @brief Get the # of blocks available in the arena.
   */
  size_t n_blocks(void) const { return m_blocks.size(); }

  /**
   * @brief Get the list of all the caches currently present in the arena and
   * active.
   */
  cache_vector& caches(void) { return m_caches; }
  const cache_vector& caches(void) const { return m_caches; }

  /**
   * @brief Get the # of caches currently in the arena.
   */
  size_t n_caches(void) const { return m_caches.size(); }

  /**
   * @brief Add caches that have been created by robots in the arena to the
   * current set of active caches.
   */
  void caches_add(const cache_vector& caches) {
    m_caches.insert(m_caches.end(), caches.begin(), caches.end());
    ER_INFO("Add %zu created caches, total=%zu", caches.size(), m_caches.size());
  }

  /**
   * @brief Remove a cache from the list of caches.
   *
   * @param victim The cache to remove.
   */
  void cache_remove(const std::shared_ptr<representation::arena_cache>& victim);

  /**
   * @brief Clear the cells that a cache covers while in the arena that are in
   * CACHE_EXTENT state, resetting them to EMPTY. Called right before deleting
   * the cache from the arena.
   *
   * @param victim The cache about to be deleted.
   */
  void cache_extent_clear(
      const std::shared_ptr<representation::arena_cache>& victim);

  template <int Index>
  typename arena_grid::layer_value_type<Index>::value_type& access(
      const rmath::vector2u& d) {
    return decoratee().access<Index>(d);
  }
  template <int Index>
  const typename arena_grid::layer_value_type<Index>::value_type& access(
      const rmath::vector2u& d) const {
    return decoratee().access<Index>(d);
  }
  template <int Index>
  typename arena_grid::layer_value_type<Index>::value_type& access(size_t i,
                                                                   size_t j) {
    return decoratee().access<Index>(i, j);
  }
  template <int Index>
  const typename arena_grid::layer_value_type<Index>::value_type& access(
      size_t i,
      size_t j) const {
    return decoratee().access<Index>(i, j);
  }

  /**
   * @brief Distribute all blocks in the arena. Resets arena state. Should only
   * be called during (re)-initialization.
   */
  void distribute_all_blocks(void);

  /**
   * @brief Distribute a particular block in the arena, according to whatever
   * policy was specified in the .argos file.
   *
   * @param block The block to distribute.
   *
   * @return \c TRUE iff distribution was successful, \c FALSE otherwise.
   */
  bool distribute_single_block(
      std::shared_ptr<representation::base_block>& block);

  DECORATE_FUNC(xdsize, const);
  DECORATE_FUNC(ydsize, const);
  DECORATE_FUNC(xrsize, const);
  DECORATE_FUNC(yrsize, const);

  /**
   * @brief Determine if a robot is currently on top of a block (i.e. if the
   * center of the robot has crossed over into the space occupied by the block
   * extent).
   *
   * While the robots also have their own means of checking if they are on a
   * block or not, there are some false positives, so this function is used as
   * the final arbiter when deciding whether or not to trigger a given event
   * (such as \ref free_block_pickup) for a particular robot.
   *
   * @param pos The position of a robot.
   *
   * @return The ID of the block that the robot is on, or -1 if the robot is not
   * actually on a block.
   */
  int robot_on_block(const rmath::vector2d& pos) const;

  /**
   * @brief Determine if a robot is currently on top of a cache (i.e. if the
   * center of the robot has crossed over into the space occupied by the block
   * extent).
   *
   * While the robots also have their own means of checking if they are on a
   * cache or not, there are some false positives, so this function is used as
   * the final arbiter when deciding whether or not to trigger a cache related
   * event for a particular robot (such as \ref cached_block_pickup).
   *
   * @param pos The position of a robot.
   *
   * @return The ID of the cache that the robot is on, or -1 if the robot is not
   * actually on a cache.
   */
  int robot_on_cache(const rmath::vector2d& pos) const;

  /**
   * @brief Get the subgrid for use in calculating a robot's LOS.
   *
   * @param x X coord of the center of the subgrid.
   * @param y Y coord of the center of the subgrid.
   * @param radius The radius of the subgrid.
   *
   * @return The subgrid.
   */
  rcppsw::ds::grid_view<cell2D> subgrid(size_t x, size_t y, size_t radius) {
    return decoratee().layer<arena_grid::kCell>()->subcircle(x, y, radius);
  }
  double grid_resolution(void) const { return decoratee().resolution(); }
  const representation::nest& nest(void) const { return m_nest; }
  const support::block_dist::base_distributor* block_distributor(void) const {
    return m_block_dispatcher.distributor();
  }

  /**
   * @brief Perform deferred initialization. This is not part the constructor so
   * that it can be verified via return code. Currently it initializes:
   *
   * - The block distributor
   * - Nest lights
   */
  bool initialize(support::base_loop_functions* loop);

 private:
  // clang-format off
  uint                            m_caches_removed{0};
  block_vector                    m_blocks;
  cache_vector                    m_caches;
  representation::nest            m_nest;
  support::block_dist::dispatcher m_block_dispatcher;
  // clang-format on
};

NS_END(ds, fordyca);

#endif /* INCLUDE_FORDYCA_DS_ARENA_MAP_HPP_ */
