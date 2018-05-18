/*
 * Copyright (c) 2018 Olivier Allaire
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

#include "mal_hspec_gnu_power.h"

static mal_volts_t rail_values[MAL_POWER_RAIL_SIZE];

mal_error_e mal_power_get_rail_voltage(mal_power_rail_e rail, mal_volts_t *value) {
	*value = rail_values[rail];
	return MAL_ERROR_OK;
}

void mal_hspec_gnu_power_set_rail_voltage(mal_power_rail_e rail, mal_volts_t value) {
	rail_values[rail] = value;
}
