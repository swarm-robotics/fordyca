/**
 * @file base_loop_functions.cpp
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
#include "fordyca/support/base_loop_functions.hpp"
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "rcppsw/math/vector2.hpp"

#include "fordyca/controller/base_controller.hpp"
#include "fordyca/params/output_params.hpp"
#include "rcppsw/algorithm/closest_pair2D.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);
namespace alg = rcppsw::algorithm;
namespace rmath = rcppsw::math;

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
base_loop_functions::base_loop_functions(void)
    : ER_CLIENT_INIT("fordyca.loop.base") {}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void base_loop_functions::output_init(
    const struct params::output_params* const output) {
  if ("__current_date__" == output->output_dir) {
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    m_output_root = output->output_root + "/" +
                    std::to_string(now.date().year()) + "-" +
                    std::to_string(now.date().month()) + "-" +
                    std::to_string(now.date().day()) + ":" +
                    std::to_string(now.time_of_day().hours()) + "-" +
                    std::to_string(now.time_of_day().minutes());
  } else {
    m_output_root = output->output_root + "/" + output->output_dir;
  }

#ifndef ER_NREPORT
  client::set_logfile(log4cxx::Logger::getLogger("fordyca.events"),
                      m_output_root + "/events.log");
  client::set_logfile(log4cxx::Logger::getLogger("fordyca.support"),
                      m_output_root + "/support.log");
  client::set_logfile(log4cxx::Logger::getLogger("fordyca.loop"),
                      m_output_root + "/sim.log");
  client::set_logfile(log4cxx::Logger::getLogger("fordyca.ds.arena_map"),
                      m_output_root + "/sim.log");
  client::set_logfile(log4cxx::Logger::getLogger("fordyca.metrics"),
                      m_output_root + "/metrics.log");
#endif
} /* output_init() */

void base_loop_functions::Init(ticpp::Element& node) {
  /* parse all environment parameters and capture in logfile */
  m_params.parse_all(node);

  /* initialize output and metrics collection */
  output_init(m_params.parse_results<params::output_params>());

  m_floor = &GetSpace().GetFloorEntity();
  std::srand(std::time(nullptr));
} /* Init() */

void base_loop_functions::PreStep(void) { nearest_neighbors(); } /* PreStep() */

std::vector<double> base_loop_functions::nearest_neighbors(void) const {
  std::vector<rmath::vector2d> v;
  auto& robots =
      const_cast<base_loop_functions*>(this)->GetSpace().GetEntitiesByType(
          "foot-bot");

  for (auto& entity_pair : robots) {
    auto& robot = *argos::any_cast<argos::CFootBotEntity*>(entity_pair.second);
    rmath::vector2d pos;
    pos.set(const_cast<argos::CFootBotEntity&>(robot)
                .GetEmbodiedEntity()
                .GetOriginAnchor()
                .Position.GetX(),
            const_cast<argos::CFootBotEntity&>(robot)
                .GetEmbodiedEntity()
                .GetOriginAnchor()
                .Position.GetY());
    v.push_back(pos);
  } /* for(&entity..) */

  /*
   * For each closest pair of robots we find, we add the corresponding distance
   * TWICE to our results vector, because 2 robots i and j are each other's
   * closest robots (if they were not, they would not have been returned by the
   * algorithm).
   */
  std::vector<double> res;
  for (size_t i = 0; i < robots.size() / 2; ++i) {
    auto dist_func = std::bind(&rmath::vector2d::distance,
                               std::placeholders::_1,
                               std::placeholders::_2);
    auto pts = alg::closest_pair<rmath::vector2d>()("recursive", v, dist_func);
    size_t old = v.size();
    v.erase(std::remove_if(v.begin(),
                           v.end(),
                           [&](const auto& pt) {
                             return pt == pts.p1 || pt == pts.p2;
                           }),
            v.end());

    ER_ASSERT(old == v.size() + 2,
              "Closest pair of points not removed from set");
    res.push_back(pts.dist);
    res.push_back(pts.dist);
  } /* for(i..) */

  return res;
} /* nearest_neighbors() */

NS_END(support, fordyca);
