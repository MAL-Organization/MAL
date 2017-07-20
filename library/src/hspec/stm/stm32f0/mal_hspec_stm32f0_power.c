/*
 * mal_hspec_stm32f0_power.c
 *
 *  Created on: Jan 26, 2017
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

#include "power/mal_power.h"

extern mal_volts_t mal_hspec_power_rail_vdda_value;

mal_error_e mal_power_get_rail_voltage(mal_power_rail_e rail, mal_volts_t *value) {
	switch (rail) {
		case MAL_POWER_RAIL_VDDA:
			*value = mal_hspec_power_rail_vdda_value;
			return MAL_ERROR_OK;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
}

