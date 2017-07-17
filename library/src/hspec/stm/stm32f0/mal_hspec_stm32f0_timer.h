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

#include "std/mal_stdint.h"
#include "stm32f0/stm32f0xx.h"
#include "timer/mal_timer.h"
#include "mal_hspec_stm32f0_cmn.h"

/**
 * Struct for direct initialization of a timer interface.
 */
typedef struct {
	uint16_t prescaler;	//!< Prescaler register
	uint32_t period;	//!< Period register
} mal_hspec_stm32f0_timer_direct_init_s;

/**
 * @brief Returns the proper IRQ for the given timer. Must be defined for every
 * family.
 * @param timer The timer to get the IRQ from.
 * @return Returns the IRQ of the timer.
 */
IRQn_Type mal_hspec_stm32f0_get_timer_update_irq(mal_timer_e timer);

/**
 * @brief Returns the IRQ for the compare interrupt of a timer. Must be defined
 * for every family.
 * @param timer The timer to get the IRQ from.
 * @return Returns the IRQ of the timer.
 */
IRQn_Type mal_hspec_stm32f0_get_timer_compare_irq(mal_timer_e timer);

/**
 * Needed by tests.
 */
TIM_TypeDef* mal_hspec_stm32f0_timer_get_typedef(mal_timer_e timer);

/**
 * Needed by tests.
 */
mal_error_e mal_hspec_stm32f0_timer_get_input_clk(mal_timer_e timer, uint64_t *clock);

/**
 * Needed by tests.
 */
uint16_t mal_hspec_stm32f0_timer_get_channel(const mal_gpio_s *gpio, mal_timer_e timer);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_TIMER_H_ */
