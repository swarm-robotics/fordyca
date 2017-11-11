/**
 * @file depth0_foraging_sensors.hpp
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

#ifndef INCLUDE_FORDYCA_CONTROLLER_DEPTH0_FORAGING_SENSORS_HPP_
#define INCLUDE_FORDYCA_CONTROLLER_DEPTH0_FORAGING_SENSORS_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fordyca/controller/base_foraging_sensors.hpp"
#include "fordyca/representation/line_of_sight.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
class depth0_foraging_sensors : public base_foraging_sensors {
 public:
  depth0_foraging_sensors(
      const struct params::sensor_params* params,
      argos::CCI_RangeAndBearingSensor* const rabs,
      argos::CCI_FootBotProximitySensor* const proximity,
      argos::CCI_FootBotLightSensor* const light,
      argos::CCI_FootBotMotorGroundSensor* const ground);

  /**
   * @brief Get the robot's current line-of-sight (LOS)
   */
  const representation::line_of_sight* los(void) const { return m_los.get(); }

  /**
   * @brief This is a hack to make it easy for me to run simulations, as I can
   * computer the line of sight for a robot within the loop functions, and just
   * pass it in here. In real robots this routine would be MUCH messier and
   * harder to work with.
   *
   * @param los The new los
   */
  void los(std::unique_ptr<representation::line_of_sight>& los) {
    m_los = std::move(los);
  }

  /**
   * @brief Get the robot's current location.
   *
   * Note that this is set via loop functions, and that robots are not capable
   * of self-localizing. That's not the point of this project, and this was much
   * faster/easier.
   */
  argos::CVector2 robot_loc(void) const { return m_robot_loc; }

  /**
   * @brief Set the robot's current location.
   */
  void robot_loc(argos::CVector2 robot_loc) {
    m_prev_robot_loc = m_robot_loc;
    m_robot_loc = robot_loc;
  }

  /**
   * @brief Get the robot's heading, which is computed from the previous 2
   * calculated (ahem set) robot positions.
   */
  argos::CVector2 robot_heading(void) { return m_robot_loc - m_prev_robot_loc; }

  /**
   * @brief Get the angle of the current robot's heading. A shortcut to help
   * reduce the ache in my typing fingers.
   *
   * @return The heading angle.
   */
  argos::CRadians heading_angle(void) { return robot_heading().Angle(); }

  /**
   * @brief Get the current simulation time tick.
   */
  uint tick(void) const { return m_tick; }

  /**
   * @brief Set the current simulation time tick.
   */
  void tick(uint tick) { m_tick = tick; }

 private:
  depth0_foraging_sensors(const depth0_foraging_sensors& fsm) = delete;
  depth0_foraging_sensors& operator=(const depth0_foraging_sensors& fsm) = delete;

  uint                                        m_tick;
  std::unique_ptr<representation::line_of_sight> m_los;
  argos::CVector2                             m_robot_loc;
  argos::CVector2                             m_prev_robot_loc;
};

NS_END(controller, fordyca);

#endif /* INCLUDE_FORDYCA_CONTROLLER_DEPTH0_FORAGING_SENSORS_HPP_ */