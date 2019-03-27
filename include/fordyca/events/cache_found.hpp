/**
 * @file cache_found.hpp
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

#ifndef INCLUDE_FORDYCA_EVENTS_CACHE_FOUND_HPP_
#define INCLUDE_FORDYCA_EVENTS_CACHE_FOUND_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/events/cell_op.hpp"
#include "rcppsw/er/client.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace repr {
class base_cache;
}

namespace ds {
class dpo_store;
class dpo_semantic_map;
} // namespace ds

namespace controller { namespace depth2 {
class grp_dpo_controller;
class grp_mdpo_controller;
}} // namespace controller::depth2

NS_START(events, detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
struct cache_found_visit_set {
  using inherited = cell_op_visit_set::value;
  using defined =
      visitor::precise_visit_set<controller::depth2::grp_dpo_controller,
                                 controller::depth2::grp_mdpo_controller,
                                 ds::dpo_store,
                                 ds::dpo_semantic_map>;
  using value = boost::mpl::joint_view<inherited::type, defined::type>;
};

/*
 * @class cache_found
 * @ingroup events detail
 *
 * @brief Created whenever a NEW cache (i.e. one that is not currently known to
 * a robot, but possibly one that it has seen before and whose relevance had
 * expired) is discovered by the robot via it appearing in the robot's LOS.
 */
class cache_found : public cell_op, public rcppsw::er::client<cache_found> {
 public:
  explicit cache_found(std::unique_ptr<repr::base_cache> cache);
  explicit cache_found(const std::shared_ptr<repr::base_cache>& cache);
  ~cache_found(void) override = default;

  cache_found(const cache_found& op) = delete;
  cache_found& operator=(const cache_found& op) = delete;

  /* DPO foraging */
  void visit(ds::dpo_store& store);

  /* MDPO foraging */
  void visit(ds::cell2D& cell);
  void visit(ds::dpo_semantic_map& map);
  void visit(fsm::cell2D_fsm& fsm);

  /* depth2 foraging */
  void visit(controller::depth2::grp_dpo_controller& controller);
  void visit(controller::depth2::grp_mdpo_controller& controller);

 private:
  std::shared_ptr<repr::base_cache> m_cache;
};

/**
 * @brief We use the picky visitor in order to force compile errors if a call to
 * a visitor is made that involves a visitee that is not in our visit set
 * (i.e. remove the possibility of implicit upcasting performed by the
 * compiler).
 */
using cache_found_visitor_impl =
    visitor::precise_visitor<detail::cache_found,
                             detail::cache_found_visit_set::value>;

NS_END(detail);

class cache_found_visitor : public detail::cache_found_visitor_impl {
  using detail::cache_found_visitor_impl::cache_found_visitor_impl;
};

NS_END(events, fordyca);

#endif /* INCLUDE_FORDYCA_EVENTS_CACHE_FOUND_HPP_ */
