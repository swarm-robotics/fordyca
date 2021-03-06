/**
 * \file d0_metrics_aggregator.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/support/d0/d0_metrics_aggregator.hpp"

#include <boost/mpl/for_each.hpp>

#include "rcppsw/mpl/typelist.hpp"
#include "rcppsw/utils/maskable_enum.hpp"

#include "cosm/metrics/collector_registerer.hpp"
#include "cosm/repr/base_block3D.hpp"
#include "cosm/spatial/metrics/goal_acq_metrics.hpp"
#include "cosm/spatial/metrics/movement_metrics.hpp"

#include "fordyca//controller/foraging_controller.hpp"
#include "fordyca/controller/cognitive/d0/dpo_controller.hpp"
#include "fordyca/controller/cognitive/d0/mdpo_controller.hpp"
#include "fordyca/controller/cognitive/d0/odpo_controller.hpp"
#include "fordyca/controller/cognitive/d0/omdpo_controller.hpp"
#include "fordyca/controller/cognitive/foraging_perception_subsystem.hpp"
#include "fordyca/controller/reactive/d0/crw_controller.hpp"
#include "fordyca/fsm/d0/crw_fsm.hpp"
#include "fordyca/fsm/d0/dpo_fsm.hpp"
#include "fordyca/metrics/perception/dpo_perception_metrics.hpp"
#include "fordyca/metrics/perception/dpo_perception_metrics_collector.hpp"
#include "fordyca/metrics/perception/mdpo_perception_metrics.hpp"
#include "fordyca/metrics/perception/mdpo_perception_metrics_collector.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support, d0, detail);

using collector_typelist = rmpl::typelist<
    rmpl::identity<metrics::perception::mdpo_perception_metrics_collector>,
    rmpl::identity<metrics::perception::dpo_perception_metrics_collector> >;

NS_END(detail);

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
d0_metrics_aggregator::d0_metrics_aggregator(
    const cmconfig::metrics_config* const mconfig,
    const cdconfig::grid2D_config* const gconfig,
    const std::string& output_root,
    size_t n_block_clusters)
    : fordyca_metrics_aggregator(mconfig, gconfig, output_root, n_block_clusters),
      ER_CLIENT_INIT("fordyca.support.d0.d0_aggregator") {
  cmetrics::collector_registerer<>::creatable_set creatable_set = {
    { typeid(metrics::perception::mdpo_perception_metrics_collector),
      "perception_mdpo",
      "perception::mdpo",
      rmetrics::output_mode::ekAPPEND },
    { typeid(metrics::perception::dpo_perception_metrics_collector),
      "perception_dpo",
      "perception::dpo",
      rmetrics::output_mode::ekAPPEND }
  };

  cmetrics::collector_registerer<> registerer(mconfig, creatable_set, this);
  boost::mpl::for_each<detail::collector_typelist>(registerer);

  reset_all();
}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
template <class T>
void d0_metrics_aggregator::collect_from_controller(const T* const controller) {
  base_metrics_aggregator::collect_from_controller(controller);

  /*
   * All d0 controllers provide these.
   */
  collect("spatial::movement", *controller);
  collect("strategy::nest_acq", *controller->fsm());
  collect("fsm::interference_counts", *controller->fsm());
  collect("blocks::acq_counts", *controller);
  collect("blocks::transporter", *controller);
  collect("blocks::manipulation", *controller->block_manip_recorder());

  collect_if("fsm::interference_locs2D",
             *controller->fsm(),
             [&](const rmetrics::base_metrics&) {
               return controller->fsm()->inta_tracker()->exp_interference();
             });

  collect_if("blocks::acq_locs2D",
             *controller,
             [&](const rmetrics::base_metrics& metrics) {
               const auto& m =
                   dynamic_cast<const csmetrics::goal_acq_metrics&>(metrics);
               return fsm::foraging_acq_goal::ekBLOCK == m.acquisition_goal() &&
                      m.goal_acquired();
             });

  /*
   * We count "false" explorations as part of gathering metrics on where robots
   * explore.
   */
  collect_if("blocks::acq_explore_locs2D",
             *controller,
             [&](const rmetrics::base_metrics& metrics) {
               const auto& m =
                   dynamic_cast<const csmetrics::goal_acq_metrics&>(metrics);
               return m.is_exploring_for_goal().is_exploring;
             });
  collect_if("blocks::acq_vector_locs2D",
             *controller,
             [&](const rmetrics::base_metrics& metrics) {
               const auto& m =
                   dynamic_cast<const csmetrics::goal_acq_metrics&>(metrics);
               return m.is_vectoring_to_goal();
             });
  /*
   * Only controllers with MDPO perception provide these.
   */
  const auto* mdpo =
      dynamic_cast<const metrics::perception::mdpo_perception_metrics*>(
          controller->perception());
  if (nullptr != mdpo) {
    collect("perception::mdpo", *mdpo);
  }
  /*
   * Only controllers with DPO perception provide these.
   */
  const auto* dpo =
      dynamic_cast<const metrics::perception::dpo_perception_metrics*>(
          controller->perception());
  if (nullptr != dpo) {
    collect("perception::dpo", *dpo);
  }
} /* collect_from_controller() */

/*******************************************************************************
 * Template Instantiations
 ******************************************************************************/
template void d0_metrics_aggregator::collect_from_controller(
    const controller::reactive::d0::crw_controller* const c);
template void d0_metrics_aggregator::collect_from_controller(
    const controller::cognitive::d0::dpo_controller* const c);
template void d0_metrics_aggregator::collect_from_controller(
    const controller::cognitive::d0::mdpo_controller* const c);
template void d0_metrics_aggregator::collect_from_controller(
    const controller::cognitive::d0::odpo_controller* const c);
template void d0_metrics_aggregator::collect_from_controller(
    const controller::cognitive::d0::omdpo_controller* const c);

NS_END(d0, support, fordyca);
