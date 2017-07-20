/*
 * test_mal_timer.h
 *
 *  Created on: Jan 24, 2017
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

#ifndef TIMER_TEST_MAL_TIMER_H_
#define TIMER_TEST_MAL_TIMER_H_

#include "timer/mal_timer.h"
#include "gpio/mal_gpio.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"

typedef struct {
	mal_timer_e timer;
	uint64_t count;
} test_mal_timer_input_capture_t;

void test_mal_hspec_timer_init_tick_1khz(mal_timer_e timer);

void test_mal_hspec_timer_get_1khz_direct_init(mal_timer_e timer, const void **direct_init);

void test_mal_hspec_timer_direct_init_tick_1khz(mal_timer_e timer);

void test_mal_hspec_timer_init_count_1khz(mal_timer_e timer);

mal_error_e test_mal_timer_get_valid_pwm_ios(mal_timer_e timer, const mal_gpio_s **ios, uint8_t *size);

void test_mal_hspec_timer_init_pwm_1khz(mal_timer_e timer);

void test_mal_hspec_timer_pwm_50pc_dc_1khz(mal_timer_e timer, const mal_gpio_s *io);

mal_error_e mal_hspec_get_valid_input_capture_ios(mal_timer_e timer, const mal_gpio_s **ios, uint8_t *size);

void test_mal_hspec_timer_input_capture_1khz(mal_timer_e timer, const mal_gpio_s *io, volatile test_mal_timer_input_capture_t *input_capture_info);

void test_mal_timer(void);

#endif /* TIMER_TEST_MAL_TIMER_H_ */
