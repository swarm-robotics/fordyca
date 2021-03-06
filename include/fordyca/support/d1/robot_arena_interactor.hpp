/**
 * \file d1/robot_arena_interactor.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_D1_ROBOT_ARENA_INTERACTOR_HPP_
#define INCLUDE_FORDYCA_SUPPORT_D1_ROBOT_ARENA_INTERACTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/support/task_abort_interactor.hpp"
#include "fordyca/support/cached_block_pickup_interactor.hpp"
#include "fordyca/support/existing_cache_block_drop_interactor.hpp"
#include "fordyca/support/free_block_pickup_interactor.hpp"
#include "fordyca/support/nest_block_drop_interactor.hpp"
#include "fordyca/support/base_cache_manager.hpp"
#include "fordyca/support/interactor_status.hpp"
#include "fordyca/support/mpl/free_block_pickup_spec.hpp"
#include "fordyca/support/mpl/nest_block_drop_spec.hpp"
#include "fordyca/support/mpl/task_abort_spec.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);
class base_loop_functions;
NS_START(d1);

/*******************************************************************************
 * Classes
 ******************************************************************************/
/**
 * \class robot_arena_interactor
 * \ingroup support d1
 *
 * \brief Handles a robot's interactions with the environment on each timestep.
 *
 * Including:
 *
 * - Picking up from/dropping a block in a cache.
 * - Subjecting robots using caches to a penalty on cache drop/pickup.
 * - Picking up a free block.
 * - Dropping a carried block in the nest.
 * - Free block drop due to task abort.
 * - Task abort.
 */
template <typename TController, typename TArenaMap>
class robot_arena_interactor final : public rer::client<robot_arena_interactor<TController,
                                                                               TArenaMap>> {
 public:
  using controller_type = TController;
  struct params {
    carena::caching_arena_map* const map;
    d0::d0_metrics_aggregator *const metrics_agg;
    argos::CFloorEntity* const floor;
    tv::env_dynamics* const envd;
    base_cache_manager* cache_manager;
    base_loop_functions* loop;
  };
  explicit robot_arena_interactor(const params& p)
      : ER_CLIENT_INIT("fordyca.support.d1.robot_arena_interactor"),
        m_free_pickup(p.map,
                                 p.floor,
                                 p.envd->penalty_handler(tv::block_op_src::ekFREE_PICKUP)),
        m_nest_drop(p.map, p.metrics_agg, p.floor, p.envd),
        m_task_abort(p.map,
                                p.envd,
                                p.floor),
        m_cached_pickup(p.map,
                                   p.floor,
                                   p.envd,
                                   p.cache_manager, p.
                                   loop),
        m_existing_cache_drop(p.map, p.envd) {}

  robot_arena_interactor(robot_arena_interactor&&) = default;

  /* Not copy-constructible/assignable by default. */
  robot_arena_interactor(const robot_arena_interactor&) = delete;
  robot_arena_interactor& operator=(const robot_arena_interactor&) = delete;

  /**
   * \brief The actual handling function for interactions.
   *
   * \param controller The controller to handle interactions for.
   * \param t The current timestep.
   */
  interactor_status operator()(TController& controller, const rtypes::timestep& t) {
    if (m_task_abort(controller)) {
      /*
       * This needs to be here, rather than in each robot's control step
       * function, in order to avoid triggering erroneous handling of an aborted
       * task in the loop functions when the executive has not aborted the newly
       * allocated task *after* the previous task was aborted. See FORDYCA#532,FORDYCA#587.
       */
      controller.task_status_update(tasks::task_status::ekRUNNING);
      return interactor_status::ekTASK_ABORT;
    }

    auto status = interactor_status::ekNO_EVENT;
    if (controller.is_carrying_block()) {
      status |= m_nest_drop(controller, t);

      /*
       * Dropped a block in a cache does not require oracular updates, so no
       * need to track its status.
       */
      m_existing_cache_drop(controller, t);
    } else { /* The foot-bot has no block item */
      status |= m_free_pickup(controller, t);
      status |= m_cached_pickup(controller, t);
    }
    return status;
  }

 private:
  using free_pickup_spec = mpl::free_block_pickup_spec<controller::d1::typelist>;
  using nest_drop_spec = mpl::nest_block_drop_spec<controller::d1::typelist>;
  using task_abort_spec = mpl::task_abort_spec<controller::d1::typelist>;

  /* clang-format off */
  free_block_pickup_interactor<TController, free_pickup_spec> m_free_pickup;
  nest_block_drop_interactor<TController, nest_drop_spec>     m_nest_drop;
  task_abort_interactor<TController, task_abort_spec>         m_task_abort;
  cached_block_pickup_interactor<TController>                 m_cached_pickup;
  existing_cache_block_drop_interactor<TController>           m_existing_cache_drop;
  /* clang-format on */
};

NS_END(d1, support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_D1_ROBOT_ARENA_INTERACTOR_HPP_ */
