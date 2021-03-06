/**
 * \file crw_adaptor.cpp
 *
 * \copyright 2020 John Harwell, All rights reserved.
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
#include "fordyca/strategy/explore/crw_adaptor.hpp"

#include "cosm/subsystem/saa_subsystemQ3D.hpp"

/*******************************************************************************
 * Namespaces
******************************************************************************/
NS_START(fordyca, strategy, explore);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
crw_adaptor::crw_adaptor(const foraging_strategy::params* c_params,
                         rmath::rng* rng)
    : crw_adaptor(c_params->saa, rng) {}

crw_adaptor::crw_adaptor(csubsystem::saa_subsystemQ3D* saa, rmath::rng* rng)
    : foraging_strategy(saa, rng), decorator(saa, rng) {}

NS_END(explore, strategy, fordyca);
