/**
 * \file site_selection_metrics_collector.hpp
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

#ifndef INCLUDE_FORDYCA_METRICS_CACHES_SITE_SELECTION_METRICS_COLLECTOR_HPP_
#define INCLUDE_FORDYCA_METRICS_CACHES_SITE_SELECTION_METRICS_COLLECTOR_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <list>
#include <string>

#include "rcppsw/metrics/base_metrics_collector.hpp"
#include "fordyca/fordyca.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, metrics, caches);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class site_selection_metrics_collector
 * \ingroup metrics caches
 *
 * \brief Collector for \ref site_selection_metrics.
 *
 * Metrics CANNOT be collected in parallel; concurrent updates to the gathered
 * stats are not supported. Metrics are output at the specified interval.
 */
class site_selection_metrics_collector final : public rmetrics::base_metrics_collector {
 public:
  /**
   * \param ofname_stem Output file name stem.
   * \param interval Collection interval.
   */
  site_selection_metrics_collector(const std::string& ofname_stem,
                                   const rtypes::timestep& interval);

  void reset(void) override;
  void reset_after_interval(void) override;
  void collect(const rmetrics::base_metrics& metrics) override;

 private:
  struct stats {
    uint int_n_successes{0};
    uint int_n_fails{0};
    uint int_nlopt_stopval{0};
    uint int_nlopt_ftol{0};
    uint int_nlopt_xtol{0};
    uint int_nlopt_maxeval{0};

    uint cum_n_successes{0};
    uint cum_n_fails{0};
    uint cum_nlopt_stopval{0};
    uint cum_nlopt_ftol{0};
    uint cum_nlopt_xtol{0};
    uint cum_nlopt_maxeval{0};
  };

  std::list<std::string> csv_header_cols(void) const override;
  boost::optional<std::string> csv_line_build(void) override;

  /* clang-format off */
  struct stats m_stats{};
  /* clang-format on */
};

NS_END(caches, metrics, fordyca);

#endif /* INCLUDE_FORDYCA_METRICS_CACHES_SITE_SELECTION_METRICS_COLLECTOR_HPP_ */
