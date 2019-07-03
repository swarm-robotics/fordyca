/**
 * @file dpo_perception_metrics_collector.hpp
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

#ifndef INCLUDE_FORDYCA_METRICS_PERCEPTION_DPO_PERCEPTION_METRICS_COLLECTOR_HPP_
#define INCLUDE_FORDYCA_METRICS_PERCEPTION_DPO_PERCEPTION_METRICS_COLLECTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string>
#include <list>

#include "rcppsw/metrics/base_metrics_collector.hpp"
#include "rcppsw/swarm/pheromone_density.hpp"
#include "fordyca/nsalias.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, metrics, perception);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class dpo_perception_metrics_collector
 * @ingroup fordyca metrics blocks
 *
 * @brief Collector for \ref dpo_perception_metrics.
 *
 * Metrics are written out at the specified collection interval.
 */
class dpo_perception_metrics_collector final
    : public rmetrics::base_metrics_collector {
 public:
  /**
   * @param ofname The output file name.
   * @param interval Collection interval.
   */
  dpo_perception_metrics_collector(const std::string& ofname, uint interval);

  void reset(void) override;
  void collect(const rmetrics::base_metrics& metrics) override;
  void reset_after_interval(void) override;

 private:
  std::list<std::string> csv_header_cols(void) const override;
  bool csv_line_build(std::string& line) override;

  /* clang-format off */
  uint                      m_int_robot_count{0};
  uint                      m_int_known_blocks{0};
  uint                      m_int_known_caches{0};
  rswarm::pheromone_density m_int_block_density_sum{};
  rswarm::pheromone_density m_int_cache_density_sum{};

  uint                      m_cum_robot_count{0};
  uint                      m_cum_known_blocks{0};
  uint                      m_cum_known_caches{0};
  rswarm::pheromone_density m_cum_block_density_sum{};
  rswarm::pheromone_density m_cum_cache_density_sum{};
  /* clang-format on */
};

NS_END(perception, metrics, fordyca);

#endif /* INCLUDE_FORDYCA_METRICS_PERCEPTION_DPO_PERCEPTION_METRICS_COLLECTOR_HPP_ */