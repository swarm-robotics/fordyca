/**
 * @file acquire_existing_cache_fsm.hpp
 *
 * @copyright 2018 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_FSM_ACQUIRE_EXISTING_CACHE_FSM_HPP_
#define INCLUDE_FORDYCA_FSM_ACQUIRE_EXISTING_CACHE_FSM_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <random>
#include "fordyca/fsm/acquire_goal_fsm.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace controller {
class cache_sel_matrix;
}
namespace representation {
class cache;
}
namespace ds {
class perceived_arena_map;
}

NS_START(fsm);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class acquire_existing_cache_fsm
 * @ingroup fsm depth1
 *
 * @brief Acquire an existing cache within the arena. Once such a cache has been
 * acquired (always by vectoring), it signals that it has completed its task.
 */
class acquire_existing_cache_fsm
    : public er::client<acquire_existing_cache_fsm>,
      public acquire_goal_fsm {
 public:
  acquire_existing_cache_fsm(const controller::cache_sel_matrix* matrix,
                             bool is_pickup,
                             controller::saa_subsystem* saa,
                             ds::perceived_arena_map* map);
  ~acquire_existing_cache_fsm(void) override = default;

  acquire_existing_cache_fsm(const acquire_existing_cache_fsm& fsm) = delete;
  acquire_existing_cache_fsm& operator=(const acquire_existing_cache_fsm& fsm) =
      delete;

 private:
  /*
   * See \ref acquire_goal_fsm for the purpose of these callbacks.
   */
  acquisition_goal_type acquisition_goal_internal(void) const;
  acquire_goal_fsm::candidate_type existing_cache_select(void);
  bool candidates_exist(void) const;
  bool calc_acquisition_location(rmath::vector2d* loc);

  bool cache_acquired_cb(bool explore_result) const;
  bool cache_exploration_term_cb(void) const;

  // clang-format off
  const bool                                mc_is_pickup;
  const controller::cache_sel_matrix* const mc_matrix;
  const ds::perceived_arena_map*      const mc_map;
  std::default_random_engine                m_rd{};
  // clang-format on
};

NS_END(fsm, fordyca);

#endif /* INCLUDE_FORDYCA_FSM_ACQUIRE_EXISTING_CACHE_FSM_HPP_ */