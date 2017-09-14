/**
 * @file foraging_signal.hpp
 *
 * @copyright 2017 John Harwell, All rights reserved.
 *
 * This file is part of RCPPSW.
 *
 * RCPPSW is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * RCPPSW is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * RCPPSW.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef INCLUDE_FORDYCA_CONTROLLER_FORAGING_SIGNAL_HPP_
#define INCLUDE_FORDYCA_CONTROLLER_FORAGING_SIGNAL_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/patterns/state_machine/event.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, controller);
namespace fsm = rcppsw::patterns::state_machine;

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * @brief Signals that sub-states can return in order to notify their super
 * states that a conditiot that they do not know how to handle has arised.
 */
class foraging_signal : public fsm::event_signal {
 public:
  enum type {
    /**
     * A block has been located (i.e. appeared within a robot's LOS)
     */
    BLOCK_LOCATED = fsm::event_signal::EXTERNAL_SIGNALS,
    BLOCK_ACQUIRED,    /// A robot has picked up a block
    ARRIVED_AT_TARGET  /// A robot has arrived at its target
  };
};

NS_END(controller, fordyca);

#endif /* INCLUDE_FORDYCA_CONTROLLER_FORAGING_SIGNAL_HPP_ */