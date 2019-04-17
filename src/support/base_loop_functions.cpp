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

#include "fordyca/controller/base_controller.hpp"
#include "fordyca/params/arena/arena_map_params.hpp"
#include "fordyca/params/output_params.hpp"
#include "fordyca/params/tv/tv_manager_params.hpp"
#include "fordyca/params/visualization_params.hpp"
#include "fordyca/support/tv/tv_manager.hpp"

#include "fordyca/ds/arena_map.hpp"
#include "rcppsw/algorithm/closest_pair2D.hpp"
#include "rcppsw/math/vector2.hpp"
#include "rcppsw/swarm/convergence/convergence_calculator.hpp"
#include "rcppsw/swarm/convergence/convergence_params.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, support);
namespace ralg = rcppsw::algorithm;
namespace rmath = rcppsw::math;
namespace rswc = rcppsw::swarm::convergence;

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/
base_loop_functions::base_loop_functions(void)
    : ER_CLIENT_INIT("fordyca.loop.base"),
      m_arena_map(nullptr),
      m_tv_manager(nullptr),
      m_conv_calc(nullptr) {}

base_loop_functions::~base_loop_functions(void) = default;

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

#ifndef RCPPSW_ER_NREPORT
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
  ndc_push();
  /* parse simulation input file */
  m_params.parse_all(node);

  /* initialize output and metrics collection */
  output_init(m_params.parse_results<params::output_params>());

  /* initialize arena map and distribute blocks */
  arena_map_init(params());

  /* initialize convergence calculations */
  convergence_init(m_params.parse_results<rswc::convergence_params>());

  /* initialize temporal variance injection */
  tv_init(params()->parse_results<params::tv::tv_manager_params>());

  m_floor = &GetSpace().GetFloorEntity();
  std::srand(std::time(nullptr));
  ndc_pop();
} /* Init() */

void base_loop_functions::convergence_init(
    const rswc::convergence_params* const params) {
  m_conv_calc = rcppsw::make_unique<rswc::convergence_calculator>(
      params,
      std::bind(&base_loop_functions::calc_robot_headings,
                this,
                std::placeholders::_1),
      std::bind(&base_loop_functions::calc_robot_nn, this, std::placeholders::_1),
      std::bind(&base_loop_functions::calc_robot_positions,
                this,
                std::placeholders::_1));
} /* convergence_init() */

void base_loop_functions::PreStep(void) {
  m_tv_manager->update();
  m_conv_calc->update();
} /* PreStep() */

void base_loop_functions::tv_init(const params::tv::tv_manager_params* const tvp) {
  m_tv_manager = rcppsw::make_unique<tv::tv_manager>(tvp, this, arena_map());

  for (auto& pair : GetSpace().GetEntitiesByType("foot-bot")) {
    auto* robot = argos::any_cast<argos::CFootBotEntity*>(pair.second);
    auto& controller = dynamic_cast<controller::base_controller&>(
        robot->GetControllableEntity().GetController());
    m_tv_manager->register_controller(controller.entity_id());
    controller.tv_init(m_tv_manager.get());
  } /* for(&pair..) */
} /* tv_init() */

void base_loop_functions::arena_map_init(
    const params::loop_function_repository* const repo) {
  auto* aparams = repo->parse_results<params::arena::arena_map_params>();
  auto* vparams = repo->parse_results<params::visualization_params>();

  m_arena_map = rcppsw::make_unique<ds::arena_map>(aparams);
  if (!m_arena_map->initialize(this)) {
    ER_ERR("Could not initialize arena map");
    std::exit(EXIT_FAILURE);
  }

  m_arena_map->distribute_all_blocks();

  /*
   * If null, visualization has been disabled.
   */
  if (nullptr != vparams) {
    for (auto& block : m_arena_map->blocks()) {
      block->display_id(vparams->block_id);
    } /* for(&block..) */
  }
} /* arena_map_init() */

void base_loop_functions::Reset(void) {
  m_arena_map->distribute_all_blocks();
} /* Reset() */

/*******************************************************************************
 * Metrics
 ******************************************************************************/
std::vector<double> base_loop_functions::calc_robot_nn(uint) const {
  std::vector<rmath::vector2d> v;
  auto& robots = GetSpace().GetEntitiesByType("foot-bot");

  for (auto& entity_pair : robots) {
    auto& robot = *argos::any_cast<argos::CFootBotEntity*>(entity_pair.second);
    rmath::vector2d pos;
    pos.set(robot.GetEmbodiedEntity()
                .GetOriginAnchor()
                .Position.GetX(),
            robot.GetEmbodiedEntity()
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
#pragma omp parallel for num_threads(n_threads)
  for (size_t i = 0; i < robots.size() / 2; ++i) {
    auto dist_func = std::bind(&rmath::vector2d::distance,
                               std::placeholders::_1,
                               std::placeholders::_2);
    auto pts = ralg::closest_pair<rmath::vector2d>()("recursive", v, dist_func);
    size_t old = v.size();
#pragma omp critical
    {
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
    }
  } /* for(i..) */

  return res;
} /* calc_robot_nn() */

std::vector<rmath::radians> base_loop_functions::calc_robot_headings(uint) const {
  std::vector<rmath::radians> v;
  auto& robots = GetSpace().GetEntitiesByType("foot-bot");

  for (auto& entity_pair : robots) {
    auto* robot = argos::any_cast<argos::CFootBotEntity*>(entity_pair.second);
    auto& controller = dynamic_cast<controller::base_controller&>(
        robot->GetControllableEntity().GetController());
    v.push_back(controller.heading().angle());
  } /* for(&entity..) */
  return v;
} /* calc_robot_headings() */

std::vector<rmath::vector2d> base_loop_functions::calc_robot_positions(
    uint) const {
  std::vector<rmath::vector2d> v;
  auto& robots = GetSpace().GetEntitiesByType("foot-bot");

  for (auto& entity_pair : robots) {
    auto* robot = argos::any_cast<argos::CFootBotEntity*>(entity_pair.second);
    auto& controller = dynamic_cast<controller::base_controller&>(
        robot->GetControllableEntity().GetController());
    v.push_back(controller.position());
  } /* for(&entity..) */
  return v;
} /* calc_robot_positions() */

NS_END(support, fordyca);
