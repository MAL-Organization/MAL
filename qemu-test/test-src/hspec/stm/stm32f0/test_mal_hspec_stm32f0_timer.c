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

static const mal_hspec_stm32f0_timer_direct_init_s direct_init_1khz = {
	.prescaler = 0,
	.period = 48000
};

void test_mal_hspec_stm32f0_timer_init_tick_1khz(mal_hspec_timer_e timer) {
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

void test_mal_hspec_stm32f0_timer_get_1khz_direct_init(mal_hspec_timer_e timer, const void **direct_init) {
	*direct_init = &direct_init_1khz;
}

void test_mal_hspec_stm32f0_timer_direct_init_tick_1khz(mal_hspec_timer_e timer) {
	test_mal_hspec_stm32f0_timer_init_tick_1khz(timer);
}

void test_mal_hspec_stm32f0_timer_init_count_1khz(mal_hspec_timer_e timer) {
	mal_error_e result;
	// Get timer typedef
	TIM_TypeDef *tim = mal_hspec_stm32f0_timer_get_timer_typedef(timer);
	// Get timer frequency
	uint64_t timer_frequency;
	result = mal_hspec_stm32f0_timer_get_input_clk(timer, &timer_frequency);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
	// Check timer count frequency
	uint64_t timer_count_freq = timer_frequency / (tim->PSC + 1);
	UCUNIT_CheckIsEqual(1000, timer_count_freq);
}

void test_mal_hspec_stm32f0_timer_init_pwm_1khz(mal_hspec_timer_e timer) {
	test_mal_hspec_stm32f0_timer_init_tick_1khz(timer);
}

void test_mal_hspec_stm32f0_timer_pwm_50pc_dc_1khz(mal_hspec_timer_e timer, const mal_hspec_gpio_s *io) {
	// Get timer
	TIM_TypeDef *tim = mal_hspec_stm32f0_timer_get_timer_typedef(timer);
	// Get compare value
	uint32_t compare_value = 0;
	switch (mal_hspec_stm32f0_timer_get_channel(io, timer)) {
		case TIM_Channel_1:
			compare_value = tim->CCR1;
			break;
		case TIM_Channel_2:
			compare_value = tim->CCR2;
			break;
		case TIM_Channel_3:
			compare_value = tim->CCR3;
			break;
		case TIM_Channel_4:
			compare_value = tim->CCR4;
			break;
		default:
			compare_value = 0;
			break;
	}
	// Check duty cycle
	mal_hspec_timer_pwm_value_t dc;
	dc = (compare_value * MAL_HSPEC_TIMER_PWM_VALUE_MAX) / tim->ARR;
	UCUNIT_CheckIsEqual(MAL_HSPEC_TIMER_PWM_VALUE_MAX / 2, dc);
}
