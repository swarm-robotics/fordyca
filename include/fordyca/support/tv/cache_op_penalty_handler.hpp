/**
 * \file cache_op_penalty_handler.hpp
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

#ifndef INCLUDE_FORDYCA_SUPPORT_TV_CACHE_OP_PENALTY_HANDLER_HPP_
#define INCLUDE_FORDYCA_SUPPORT_TV_CACHE_OP_PENALTY_HANDLER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>

#include "fordyca/fsm/block_transporter.hpp"
#include "fordyca/support/tv/cache_op_filter.hpp"
#include "fordyca/support/tv/cache_op_src.hpp"
#include "fordyca/support/tv/temporal_penalty_handler.hpp"
#include "fordyca/support/utils/event_utils.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support, tv);

/*******************************************************************************
 * Classes
 ******************************************************************************/
/**
 * \class cache_op_penalty_handler
 * \ingroup support
 *
 * \brief The handler for block operation penalties for robots (e.g. picking
 * up, dropping in places that do not involve existing caches.
 */
class cache_op_penalty_handler final
    : public temporal_penalty_handler,
      public rer::client<cache_op_penalty_handler> {
 public:
  cache_op_penalty_handler(cfds::arena_map* const map,
                           const rct::config::waveform_config* const config,
                           const std::string& name)
      : temporal_penalty_handler(config, name),
        ER_CLIENT_INIT("fordyca.support.cache_op_penalty_handler"),
        m_map(map) {}

  ~cache_op_penalty_handler(void) override = default;
  cache_op_penalty_handler& operator=(const cache_op_penalty_handler& other) =
      delete;
  cache_op_penalty_handler(const cache_op_penalty_handler&) = delete;

  /**
   * \brief Check if a robot has acquired a block or is in the nest, and is
   * trying to drop/pickup a block. If so, create a \ref temporal_penalty object
   * and associate it with the robot.
   *
   * \tparam TControllerType The type of the controller. Must be a template
   * parameter, rather than \ref controller::foraging_controller, because of the
   * goal acquisition determination done by \ref cache_op_filter.

   * \param controller The robot to check.
   * \param src The penalty source (i.e. what event caused this penalty to be
   *            applied).
   * \param t The current timestep.
  */
  template<typename TControllerType>
  op_filter_status penalty_init(const TControllerType& controller,
                                cache_op_src src,
                                const rtypes::timestep& t) {
    /*
     * If the robot has not acquired a cache, or thinks it has but actually has
     * not, nothing to do.
     */
    auto filter = cache_op_filter<TControllerType>(m_map)(controller, src);
    if (filter != op_filter_status::ekSATISFIED) {
      return filter;
    }

    ER_ASSERT(!is_serving_penalty(controller),
              "%s already serving cache penalty?",
              controller.GetId().c_str());

    rtypes::timestep orig_duration = penalty_calc(t);
    auto id = utils::robot_on_cache(controller, *m_map);
    ER_ASSERT(rtypes::constants::kNoUUID != id,
              "%s not in cache?",
              controller.GetId().c_str());

    RCSW_UNUSED auto duration = penalty_add(&controller, id, orig_duration, t);
    ER_INFO("%s: cache%d start=%u, penalty=%u, adjusted penalty=%u src=%d",
            controller.GetId().c_str(),
            id.v(),
            t.v(),
            orig_duration.v(),
            duration.v(),
            static_cast<int>(src));

    return filter;
  }

 private:
  /* clang-format off */
  cfds::arena_map* const m_map;
  /* clang-format on */
};
NS_END(tv, support, fordyca);

#endif /* INCLUDE_FORDYCA_SUPPORT_TV_CACHE_OP_PENALTY_HANDLER_HPP_ */
