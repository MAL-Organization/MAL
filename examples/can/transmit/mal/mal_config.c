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
 */
#include "clock/mal_clock.h"
#include "power/mal_power.h"

#if defined(MAL_STM32F7)
static const mal_clock_config_s clock_config = {
		.set_apb1 = true,
		.apb1_frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(27000000000ULL),
		.set_apb2 = true,
		.apb2_frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(13500000000ULL)
};
#endif

const mal_system_clk_s mal_target_system_clock = {
		.frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(216000000000ULL),
		.src = MAL_SYSTEM_CLK_SRC_EXTERNAL,
#if defined(MAL_GNU)
		.clock_config = NULL
#else
		.clock_config = &clock_config
#endif
};

mal_hertz_t mal_external_clk_freq = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(16000000000ULL);

mal_volts_t mal_hspec_power_rail_vdda_value = MAL_TYPES_MILLIVOLTS_TO_MAL_VOLTS(3300);

mal_volts_t mal_hspec_power_rail_vdd_value = MAL_TYPES_MILLIVOLTS_TO_MAL_VOLTS(3300);
