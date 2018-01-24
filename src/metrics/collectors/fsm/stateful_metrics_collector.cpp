/**
 * @file stateful_metrics_collector.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/metrics/collectors/fsm/stateful_metrics_collector.hpp"
#include "fordyca/metrics/collectible_metrics/fsm/stateful_metrics.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, metrics, collectors, fsm);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
stateful_metrics_collector::stateful_metrics_collector(const std::string& ofname,
                                                       bool collect_cum,
                                                       uint collect_interval)
    : base_metric_collector(ofname, collect_cum), m_stats() {
  if (collect_cum) {
    use_interval(true);
    interval(collect_interval);
  }
}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
std::string stateful_metrics_collector::csv_header_build(
    const std::string& header) {
  // clang-format off
  if (collect_cum()) {
  return base_metric_collector::csv_header_build(header) +
      "n_acquiring_block"  + separator() +
      "n_cum_acquiring_block"  + separator() +
      "n_vectoring_to_block"  + separator() +
      "n_cum_vectoring_to_block"  + separator();
  }
  return base_metric_collector::csv_header_build(header) +
      "n_acquiring_block"  + separator() +
      "n_vectoring_to_block"  + separator();
  // clang-format on
} /* csv_header_build() */

void stateful_metrics_collector::reset(void) {
  base_metric_collector::reset();
  reset_after_interval();
} /* reset() */

void stateful_metrics_collector::collect(
    const collectible_metrics::base_collectible_metrics& metrics) {
  auto& m =
      static_cast<const collectible_metrics::fsm::stateful_metrics&>(metrics);
  m_stats.n_acquiring_block += static_cast<uint>(m.is_acquiring_block());
  m_stats.n_vectoring_to_block += static_cast<uint>(m.is_vectoring_to_block());

  m_stats.n_cum_acquiring_block += static_cast<uint>(m.is_acquiring_block());
  m_stats.n_cum_vectoring_to_block +=
      static_cast<uint>(m.is_vectoring_to_block());
} /* collect() */

bool stateful_metrics_collector::csv_line_build(std::string& line) {
  if (!((timestep() + 1) % interval() == 0)) {
    return false;
  }
  if (collect_cum()) {
    line = std::to_string(m_stats.n_acquiring_block) + separator() +
           std::to_string(m_stats.n_cum_acquiring_block) + separator() +
           std::to_string(m_stats.n_vectoring_to_block) + separator() +
           std::to_string(m_stats.n_cum_vectoring_to_block) + separator();
  } else {
    line = std::to_string(m_stats.n_acquiring_block) + separator() +
           std::to_string(m_stats.n_vectoring_to_block) + separator();
  }
  return true;
} /* csv_line_build() */

void stateful_metrics_collector::reset_after_interval(void) {
  m_stats.n_cum_acquiring_block = 0;
  m_stats.n_cum_vectoring_to_block = 0;
} /* reset_after_interval() */

void stateful_metrics_collector::reset_after_timestep(void) {
  m_stats.n_acquiring_block = 0;
  m_stats.n_vectoring_to_block = 0;
} /* reset_after_timestep() */

NS_END(fsm, collectors, metrics, fordyca);
