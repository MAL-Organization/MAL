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

#include "std/mal_error.h"
#include "stm32f0/stm32f0xx.h"
#include "cmsis/core_cmInstr.h"
#include "stm32f0/stm32f0xx_rcc.h"

#define MAL_HSPEC_STM32F0_MAX_FREQUENCY	48000000

#define MAL_HSPEC_STM32F0_HSI_PLL_DIV	1

#define MAL_HSPEC_STM32F0_HSI_PLL_SRC	RCC_PLLSource_HSI

void mal_hspec_stm32f0_startup();

mal_error_e mal_hspec_stm32f0_set_system_clk(const mal_hspec_system_clk_s *clk);

mal_error_e mal_hspec_stm32f0_serial_init(mal_hspec_serial_init_s *init);

mal_error_e mal_hspec_stm32f0_serial_transmit(mal_hspec_serial_port_e port, uint16_t data);

#endif /* HSPEC_STM_MAL_HSPEC_STM32F0_H_ */
