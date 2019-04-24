/**
 * @file gp_omdpo_controller.hpp
 *
 * @copyright 2019 John Harwell, All rights reserved.
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

#ifndef INCLUDE_FORDYCA_CONTROLLER_DEPTH1_GP_OMDPO_CONTROLLER_HPP_
#define INCLUDE_FORDYCA_CONTROLLER_DEPTH1_GP_OMDPO_CONTROLLER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/controller/depth1/gp_mdpo_controller.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller);
class oracular_info_receptor;
NS_START(depth1);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class gp_omdpo_controller
 * @ingroup fordyca controller depth1
 *
 * @brief A foraging controller built on \ref gp_mdpo_controller
 * that has perfect information about on or more of the following, depending on
 * configuration:
 *
 * - Block/cache locations
 * - Task durations/estimates
 */
class gp_omdpo_controller : public depth1::gp_mdpo_controller,
                            public rer::client<gp_omdpo_controller> {
 public:
  using gp_dpo_controller::perception;

  gp_omdpo_controller(void);
  ~gp_omdpo_controller(void) override;

  /* CCI_Controller overrides */
  void ControlStep(void) override;

  std::type_index type_index(void) const override { return {typeid(*this)}; }

  void oracle_init(std::unique_ptr<oracular_info_receptor> receptor);

 private:
  /* clang-format off */
  std::unique_ptr<oracular_info_receptor> m_receptor;
  /* clang-format on */
};

NS_END(depth1, controller, fordyca);

#endif /* INCLUDE_FORDYCA_CONTROLLER_DEPTH1_GP_OMDPO_CONTROLLER_HPP_ */
