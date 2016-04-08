/*
 * mal_hspec_stm32f0_timer.h
 *
 *  Created on: Jun 24, 2015
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_TIMER_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_TIMER_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"

#define mal_hspec_stm32f0_enable_timer_interrupt(timer, active) do {\
	if (active) { \
		NVIC_EnableIRQ(mal_hspec_stm32f0_get_timer_update_irq(timer)); \
	} \
} while(0)

mal_error_e mal_hspec_stm32f0_timer_init(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback);

mal_error_e mal_hspec_stm32f0_timer_get_input_clk(mal_hspec_timer_e timer, uint64_t *clock);

mal_error_e mal_hspec_stm32f0_timer_free(mal_hspec_timer_e timer);

bool mal_hspec_stm32f0_disable_timer_interrupt(mal_hspec_timer_e timer);

mal_error_e mal_hspec_stm32f0_timer_pwm_init(mal_hspec_timer_pwm_init_s *init);

mal_error_e mal_hspec_stm32f0_timer_set_pwm_duty_cycle(mal_hspec_timer_e timer, const mal_hspec_gpio_s *gpio, float duty_cycle);

mal_error_e mal_hspec_stm32f0_timer_count_init(mal_hspec_timer_e timer, float frequency);

mal_error_e mal_hspec_stm32f0_timer_get_resolution(mal_hspec_timer_e timer, uint8_t *resolution);

mal_error_e mal_hspec_stm32f0_timer_get_count_frequency(mal_hspec_timer_e timer, float *frequency);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_TIMER_H_ */
