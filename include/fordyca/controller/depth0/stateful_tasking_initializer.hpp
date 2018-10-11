/**
 * @file stateful_tasking_initializer.hpp
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

#ifndef INCLUDE_FORDYCA_CONTROLLER_DEPTH0_STATEFUL_TASKING_INITIALIZER_HPP_
#define INCLUDE_FORDYCA_CONTROLLER_DEPTH0_STATEFUL_TASKING_INITIALIZER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/common/common.hpp"
#include "rcppsw/er/client.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace rcppsw {
namespace task_allocation {
class bi_tdgraph_executive;
class bi_tdgraph;
}}
namespace ta = rcppsw::task_allocation;

NS_START(fordyca);
namespace params {
namespace depth0 { class stateful_controller_repository; }
}

NS_START(controller);
class saa_subsystem;
class base_perception_subsystem;
class block_selection_matrix;
namespace er = rcppsw::er;
NS_START(depth0);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class stateful_tasking_initializer
 * @ingroup controller depth0
 *
 * @brief A helper class to offload initialization of the task tree for depth0
 * foraging.
 */
class stateful_tasking_initializer : public er::client<stateful_tasking_initializer> {
 public:
  stateful_tasking_initializer(const controller::block_selection_matrix* sel_matrix,
                               saa_subsystem* saa,
                               base_perception_subsystem* perception);
  virtual ~stateful_tasking_initializer(void);
  stateful_tasking_initializer& operator=(const stateful_tasking_initializer& other) = delete;
  stateful_tasking_initializer(const stateful_tasking_initializer& other) = delete;

  std::unique_ptr<ta::bi_tdgraph_executive>
  operator()(params::depth0::stateful_controller_repository *const stateful_repo);

 protected:
  void stateful_tasking_init(params::depth0::stateful_controller_repository* stateful_repo);
  const base_perception_subsystem* perception(void) const { return m_perception; }
  base_perception_subsystem* perception(void) { return m_perception; }

  controller::saa_subsystem* saa_subsystem(void) const { return m_saa; }
  ta::bi_tdgraph* graph(void) { return m_graph; }
  const ta::bi_tdgraph* graph(void) const { return m_graph; }
  const block_selection_matrix* block_sel_matrix(void) const { return mc_sel_matrix; }

 private:
  // clang-format off
  controller::saa_subsystem* const    m_saa;
  base_perception_subsystem* const    m_perception;
  const block_selection_matrix* const mc_sel_matrix;
  ta::bi_tdgraph*                     m_graph;
  // clang-format on
};

NS_END(depth0, controller, fordyca);

#endif /* INCLUDE_FORDYCA_CONTROLLER_DEPTH0_STATEFUL_TASKING_INITIALIZER_HPP_ */
