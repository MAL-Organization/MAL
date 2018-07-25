/*
 * mal_hspec_stm32f7_reset.c
 *
 *  Created on: May 7, 2018
 *      Author: olivi
 */
/*
 * Copyright (c) 2018 Olivier Allaire
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
#include "stm32f7/stm32f7xx_hal_rcc.h"

mal_reset_source_e mal_reset_get_source_unmanaged(void) {
    mal_reset_source_e source = MAL_RESET_SOURCE_UNKNOWN;
    // Get current source based on RCC flags
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
        source = MAL_RESET_SOURCE_SOFTWARE;
    } else if ((__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) == ENABLE) ||
               (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == ENABLE)) {
        source = MAL_RESET_SOURCE_WATCHDOG;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) == ENABLE) {
        source = MAL_RESET_SOURCE_POWER;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) == ENABLE) {
        source = MAL_RESET_SOURCE_SLEEP;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) == ENABLE) {
        source = MAL_RESET_SOURCE_RESET_PIN;
    }
    // Reset source
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return source;
}
