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
#include "hspec/mal_hspec_cmn.h"

const mal_hspec_system_clk_s mal_target_system_clock = {
		.frequency = MAL_TARGET_SYSTEM_CLOCK,
		.src = MAL_TARGET_SYSTEM_CLOCK_SRC
};

uint64_t mal_external_clk_freq = MAL_EXTERNAL_CLOCK_FREQUENCY;

float mal_external_vdda = 3.3f;
