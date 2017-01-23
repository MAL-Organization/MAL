/*
 * test_mal_hspec_stm32f0_timer.c
 *
 *  Created on: Jan 23, 2017
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

#include "test_mal_hspec_stm32f0_timer.h"
#include "hspec/stm/stm32f0/mal_hspec_stm32f0_timer.h"
#include "uCUnit-v1.0.h"

void test_mal_hspec_stm32f0_init_tick_1khz(mal_hspec_timer_e timer) {
	mal_error_e result;
	// Get timer typedef
	TIM_TypeDef *tim = mal_hspec_stm32f0_timer_get_timer_typedef(timer);
	// Get timer frequency
	uint64_t timer_frequency;
	result = mal_hspec_stm32f0_timer_get_input_clk(timer, &timer_frequency);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
	// Check if autoreload event is at 1 kHz
	uint64_t timer_count_freq = timer_frequency / (tim->PSC + 1);
	uint64_t tick_freq = timer_count_freq / tim->ARR;
	UCUNIT_CheckIsEqual(1000, tick_freq);
}
