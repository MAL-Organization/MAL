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
#include "stm32f0/stm32f0xx_gpio.h"
#include "hspec/stm/stm32f0/mal_hspec_stm32f0_cmn.h"

static const mal_hspec_stm32f0_timer_direct_init_s direct_init_1khz = {
	.prescaler = 0,
	.period = 48000
};

void test_mal_hspec_stm32f0_timer_init_tick_1khz(mal_timer_e timer) {
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

void test_mal_hspec_stm32f0_timer_get_1khz_direct_init(mal_timer_e timer, const void **direct_init) {
	*direct_init = &direct_init_1khz;
}

void test_mal_hspec_stm32f0_timer_direct_init_tick_1khz(mal_timer_e timer) {
	test_mal_hspec_stm32f0_timer_init_tick_1khz(timer);
}

void test_mal_hspec_stm32f0_timer_init_count_1khz(mal_timer_e timer) {
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

void test_mal_hspec_stm32f0_timer_init_pwm_1khz(mal_timer_e timer) {
	test_mal_hspec_stm32f0_timer_init_tick_1khz(timer);
}

void test_mal_hspec_stm32f0_timer_pwm_50pc_dc_1khz(mal_timer_e timer, const mal_hspec_gpio_s *io) {
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
	mal_ratio_t dc = MAL_TYPES_INT_RATIO_TO_RATIO(compare_value, tim->ARR);
	UCUNIT_CheckIsAlmostEqual(MAL_TYPES_RATIO_NORMALIZER / 2, dc, 2);
}

void test_mal_hspec_stm32f0_timer_input_capture_1khz(mal_timer_e timer,
													 const mal_hspec_gpio_s *io,
													 volatile test_mal_hspec_timer_input_capture_t *input_capture_info) {
	mal_error_e result;
	test_mal_hspec_stm32f0_timer_init_count_1khz(timer);
	// Force pull down on GPIO
	GPIO_TypeDef *gpio = mal_hspec_stm32f0_get_gpio_typedef(io->port);
    gpio->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (io->pin * 2));
    gpio->PUPDR |= (GPIO_PuPd_DOWN << (io->pin * 2));
	// Reset input data
	input_capture_info->timer = timer + 1;
	input_capture_info->count = UINT64_MAX;
	// Wait for count to change
	uint64_t ref_count;
	result = mal_timer_get_count(timer, &ref_count);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
	for (int i = 0; i < 10000; i++) {
		uint64_t new_count;
		mal_timer_get_count(timer, &new_count);
		if (new_count != ref_count) {
			break;
		}
	}
	// Set pull up
	gpio->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (io->pin * 2));
	gpio->PUPDR |= (GPIO_PuPd_UP << (io->pin * 2));
	// Wait callback execution
	for (int i = 0; i < 10000; i++) {
		if (timer == input_capture_info->timer) {
			break;
		}
	}
	UCUNIT_CheckIsEqual(timer, input_capture_info->timer);
	UCUNIT_CheckIsNotEqual(UINT64_MAX, input_capture_info->count);
}
