/**
 * @file block_to_cache_site_fsm.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/fsm/depth2/block_to_cache_site_fsm.hpp"

#include "fordyca/fsm/expstrat/foraging_expstrat.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, fsm, depth2);

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
block_to_cache_site_fsm::block_to_cache_site_fsm(
    const controller::block_sel_matrix* bsel_matrix,
    const controller::cache_sel_matrix* csel_matrix,
    crfootbot::footbot_saa_subsystem* const saa,
    ds::dpo_store* const store,
    std::unique_ptr<expstrat::foraging_expstrat> exp_behavior)
    : block_to_goal_fsm(&m_cache_fsm, &m_block_fsm, saa),
      m_cache_fsm(csel_matrix, saa, store),
      m_block_fsm(bsel_matrix, saa, store, std::move(exp_behavior)) {}

/*******************************************************************************
 * FSM Metrics
 ******************************************************************************/
cfmetrics::goal_acq_metrics::goal_type block_to_cache_site_fsm::acquisition_goal(
    void) const {
  if (ekST_ACQUIRE_BLOCK == current_state() ||
      ekST_WAIT_FOR_BLOCK_PICKUP == current_state()) {
    return cfmetrics::goal_acq_metrics::goal_type(
        foraging_acq_goal::type::ekBLOCK);
  } else if (ekST_TRANSPORT_TO_GOAL == current_state() ||
             ekST_WAIT_FOR_BLOCK_DROP == current_state()) {
    return cfmetrics::goal_acq_metrics::goal_type(
        foraging_acq_goal::type::ekCACHE_SITE);
  }
  return cfmetrics::goal_acq_metrics::goal_type(foraging_acq_goal::type::ekNONE);
} /* acquisition_goal() */

foraging_transport_goal::type block_to_cache_site_fsm::block_transport_goal(
    void) const {
  if (ekST_TRANSPORT_TO_GOAL == current_state() ||
      ekST_WAIT_FOR_BLOCK_DROP == current_state()) {
    return foraging_transport_goal::type::ekCACHE_SITE;
  }
  return foraging_transport_goal::type::ekNONE;
} /* acquisition_goal() */

NS_END(depth2, controller, fordyca);
