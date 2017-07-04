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

/**
 * Struct for direct initialization of a timer interface.
 */
typedef struct {
	uint16_t prescaler;	//!< Prescaler register
	uint32_t period;	//!< Period register
} mal_hspec_stm32f0_timer_direct_init_s;

mal_error_e mal_hspec_stm32f0_timer_count_direct_init(mal_timer_e timer, mal_hertz_t frequency, const void *direct_init);

TIM_TypeDef* mal_hspec_stm32f0_timer_get_typedef(mal_timer_e timer);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_TIMER_H_ */
