/**
 * @file block.cpp
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
#include "fordyca/representation/block.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NS_START(fordyca, representation);

/*******************************************************************************
 *  Static Variables
 ******************************************************************************/
rcppsw::math::dcoord2 block::kOutOfSightDLoc = rcppsw::math::dcoord2(100, 100);
argos::CVector2 block::kOutOfSightRLoc = argos::CVector2(100.0, 100.0);

/******************************************************************************
 * Member Functions
 ******************************************************************************/
void block::move_out_of_sight(void) {
  real_loc(kOutOfSightRLoc);
  discrete_loc(kOutOfSightDLoc);
} /* move_out_of_sight() */

std::unique_ptr<block> block::clone(void) const {
  std::unique_ptr<block> tmp = rcppsw::make_unique<block>(xsize());
  tmp->discrete_loc(this->discrete_loc());
  tmp->real_loc(this->real_loc());
  tmp->id(this->id());
  tmp->reset_robot_id();
  return tmp;
} /* clone() */

void block::reset_metrics(void) {
  m_transporters = 0;
  m_first_pickup_time = 0.0;
  m_first_pickup = false;
  m_dist_time = 0.0;
  m_nest_drop_time = 0.0;
} /* reset_metrics(); */

void block::first_pickup_time(double time) {
  if (!m_first_pickup) {
    m_first_pickup_time = time;
    m_first_pickup = true;
  }
} /* first_pickup_time() */

double block::total_transport_time(void) const {
  return m_nest_drop_time - m_dist_time;
} /* total_transport_time() */

double block::initial_wait_time(void) const {
  return m_first_pickup_time - m_dist_time;
} /* initial_wait_time() */

NS_END(representation, fordyca);
