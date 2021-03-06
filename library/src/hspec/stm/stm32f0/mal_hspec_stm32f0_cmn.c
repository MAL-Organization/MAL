/*
 * mal_hspec_stm32f0_cmn.c
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

#include "mal_hspec_stm32f0_cmn.h"
#include "std/mal_stdlib.h"

GPIO_TypeDef* mal_hspec_stm32f0_get_gpio_typedef(mal_gpio_port_e port) {
	switch (port) {
	case MAL_GPIO_PORT_A:
		return GPIOA;
	case MAL_GPIO_PORT_B:
		return GPIOB;
	case MAL_GPIO_PORT_C:
		return GPIOC;
	case MAL_GPIO_PORT_D:
		return GPIOD;
	case MAL_GPIO_PORT_E:
		return GPIOE;
	case MAL_GPIO_PORT_F:
		return GPIOF;
	default:
		return NULL;
	}
}

uint32_t mal_hspec_stm32f0_get_rcc_gpio_port(mal_gpio_port_e port) {
	switch (port) {
	case MAL_GPIO_PORT_A:
		return RCC_AHBPeriph_GPIOA;
	case MAL_GPIO_PORT_B:
		return RCC_AHBPeriph_GPIOB;
	case MAL_GPIO_PORT_C:
		return RCC_AHBPeriph_GPIOC;
	case MAL_GPIO_PORT_D:
		return RCC_AHBPeriph_GPIOD;
	case MAL_GPIO_PORT_E:
		return RCC_AHBPeriph_GPIOE;
	case MAL_GPIO_PORT_F:
	default:
		return RCC_AHBPeriph_GPIOA;
	}
}

mal_error_e mal_hspec_stm32f0_get_pin_af(mal_gpio_port_e port, uint8_t pin, mal_hspec_stm32f0_af_e af,
										 uint8_t *function) {
	const mal_hspec_stm32f0_af_e (*afs)[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE];
	mal_error_e result;
	int port_af, pin_af;

	result = mal_hspec_stm32f0_get_port_afs(port, &afs);
	if (MAL_ERROR_OK != result) {
		return result;
	}

	for (port_af = 0; port_af < MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE; port_af++) {
		for (pin_af = 0; pin_af < MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE; pin_af++) {
			if ((*afs)[pin][port_af][pin_af] == af) {
				*function = (uint8_t)port_af;
				return MAL_ERROR_OK;
			}
		}
	}

	return MAL_ERROR_HARDWARE_INVALID;
}

mal_error_e mal_hspec_stm32f0_get_timer_af(mal_gpio_port_e port, uint8_t pin, mal_timer_e timer, uint8_t *function) {
	const mal_hspec_stm32f0_af_e (*timer_afs)[MAL_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_TIMER_SIZE];
	// Fetch timer alternate functions
	mal_hspec_stm32f0_get_timer_afs(&timer_afs);
	// Extract alternate function
	mal_hspec_stm32f0_af_e target_af = (*timer_afs)[port][pin][timer];
	// Check if it is a valid combinations
	if (MAL_HSPEC_STM32F0_AF_NONE == target_af) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Get alternate function
	return mal_hspec_stm32f0_get_pin_af(port, pin, target_af, function);
}
