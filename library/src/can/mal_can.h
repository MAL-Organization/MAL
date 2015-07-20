/*
 * mal_can.h
 *
 *  Created on: Jun 14, 2015
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CAN_MAL_CAN_H_
#define CAN_MAL_CAN_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec.h"

#define mal_can_add_filter(interface, filter) mal_hspec_can_add_filter(interface, filter)

#define mal_can_remove_filter(interface, filter) mal_hspec_can_remove_filter(interface, filter)

#define mal_can_transmit(interface, msg) mal_hspec_can_transmit(interface, msg)

#define mal_can_init(init) mal_hspec_can_init(init)

#define mal_can_disable_interrupt(interface) mal_hspec_disable_can_interrupt(interface)

#define mal_can_enable_interrupt(interface) mal_hspec_enable_can_interrupt(interface)

mal_error_e mal_can_init(mal_hspec_can_init_s *init);

#endif /* CAN_MAL_CAN_H_ */
