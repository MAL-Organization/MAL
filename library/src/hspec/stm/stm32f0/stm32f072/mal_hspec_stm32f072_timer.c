/*
 * mal_hspec_stm32f072_timer.c
 *
 *  Created on: Sep 6, 2016
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

#include "mal_hspec_stm32f072_timer.h"

static const mal_hspec_timer_e valid_timers[] = {
	MAL_HSPEC_TIMER_1,
	MAL_HSPEC_TIMER_2,
	MAL_HSPEC_TIMER_3,
	MAL_HSPEC_TIMER_6,
	MAL_HSPEC_TIMER_7,
	MAL_HSPEC_TIMER_14,
	MAL_HSPEC_TIMER_15,
	MAL_HSPEC_TIMER_16,
	MAL_HSPEC_TIMER_17
};

mal_error_e mal_hspec_stm32f072_get_valid_timers(const mal_hspec_timer_e **timers, uint8_t *size) {
	*timers = valid_timers;
	*size = sizeof(valid_timers) / sizeof(mal_hspec_timer_e);
	return MAL_ERROR_OK;
}

IRQn_Type mal_hspec_stm32f072_get_timer_update_irq(mal_hspec_timer_e timer) {
	switch (timer) {
	case MAL_HSPEC_TIMER_1:
		return TIM1_BRK_UP_TRG_COM_IRQn;
	case MAL_HSPEC_TIMER_2:
		return TIM2_IRQn;
	case MAL_HSPEC_TIMER_3:
		return TIM3_IRQn;
	case MAL_HSPEC_TIMER_6:
		return TIM6_DAC_IRQn;
	case MAL_HSPEC_TIMER_7:
		return TIM7_IRQn;
	case MAL_HSPEC_TIMER_14:
		return TIM14_IRQn;
	case MAL_HSPEC_TIMER_15:
		return TIM15_IRQn;
	case MAL_HSPEC_TIMER_16:
		return TIM16_IRQn;
	case MAL_HSPEC_TIMER_17:
	default:
		return TIM17_IRQn;
	}
}

IRQn_Type mal_hspec_stm32f072_get_timer_compare_irq(mal_hspec_timer_e timer) {
	switch (timer) {
	case MAL_HSPEC_TIMER_1:
		return TIM1_CC_IRQn;
	case MAL_HSPEC_TIMER_2:
		return TIM2_IRQn;
	case MAL_HSPEC_TIMER_3:
		return TIM3_IRQn;
	case MAL_HSPEC_TIMER_6:
		return TIM6_DAC_IRQn;
	case MAL_HSPEC_TIMER_7:
		return TIM7_IRQn;
	case MAL_HSPEC_TIMER_14:
		return TIM14_IRQn;
	case MAL_HSPEC_TIMER_15:
		return TIM15_IRQn;
	case MAL_HSPEC_TIMER_16:
		return TIM16_IRQn;
	case MAL_HSPEC_TIMER_17:
	default:
		return TIM17_IRQn;
	}
}
