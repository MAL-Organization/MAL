/*
 * mal_hspec_stm32f0_gpio.c
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
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "std/mal_stdlib.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx.h"
#include "hspec/mal_hspec.h"
#include "stm32f0/stm32f0xx_gpio.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"

static GPIOSpeed_TypeDef get_gpio_speed(uint64_t speed);

mal_error_e mal_hspec_stm32f0_gpio_init(mal_hpsec_gpio_init_s *gpio_init) {
	// Enable clock domain
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(gpio_init->gpio.port), ENABLE);
	// Set GPIO
	GPIO_InitTypeDef init;
	if (MAL_GPIO_DIR_IN == gpio_init->direction) {
		init.GPIO_Mode = GPIO_Mode_IN;
	} else {
		init.GPIO_Mode = GPIO_Mode_OUT;
		if (MAL_GPIO_OUT_PP == gpio_init->output_config) {
			init.GPIO_OType = GPIO_OType_PP;
		} else {
			init.GPIO_OType = GPIO_OType_OD;
		}
	}
	init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(gpio_init->gpio.pin);
	switch (gpio_init->pupd) {
	case MAL_GPIO_PUPD_PU:
		init.GPIO_PuPd = GPIO_PuPd_UP;
		break;
	case MAL_GPIO_PUPD_PD:
		init.GPIO_PuPd = GPIO_PuPd_DOWN;
		break;
	case MAL_GPIO_PUPD_NONE:
	default:
		init.GPIO_PuPd = GPIO_PuPd_NOPULL;
		break;
	}
	init.GPIO_Speed = get_gpio_speed(gpio_init->speed);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(gpio_init->gpio.port), &init);

	return MAL_ERROR_OK;
}

GPIOSpeed_TypeDef get_gpio_speed(uint64_t speed) {
	if (speed >= 50000000) {
		return GPIO_Speed_Level_3;
	} else if (speed >= 10000000) {
		return GPIO_Speed_Level_2;
	}
	return GPIO_Speed_Level_1;
}

mal_error_e mal_hspec_stm32f0_set_gpio(mal_hspec_gpio_s *gpio, bool value) {
	BitAction action = Bit_RESET;
	mal_error_e result;
	// Check gpio
	result = mal_hspec_is_gpio_valid(gpio);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	if (value) {
		action = Bit_SET;
	}
	GPIO_WriteBit(mal_hspec_stm32f0_get_gpio_typedef(gpio->port), MAL_HSPEC_STM32F0_GET_GPIO_PIN(gpio->pin),
			action);

	return MAL_ERROR_OK;
}

bool mal_hspec_stm32f0_get_gpio(mal_hspec_gpio_s *gpio) {
	uint8_t result = GPIO_ReadInputDataBit(mal_hspec_stm32f0_get_gpio_typedef(gpio->port),
			MAL_HSPEC_STM32F0_GET_GPIO_PIN(gpio->pin));
	if (Bit_SET == result) {
		return true;
	}
	return false;
}

mal_error_e mal_hspec_stm32f0_toggle_gpio(mal_hspec_gpio_s *gpio) {
	uint8_t state = GPIO_ReadOutputDataBit(mal_hspec_stm32f0_get_gpio_typedef(gpio->port), MAL_HSPEC_STM32F0_GET_GPIO_PIN(gpio->pin));
	if (Bit_SET == state) {
		mal_hspec_stm32f0_set_gpio(gpio, false);
	} else {
		mal_hspec_stm32f0_set_gpio(gpio, true);
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_gpio_event_init(mal_hspec_gpio_event_init_s *init) {
	// Enable syscfg clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// Disable interrupt
	//NVIC_DisableIRQ()

}
