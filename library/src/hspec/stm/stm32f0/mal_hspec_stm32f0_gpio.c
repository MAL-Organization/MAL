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

#include "std/mal_stdlib.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx.h"
#include "hspec/mal_hspec.h"
#include "stm32f0/stm32f0xx_gpio.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "mal_hspec_stm32f0_gpio.h"

static GPIOSpeed_TypeDef get_gpio_speed(uint64_t speed);
static uint8_t get_exti_port_source(mal_hspec_gpio_port_e port);
static void handle_exti_interrupt(uint32_t exti_line, uint8_t pin);

static mal_hspec_gpio_event_callback_t gpio_event_callbacks[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE];

/**
 * The port size for STM32F0 is 16. We need to remember which port is
 * associated to which exti line. This where we do it.
 */
static mal_hspec_gpio_port_e exti_ports[16];

mal_error_e mal_hspec_stm32f0_gpio_init(mal_hspec_gpio_init_s *gpio_init) {
	// Enable clock domain
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(gpio_init->gpio.port), ENABLE);
	// Set GPIO
	GPIO_InitTypeDef init;
	if (MAL_HSPEC_GPIO_DIR_IN == gpio_init->direction) {
		init.GPIO_Mode = GPIO_Mode_IN;
	} else {
		init.GPIO_Mode = GPIO_Mode_OUT;
		if (MAL_HSPEC_GPIO_OUT_PP == gpio_init->output_config) {
			init.GPIO_OType = GPIO_OType_PP;
		} else {
			init.GPIO_OType = GPIO_OType_OD;
		}
	}
	init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(gpio_init->gpio.pin);
	switch (gpio_init->pupd) {
	case MAL_HSPEC_GPIO_PUPD_PU:
		init.GPIO_PuPd = GPIO_PuPd_UP;
		break;
	case MAL_HSPEC_GPIO_PUPD_PD:
		init.GPIO_PuPd = GPIO_PuPd_DOWN;
		break;
	case MAL_HSPEC_GPIO_PUPD_NONE:
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

mal_error_e mal_hspec_stm32f0_set_gpio(const mal_hspec_gpio_s *gpio, bool value) {
	BitAction action = Bit_RESET;

	if (value) {
		action = Bit_SET;
	}
	GPIO_WriteBit(mal_hspec_stm32f0_get_gpio_typedef(gpio->port), MAL_HSPEC_STM32F0_GET_GPIO_PIN(gpio->pin),
			action);

	return MAL_ERROR_OK;
}

bool mal_hspec_stm32f0_get_gpio(const mal_hspec_gpio_s *gpio) {
	uint8_t result = GPIO_ReadInputDataBit(mal_hspec_stm32f0_get_gpio_typedef(gpio->port),
			MAL_HSPEC_STM32F0_GET_GPIO_PIN(gpio->pin));
	if (Bit_SET == result) {
		return true;
	}
	return false;
}

mal_error_e mal_hspec_stm32f0_toggle_gpio(const mal_hspec_gpio_s *gpio) {
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
	mal_hspec_stm32f0_gpio_event_disable_interrupt(init->gpio);
	// Configure EXTI source
	SYSCFG_EXTILineConfig(get_exti_port_source(init->gpio->port), init->gpio->pin);
	// Save callback
	gpio_event_callbacks[init->gpio->pin] = init->callback;
	// Save port
	exti_ports[init->gpio->pin] = init->gpio->port;
	// Configure EXTI
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = (1 << init->gpio->pin);
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	if (MAL_HSPEC_GPIO_EVENT_FALLING == init->event) {
		exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
	} else if (MAL_HSPEC_GPIO_EVENT_RISING == init->event) {
		exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
	} else {
		exti_init.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	}
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);
	// Enable interrupt
	mal_hspec_stm32f0_gpio_event_enable_interrupt(init->gpio, true);

	return MAL_ERROR_OK;
}

IRQn_Type mal_hspec_stm32f0_gpio_get_exti_irq(uint8_t pin) {
	if (pin >= 0 && pin <= 1) {
		return EXTI0_1_IRQn;
	} else if (pin >= 2 && pin <= 3) {
		return EXTI2_3_IRQn;
	} else {
		return EXTI4_15_IRQn;
	}
}

static uint8_t get_exti_port_source(mal_hspec_gpio_port_e port) {
	switch (port) {
		case MAL_GPIO_PORT_A:
			return EXTI_PortSourceGPIOA;
		case MAL_GPIO_PORT_B:
			return EXTI_PortSourceGPIOB;
		case MAL_GPIO_PORT_C:
			return EXTI_PortSourceGPIOC;
		case MAL_GPIO_PORT_D:
			return EXTI_PortSourceGPIOD;
		case MAL_GPIO_PORT_E:
			return EXTI_PortSourceGPIOE;
		default:
			return EXTI_PortSourceGPIOF;
	}
}

void EXTI0_1_IRQHandler(void) {
	handle_exti_interrupt(EXTI_Line0, 0);
	handle_exti_interrupt(EXTI_Line1, 1);
}

void EXTI2_3_IRQHandler(void) {
	handle_exti_interrupt(EXTI_Line2, 2);
	handle_exti_interrupt(EXTI_Line3, 3);
}

void EXTI4_15_IRQHandler(void) {
	handle_exti_interrupt(EXTI_Line4, 4);
	handle_exti_interrupt(EXTI_Line5, 5);
	handle_exti_interrupt(EXTI_Line6, 6);
	handle_exti_interrupt(EXTI_Line7, 7);
	handle_exti_interrupt(EXTI_Line8, 8);
	handle_exti_interrupt(EXTI_Line9, 9);
	handle_exti_interrupt(EXTI_Line10, 10);
	handle_exti_interrupt(EXTI_Line11, 11);
	handle_exti_interrupt(EXTI_Line12, 12);
	handle_exti_interrupt(EXTI_Line13, 13);
	handle_exti_interrupt(EXTI_Line14, 14);
	handle_exti_interrupt(EXTI_Line15, 15);
}

static void handle_exti_interrupt(uint32_t exti_line, uint8_t pin) {
	mal_hspec_gpio_s event_gpio;
	event_gpio.pin = pin;
	event_gpio.port = exti_ports[pin];
	if (EXTI_GetITStatus(exti_line) != RESET) {
		EXTI_ClearITPendingBit(exti_line);
		if (gpio_event_callbacks[pin] != NULL) {
			gpio_event_callbacks[pin](&event_gpio);
		}
	}
}

mal_error_e mal_hspec_stm32f0_gpio_event_remove(const mal_hspec_gpio_s *gpio) {
	// Disable interrupt
	bool active = mal_hspec_stm32f0_gpio_event_disable_interrupt(gpio);
	// Remove callback
	gpio_event_callbacks[gpio->pin] = NULL;
	// Disable EXTI
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = (1 << gpio->pin);
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init.EXTI_LineCmd = DISABLE;
	EXTI_Init(&exti_init);
	// Enable interrupt
	mal_hspec_stm32f0_gpio_event_enable_interrupt(gpio, active);

	return MAL_ERROR_OK;
}

bool mal_hspec_stm32f0_gpio_event_disable_interrupt(const mal_hspec_gpio_s *gpio) {
	IRQn_Type irq = mal_hspec_stm32f0_gpio_get_exti_irq((gpio)->pin);
	bool active = NVIC_GetActive(irq);
	NVIC_DisableIRQ(irq);
	__DSB();
	__ISB();
	return active;
}
