/*
 * mal_hspec_stm32f0_timer.c
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

#include "mal_hspec_stm32f0_timer.h"
#include "std/mal_stdlib.h"
#include "std/mal_math.h"
#include "stm32f0/stm32f0xx_rcc.h"

#define INVOKE_CALLBACK(timer) do { \
	if (timer_callbacks[timer] != NULL) { \
		timer_callbacks[timer](timer); \
	} \
}while(0)

static mal_error_e init_timer_rcc(mal_hspec_timer_e timer);

static uint32_t get_rcc_timer(mal_hspec_timer_e timer);

static TIM_TypeDef* get_timer_typedef(mal_hspec_timer_e timer);

static mal_hspec_timer_callback_t timer_callbacks[MAL_HSPEC_TIMER_SIZE];

mal_error_e mal_hspec_stm32f0_timer_init(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback) {
	mal_error_e result;
	// Initialise peripheral clock
	result = init_timer_rcc(timer);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	// Get timer frequency
	uint64_t timer_frequency;
	result = mal_hspec_stm32f0_timer_get_input_clk(timer, &timer_frequency);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Computations will be done in integers for speed. We will use uHz for the current precision
	uint64_t uhz_frequency = frequency * 1000000.0f;
	if (uhz_frequency == 0) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	uint64_t uhz_delta = delta * 1000000.0f;
	// Initialise time base timer
	TIM_TimeBaseInitTypeDef params;
	TIM_TimeBaseStructInit(&params);
	params.TIM_CounterMode = TIM_CounterMode_Up;
	// Choose longest count to get maximum precision
	uint16_t prescaler;
	uint16_t period;
	bool found = false;
	uint64_t smallest_delta = UINT64_MAX;
	for (period = UINT16_MAX; period > 0; period--) {
		for (prescaler = 1; prescaler <= UINT16_MAX; prescaler++) {
			// Resolution is in uHz to handle fractions of Hz
			uint64_t potential_frequency = (timer_frequency * 1000000) / ((uint64_t)period * (uint64_t)prescaler);
			uint64_t actual_delta;
			if (potential_frequency >= uhz_frequency) {
				actual_delta = potential_frequency - uhz_frequency;
			} else {
				actual_delta = uhz_frequency - potential_frequency;
			}
			if (actual_delta <= uhz_delta) {
				found = true;
				break;
			} else {
				if (actual_delta < smallest_delta) {
					smallest_delta = actual_delta;
				}
			}
			// If we get here and the potential frequency is smaller than the
			// target frequency, we can change the period as the potential
			// frequency is just gonna keep getting smaller if we continue
			if (potential_frequency < uhz_frequency) {
				break;
			}
		}
		if (found) {
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	params.TIM_Prescaler = prescaler - 1;
	params.TIM_Period = period;
	TIM_TimeBaseInit(tim, &params);
	// Save tick handle
	timer_callbacks[timer] = callback;
	// Enable NVIC interrupt for timer
	IRQn_Type irq = mal_hspec_stm32f0_get_timer_update_irq(timer);
	NVIC_EnableIRQ(irq);
	NVIC_SetPriority(irq, 2); // Find a way to manage priorities for interrupts
	// Enable timer interrupt
	TIM_ITConfig(tim, TIM_IT_Update, ENABLE);
	// Enable timer
	TIM_Cmd(tim, ENABLE);

	return MAL_ERROR_OK;
}

static mal_error_e init_timer_rcc(mal_hspec_timer_e timer) {
switch (timer) {
case MAL_HSPEC_TIMER_1:
case MAL_HSPEC_TIMER_15:
case MAL_HSPEC_TIMER_16:
case MAL_HSPEC_TIMER_17:
	RCC_APB2PeriphClockCmd(get_rcc_timer(timer), ENABLE);
	break;
case MAL_HSPEC_TIMER_2:
case MAL_HSPEC_TIMER_3:
case MAL_HSPEC_TIMER_6:
case MAL_HSPEC_TIMER_7:
case MAL_HSPEC_TIMER_14:
default:
	RCC_APB1PeriphClockCmd(get_rcc_timer(timer), ENABLE);
	break;
}
return MAL_ERROR_OK;
}

static uint32_t get_rcc_timer(mal_hspec_timer_e timer) {
switch (timer) {
case MAL_HSPEC_TIMER_1:
	return RCC_APB2Periph_TIM1;
case MAL_HSPEC_TIMER_2:
	return RCC_APB1Periph_TIM2;
case MAL_HSPEC_TIMER_3:
	return RCC_APB1Periph_TIM3;
case MAL_HSPEC_TIMER_6:
	return RCC_APB1Periph_TIM6;
case MAL_HSPEC_TIMER_7:
	return RCC_APB1Periph_TIM7;
case MAL_HSPEC_TIMER_14:
	return RCC_APB1Periph_TIM14;
case MAL_HSPEC_TIMER_15:
	return RCC_APB2Periph_TIM15;
case MAL_HSPEC_TIMER_16:
	return RCC_APB2Periph_TIM16;
case MAL_HSPEC_TIMER_17:
default:
	return RCC_APB2Periph_TIM17;
}
}

static TIM_TypeDef* get_timer_typedef(mal_hspec_timer_e timer) {
	switch (timer) {
	case MAL_HSPEC_TIMER_1:
		return TIM1;
	case MAL_HSPEC_TIMER_2:
		return TIM2;
	case MAL_HSPEC_TIMER_3:
		return TIM3;
	case MAL_HSPEC_TIMER_6:
		return TIM6;
	case MAL_HSPEC_TIMER_7:
		return TIM7;
	case MAL_HSPEC_TIMER_14:
		return TIM14;
	case MAL_HSPEC_TIMER_15:
		return TIM15;
	case MAL_HSPEC_TIMER_16:
		return TIM16;
	case MAL_HSPEC_TIMER_17:
		return TIM17;
	default:
		return NULL;
	}
}

mal_error_e mal_hspec_stm32f0_timer_get_input_clk(mal_hspec_timer_e timer,
	uint64_t *clock) {
	// MCU clocks
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// APB Prescaler
	uint32_t apb_pre = RCC->CFGR & RCC_CFGR_PPRE;

	*clock = clocks.PCLK_Frequency;
	if (apb_pre != RCC_CFGR_PPRE_DIV1) {
		// Times 2, see clock tree in datasheet
		*clock += *clock;
	}
	return MAL_ERROR_OK;
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_1);
}

void TIM2_IRQHandler(void) {
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_2);
}

void TIM3_IRQHandler(void) {
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_3);
}

void TIM6_DAC_IRQHandler(void) {
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_6);
}

void TIM7_IRQHandler(void) {
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_7);
}

void TIM14_IRQHandler(void) {
	TIM_ClearFlag(TIM14, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_14);
}

void TIM15_IRQHandler(void) {
	TIM_ClearFlag(TIM15, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_15);
}

void TIM16_IRQHandler(void) {
	TIM_ClearFlag(TIM16, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_16);
}

void TIM17_IRQHandler(void) {
	TIM_ClearFlag(TIM17, TIM_FLAG_Update);
	// Handles tick
	INVOKE_CALLBACK(MAL_HSPEC_TIMER_17);
}

mal_error_e mal_hspec_stm32f0_timer_free(mal_hspec_timer_e timer) {
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	// Disable timer
	TIM_Cmd(tim, DISABLE);
	// Disable interrupt
	TIM_ITConfig(tim, TIM_IT_Update, DISABLE);
	NVIC_DisableIRQ(mal_hspec_stm32f0_get_timer_update_irq(timer));
	// Unitialise timer
	TIM_DeInit(tim);
	// Remove callback
	timer_callbacks[timer] = NULL;

	return MAL_ERROR_OK;
}