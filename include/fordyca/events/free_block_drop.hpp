/**
 * @file free_block_drop.hpp
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

#ifndef INCLUDE_FORDYCA_EVENTS_FREE_BLOCK_DROP_HPP_
#define INCLUDE_FORDYCA_EVENTS_FREE_BLOCK_DROP_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/events/block_drop_base_visit_set.hpp"
#include "fordyca/events/cell_op.hpp"
#include "rcppsw/er/client.hpp"
#include "rcppsw/math/vector2.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace rvisitor = rcppsw::patterns::visitor;
namespace rmath = rcppsw::math;
namespace controller {
class block_sel_matrix;
}
namespace ds {
class dpo_semantic_map;
}
namespace fsm {
class block_to_goal_fsm;
} // namespace fsm
namespace controller {
namespace depth1 {
class gp_dpo_controller;
class gp_mdpo_controller;
} // namespace depth1
namespace depth2 {
class grp_dpo_controller;
class grp_mdpo_controller;
} // namespace depth2
} // namespace controller
namespace tasks {
class base_foraging_task;
namespace depth2 {
class cache_starter;
class cache_finisher;
} // namespace depth2
} // namespace tasks

NS_START(events, detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
struct free_block_drop_visit_set {
  using inherited = boost::mpl::joint_view<block_drop_base_visit_set::value,
                                           cell_op_visit_set::value>;
  using defined = boost::mpl::vector<
      /* depth0 */
      fsm::block_to_goal_fsm,
      ds::dpo_semantic_map,
      /* depth1 */
      controller::depth1::gp_dpo_controller,
      controller::depth1::gp_mdpo_controller,
      /* depth2 */
      controller::depth2::grp_dpo_controller,
      controller::depth2::grp_mdpo_controller,
      tasks::depth2::cache_starter,
      tasks::depth2::cache_finisher,
      fsm::block_to_goal_fsm,
      ds::dpo_semantic_map>;
  using value = boost::mpl::joint_view<inherited, defined>;
};

/**
 * @class free_block_drop
 * @ingroup events detail
 *
 * @brief Created whenever a block is dropped somewhere in the arena that is not
 * a cache or the nest.
 *
 * This can happen when:
 *
 * - The loop functions are doing block distribution.
 * - A robot aborts its task, and is carrying a block.
 */
class free_block_drop : public rcppsw::er::client<free_block_drop>,
                        public cell_op {
 public:
  /**
   * @param block The block to drop.
   * @param coord The discrete coordinates of the cell to drop the block in.
   * @param resolution The resolution of the arena map.
   */
  free_block_drop(const std::shared_ptr<repr::base_block>& block,
                  const rmath::vector2u& coord,
                  double resolution);
  ~free_block_drop(void) override = default;

  free_block_drop(const free_block_drop& op) = delete;
  free_block_drop& operator=(const free_block_drop& op) = delete;

  /* depth0 */
  void visit(ds::cell2D& cell);
  void visit(repr::base_block& block);
  void visit(fsm::cell2D_fsm& fsm);
  void visit(ds::arena_map& map);

  /* depth1 */
  void visit(controller::depth1::gp_dpo_controller&);
  void visit(controller::depth1::gp_mdpo_controller&);

  /* depth2 */
  void visit(controller::depth2::grp_dpo_controller&);
  void visit(controller::depth2::grp_mdpo_controller&);
  void visit(tasks::depth2::cache_starter&);
  void visit(tasks::depth2::cache_finisher&);
  void visit(fsm::block_to_goal_fsm&);
  void visit(ds::dpo_semantic_map& map);

  /**
   * @brief Get the handle on the block that has been dropped.
   */
  std::shared_ptr<repr::base_block> block(void) const { return m_block; }

 private:
  bool dispatch_free_block_interactor(tasks::base_foraging_task* task,
                                      controller::block_sel_matrix* bsel_matrix);

  /* clang-format off */
  double                            m_resolution;
  std::shared_ptr<repr::base_block> m_block;
  /* clang-format on */
};

/**
 * @brief We use the picky visitor in order to force compile errors if a call to
 * a visitor is made that involves a visitee that is not in our visit set
 * (i.e. remove the possibility of implicit upcasting performed by the
 * compiler).
 */
using free_block_drop_visitor_impl =
    rvisitor::precise_visitor<detail::free_block_drop,
                             detail::free_block_drop_visit_set::value>;

NS_END(detail);

class free_block_drop_visitor : public detail::free_block_drop_visitor_impl {
  using detail::free_block_drop_visitor_impl::free_block_drop_visitor_impl;
};

NS_END(events, fordyca);

#endif /* INCLUDE_FORDYCA_EVENTS_FREE_BLOCK_DROP_HPP_ */
