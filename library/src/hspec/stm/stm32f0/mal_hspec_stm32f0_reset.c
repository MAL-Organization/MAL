/*
 * mal_hspec_stm32f0_reset.c
 *
 *  Created on: Jul 27, 2016
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

#include "reset/mal_reset.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "cm0/core_cm0.h"

mal_reset_source_e mal_reset_get_source_unmanaged(void) {
	mal_reset_source_e source = MAL_RESET_SOURCE_UNKNOWN;
	// Get current source based on RCC flags
	if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) == ENABLE) {
		source = MAL_RESET_SOURCE_SOFTWARE;
	} else if ((RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == ENABLE) ||
			   (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == ENABLE)) {
		source = MAL_RESET_SOURCE_WATCHDOG;
	} else if (RCC_GetFlagStatus(RCC_FLAG_PORRST) == ENABLE) {
		source = MAL_RESET_SOURCE_POWER;
	} else if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == ENABLE) {
		source = MAL_RESET_SOURCE_SLEEP;
	} else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) == ENABLE) {
		source = MAL_RESET_SOURCE_RESET_PIN;
	}
	// Reset source
	RCC_ClearFlag();

	return source;
}

void mal_reset_mcu(void) {
	NVIC_SystemReset();
}
