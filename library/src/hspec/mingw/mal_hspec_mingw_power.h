/*
 * mal_hspec_mingw_power.h
 *
 *  Created on: Jan 30, 2017
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_POWER_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_POWER_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"

mal_error_e mal_hspec_mingw_power_get_rail_voltage(mal_hspec_power_rail_e rail, mal_volts_t *value);

void mal_hspec_mingw_power_set_rail_voltage(mal_hspec_power_rail_e rail, mal_volts_t value);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_POWER_H_ */
