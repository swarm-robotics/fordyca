/**
 * @file block_vanished.hpp
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

#ifndef INCLUDE_FORDYCA_EVENTS_BLOCK_VANISHED_HPP_
#define INCLUDE_FORDYCA_EVENTS_BLOCK_VANISHED_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/events/cell_op.hpp"
#include "rcppsw/er/client.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace rvisitor = rcppsw::patterns::visitor;
namespace controller {
namespace depth0 {
class crw_controller;
class dpo_controller;
class mdpo_controller;
} // namespace depth0
namespace depth1 {
class gp_dpo_controller;
class gp_mdpo_controller;
} // namespace depth1
namespace depth2 {
class grp_dpo_controller;
class grp_mdpo_controller;
} // namespace depth2
} // namespace controller

namespace fsm {
namespace depth0 {
class crw_fsm;
class dpo_fsm;
class free_block_to_nest_fsm;
} // namespace depth0
class block_to_goal_fsm;
} // namespace fsm
namespace tasks {
class base_foraging_task;
namespace depth0 {
class generalist;
}
namespace depth1 {
class harvester;
} // namespace depth1
namespace depth2 {
class cache_starter;
class cache_finisher;
} // namespace depth2
} // namespace tasks

NS_START(events, detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
struct block_vanished_visit_set {
  using value =
      rvisitor::precise_visit_set<controller::depth0::crw_controller,
                                 controller::depth0::dpo_controller,
                                 controller::depth0::mdpo_controller,
                                 controller::depth1::gp_dpo_controller,
                                 controller::depth1::gp_mdpo_controller,
                                 controller::depth2::grp_dpo_controller,
                                 controller::depth2::grp_mdpo_controller,
                                 tasks::depth0::generalist,
                                 tasks::depth1::harvester,
                                 tasks::depth2::cache_starter,
                                 tasks::depth2::cache_finisher,
                                 fsm::depth0::crw_fsm,
                                 fsm::depth0::dpo_fsm,
                                 fsm::depth0::free_block_to_nest_fsm,
                                 fsm::block_to_goal_fsm>;
};

/*
 * @class block_vanished
 * @ingroup fordyca events detail
 *
 * @brief Created whenever a robot is serving a block pickup penalty, but while
 * serving the penalty the block it is waiting for vanishes due to another
 * robot picking it up (ramp blocks only).
 */
class block_vanished : public rcppsw::er::client<block_vanished> {
 public:
  explicit block_vanished(uint block_id);
  ~block_vanished(void) override = default;

  block_vanished(const block_vanished& op) = delete;
  block_vanished& operator=(const block_vanished& op) = delete;

  /* depth0 foraging */
  void visit(controller::depth0::crw_controller& controller);
  void visit(controller::depth0::dpo_controller& controller);
  void visit(controller::depth0::mdpo_controller& controller);
  void visit(tasks::depth0::generalist& task);
  void visit(fsm::depth0::crw_fsm& fsm);
  void visit(fsm::depth0::dpo_fsm& fsm);

  /* depth1 foraging */
  void visit(fsm::depth0::free_block_to_nest_fsm& fsm);
  void visit(fsm::block_to_goal_fsm& fsm);
  void visit(tasks::depth1::harvester& task);
  void visit(controller::depth1::gp_dpo_controller& controller);
  void visit(controller::depth1::gp_mdpo_controller& controller);

  /* depth2 foraging */
  void visit(controller::depth2::grp_dpo_controller& controller);
  void visit(controller::depth2::grp_mdpo_controller& controller);
  void visit(tasks::depth2::cache_starter& task);
  void visit(tasks::depth2::cache_finisher& task);

 private:
  void dispatch_free_block_interactor(tasks::base_foraging_task* task);

  /* clang-format off */
  uint m_block_id;
  /* clang-format on */
};

/**
 * @brief We use the picky visitor in order to force compile errors if a call to
 * a visitor is made that involves a visitee that is not in our visit set
 * (i.e. remove the possibility of implicit upcasting performed by the
 * compiler).
 */
using block_vanished_visitor_impl =
    rvisitor::precise_visitor<detail::block_vanished,
                             detail::block_vanished_visit_set::value>;

NS_END(detail);

class block_vanished_visitor : public detail::block_vanished_visitor_impl {
  using detail::block_vanished_visitor_impl::block_vanished_visitor_impl;
};

NS_END(events, fordyca);

#endif /* INCLUDE_FORDYCA_EVENTS_BLOCK_VANISHED_HPP_ */
