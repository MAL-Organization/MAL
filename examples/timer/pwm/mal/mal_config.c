/*
 * Copyright (c) 2019 Olivier Allaire
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
 */#include "clock/mal_clock.h"
#include "power/mal_power.h"


const mal_system_clk_s mal_target_system_clock = {
		.frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(216000000000ULL),
		.src = MAL_SYSTEM_CLK_SRC_INTERNAL,
		.clock_config = NULL
};

mal_hertz_t mal_external_clk_freq = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(0);

mal_volts_t mal_hspec_power_rail_vdda_value = MAL_TYPES_MILLIVOLTS_TO_MAL_VOLTS(3300);

mal_volts_t mal_hspec_power_rail_vdd_value = MAL_TYPES_MILLIVOLTS_TO_MAL_VOLTS(3300);
