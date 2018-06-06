/**
 * @file mal_hspec_stm32
 * @author Olivier Allaire
 * @date May 2 2015
 * @copyright Copyright (c) 2015 Olivier Allaire
 * @par This file is part of MAL.
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
 * @brief Definitions for STM32F0.
 * @note MAL_FLOAT is not defined for all STM32F0 platforms.
 */

#ifndef HSPEC_STM_MAL_HSPEC_STM32F0_H_
#define HSPEC_STM_MAL_HSPEC_STM32F0_H_

#include "stm32f0/stm32f0xx.h"
#include "cmsis/core_cmInstr.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "clock/mal_clock.h"
#include "std/mal_bool.h"

#include "mal_hspec_stm32f0_serial.h"
#include "mal_hspec_stm32f0_adc.h"
#include "mal_hspec_stm32f0_dac.h"
#include "mal_hspec_stm32f0_gpio.h"

#define MAL_HSPEC_STM32F0_MAX_FREQUENCY	48000000

#define MAL_HSPEC_STM32F0_HSI_PLL_DIV	1

#define MAL_HSPEC_STM32F0_HSI_PLL_SRC	RCC_PLLSource_HSI

/**
 * @brief Indicates if a PLL division is available for the current family of
 * STM32F0. Must be defined for every family.
 * @param source The source of the clock.
 * @return Returns true if available.
 */
bool mal_hspec_stm32f0_is_pll_div_available(mal_system_clk_src_e source);

#endif /* HSPEC_STM_MAL_HSPEC_STM32F0_H_ */
