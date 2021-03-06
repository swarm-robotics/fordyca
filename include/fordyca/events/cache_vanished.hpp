/**
 * \file cache_vanished.hpp
 *
 * \copyright 2017 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_EVENTS_CACHE_VANISHED_HPP_
#define INCLUDE_FORDYCA_EVENTS_CACHE_VANISHED_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/er/client.hpp"
#include "rcppsw/patterns/visitor/visitor.hpp"
#include "rcppsw/types/type_uuid.hpp"

#include "fordyca/controller/controller_fwd.hpp"
#include "fordyca/fsm/fsm_fwd.hpp"
#include "fordyca/tasks/tasks_fwd.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, events, detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class cache_vanished
 * \ingroup events detail
 *
 * \brief Created whenever a robot is serving a cache penalty, but while
 * serving the penalty the cache it is waiting in vanishes due to another
 * robot picking up the last available block.
 */
class cache_vanished : public rer::client<cache_vanished> {
 private:
  struct visit_typelist_impl {
    using controllers = boost::mpl::joint_view<controller::d1::typelist::type,
                                               controller::d2::typelist::type>;
    using tasks = rmpl::typelist<tasks::d1::collector,
                                 tasks::d1::harvester,
                                 tasks::d2::cache_transferer>;
    using fsms =
        rmpl::typelist<fsm::block_to_goal_fsm, fsm::d1::cached_block_to_nest_fsm>;
    using value =
        boost::mpl::joint_view<boost::mpl::joint_view<tasks::type, fsms::type>,
                               controllers::type>;
  };

 public:
  using visit_typelist = visit_typelist_impl::value;
  explicit cache_vanished(const rtypes::type_uuid& cache_id);
  ~cache_vanished(void) override = default;

  cache_vanished(const cache_vanished& op) = delete;
  cache_vanished& operator=(const cache_vanished& op) = delete;

  /* d1 foraging */
  void visit(fsm::block_to_goal_fsm& fsm);
  void visit(fsm::d1::cached_block_to_nest_fsm& fsm);
  void visit(tasks::d1::collector& task);
  void visit(tasks::d1::harvester& task);
  void visit(controller::cognitive::d1::bitd_dpo_controller& controller);
  void visit(controller::cognitive::d1::bitd_mdpo_controller& controller);
  void visit(controller::cognitive::d1::bitd_odpo_controller& controller);
  void visit(controller::cognitive::d1::bitd_omdpo_controller& controller);

  /* d2 foraging */
  void visit(controller::cognitive::d2::birtd_dpo_controller& controller);
  void visit(controller::cognitive::d2::birtd_mdpo_controller& controller);
  void visit(controller::cognitive::d2::birtd_odpo_controller& controller);
  void visit(controller::cognitive::d2::birtd_omdpo_controller& controller);
  void visit(tasks::d2::cache_transferer& task);

 private:
  /* clang-format off */
  void dispatch_cache_interactor(tasks::base_foraging_task* task);

  const rtypes::type_uuid mc_cache_id;
  /* clang-format on */
};

NS_END(detail);

/**
 * \brief We use the precise visitor in order to force compile errors if a call to
 * a visitor is made that involves a visitee that is not in our visit set
 * (i.e. remove the possibility of implicit upcasting performed by the
 * compiler).
 */
using cache_vanished_visitor =
    rpvisitor::filtered_visitor<detail::cache_vanished>;

NS_END(events, fordyca);

#endif /* INCLUDE_FORDYCA_EVENTS_CACHE_VANISHED_HPP_ */
