/**
 * @file base_foraging_controller.hpp
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

#ifndef INCLUDE_FORDYCA_CONTROLLER_BASE_FORAGING_CONTROLLER_HPP_
#define INCLUDE_FORDYCA_CONTROLLER_BASE_FORAGING_CONTROLLER_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <argos3/core/control_interface/ci_controller.h>
#include "rcppsw/er/client.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca);

namespace representation { class block; class line_of_sight; }
namespace params { struct output_params; }

NS_START(controller);

class actuator_manager;
class base_foraging_sensors;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @class base_foraging_controller
 *
 * @brief The base controller foraging class that all FORDYCA controllers derive
 * from. It holds all functionality common to all controllers, as well that some
 * that is stubbed out here, but overridden in derived classes which allows this
 * class to be used as the robot controller handle when rendering QT graphics
 * overlays.
 */
class base_foraging_controller : public argos::CCI_Controller,
                                 public rcppsw::er::client {
 public:
  base_foraging_controller(void);
  ~base_foraging_controller(void) override = default;

  base_foraging_controller(const base_foraging_controller& other) = delete;
  base_foraging_controller& operator=(const base_foraging_controller& other) = delete;

  /* CCI_Controller overrides */
  void Init(argos::TConfigurationNode& node) override;
  void Reset(void) override;

  /**
   * @brief Set whether or not a robot is supposed to display it's LOS as a
   * square of the appropriate size during simulation.
   */
  void display_los(bool display_los) { m_display_los = display_los; }

  /**
   * @brief If \c TRUE, then the robot should display its approximate LOS as a
   * circle on the ground during simulation.
   */
  bool display_los(void) const { return m_display_los; }

  /**
   * @brief Set whether or not a robot is supposed to display it's ID above its
   * head during simulation.
   */
  void display_id(bool display_id) { m_display_id = display_id; }

  /**
   * @brief If \c TRUE, then the robot should display its ID above its head
   * during simulation.
   */
  bool display_id(void) const { return m_display_id; }

  /**
   * @brief If \c TRUE, the robot is currently at least most of the way in the
   * nest, as reported by the sensors.
   */
  bool in_nest(void) const;

  /**
   * @brief Get the current LOS for the robot.
   *
   * By default this returns nullptr. It is only here so that it this class can
   * be used as the robot controller handle when rendering QT graphics
   * overlays.
   */
  virtual const representation::line_of_sight* los(void) const { return nullptr; }

  /**
   * @brief Return if the robot is currently carrying a block.
   */
  bool is_carrying_block(void) const { return nullptr != m_block; }

  /**
   * @brief Return the block robot is carrying, or NULL if the robot is not
   * currently carrying a block.
   */
  representation::block* block(void) const { return m_block; }

  /**
   * @brief Set the block that the robot is carrying.
   */
  void block(representation::block* block) { m_block = block; }

  /**
   * @brief If \c TRUE, then the robot thinks that it is on top of a block.
   *
   * On rare occasions this may be a false positive, which is why it is also
   * checked in the loop functions before passing any events to the
   * controller. One such occasion that is known to occur is the first timestep,
   * because the sensors have not yet finished initializing, and will return the
   * values that are incidentally the same as those that correspond to a block
   * being found.
   */
  bool block_detected(void) const;

  /**
   * @brief Set the current clock tick.
   *
   * In a real world, each robot would maintain its own clock tick, and overall
   * there would no doubt be considerable skew; this is a simulation hack that
   * makes things much nicer/easier to deal with.
   */
  void tick(uint tick);

 protected:
  const std::shared_ptr<actuator_manager>& actuators(void) const { return m_actuators; }
  const std::shared_ptr<rcppsw::er::server>& server(void) const { return m_server; }
  const std::shared_ptr<base_foraging_sensors>& sensors(void) const { return m_sensors; }

 private:
  void output_init(const struct params::output_params* const params);
  std::string log_header_calc(void);
  std::string dbg_header_calc(void);

  bool                                   m_display_los;
  bool                                   m_display_id;
  representation::block*                 m_block;
  std::shared_ptr<actuator_manager>      m_actuators;
  std::shared_ptr<base_foraging_sensors> m_sensors;
  std::shared_ptr<rcppsw::er::server>    m_server;
};

NS_END(fordyca, controller);

#endif /* INCLUDE_FORDYCA_CONTROLLER_BASE_FORAGING_CONTROLLER_HPP_ */
