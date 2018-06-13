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

#include "std/mal_stdlib.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx.h"
#include "stm32f0/stm32f0xx_gpio.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "gpio/mal_gpio.h"

static GPIOSpeed_TypeDef get_gpio_speed(uint64_t speed);
static uint8_t get_exti_port_source(mal_gpio_port_e port);
static void handle_exti_interrupt(uint32_t exti_line, uint8_t pin);
IRQn_Type mal_hspec_stm32f0_gpio_get_exti_irq(uint8_t pin);

static mal_gpio_event_s *gpio_events[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE];

mal_error_e mal_gpio_init(mal_gpio_init_s *init, mal_gpio_s *handle) {
	// Enable clock domain
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->port), ENABLE);
	handle->port_handle = mal_hspec_stm32f0_get_gpio_typedef(init->port);
	handle->gpio_pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->pin);
	handle->mal_port = init->port;
	handle->mal_pin = init->pin;
	// Set GPIO
	GPIO_InitTypeDef gpio_init;
	if (MAL_GPIO_DIRECTION_IN == init->direction) {
		gpio_init.GPIO_Mode = GPIO_Mode_IN;
	} else {
		gpio_init.GPIO_Mode = GPIO_Mode_OUT;
		if (MAL_GPIO_OUT_PP == init->output_config) {
			gpio_init.GPIO_OType = GPIO_OType_PP;
		} else {
			gpio_init.GPIO_OType = GPIO_OType_OD;
		}
	}
	gpio_init.GPIO_Pin = handle->gpio_pin;
	switch (init->pupd) {
	case MAL_GPIO_PUPD_PU:
		gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
		break;
	case MAL_GPIO_PUPD_PD:
		gpio_init.GPIO_PuPd = GPIO_PuPd_DOWN;
		break;
	case MAL_GPIO_PUPD_NONE:
	default:
		gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
		break;
	}
	gpio_init.GPIO_Speed = get_gpio_speed(init->speed);
	GPIO_Init(handle->port_handle, &gpio_init);

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

mal_error_e mal_gpio_set(mal_gpio_s *gpio, bool value) {
	BitAction action = Bit_RESET;

	if (value) {
		action = Bit_SET;
	}
	GPIO_WriteBit(gpio->port_handle, (uint16_t)gpio->gpio_pin, action);

	return MAL_ERROR_OK;
}

mal_error_e mal_gpio_get(mal_gpio_s *gpio, bool *value) {
	uint8_t result = GPIO_ReadInputDataBit(gpio->port_handle, (uint16_t)gpio->gpio_pin);
	if (Bit_SET == result) {
		*value = true;
	} else {
        *value = false;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_toggle(mal_gpio_s *gpio) {
	uint8_t state = GPIO_ReadOutputDataBit(gpio->port_handle, (uint16_t)gpio->gpio_pin);
	if (Bit_SET == state) {
	    mal_gpio_set(gpio, false);
	} else {
	    mal_gpio_set(gpio, true);
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_gpio_event_init(mal_gpio_event_init_s *init, mal_gpio_s *gpio_handle, mal_gpio_event_s *event_handle) {
    mal_gpio_init_s gpio_init;
    gpio_init.port = init->port;
    gpio_init.pin = init->pin;
    gpio_init.pupd = init->pupd;
    gpio_init.direction = MAL_GPIO_DIRECTION_IN;
    gpio_init.speed = init->speed;
	mal_error_e result = mal_gpio_init(&gpio_init, gpio_handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Enable syscfg clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// Initialize handle
    event_handle->port = init->port;
    event_handle->pin = init->pin;
    event_handle->irq = mal_hspec_stm32f0_gpio_get_exti_irq(init->pin);
    event_handle->callback = init->callback;
    event_handle->callback_handle = init->handle;
    gpio_events[init->pin] = event_handle;
	// Disable interrupt
    mal_gpio_interrupt_state_s state;
    mal_gpio_event_disable_interrupt(event_handle, &state);
	// Configure EXTI source
	SYSCFG_EXTILineConfig(get_exti_port_source(init->port), init->pin);
	// Configure EXTI
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = ((uint32_t)1 << init->pin);
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	if (MAL_GPIO_EVENT_FALLING == init->event) {
		exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
	} else if (MAL_GPIO_EVENT_RISING == init->event) {
		exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
	} else {
		exti_init.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	}
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);
	// Enable interrupt
    state.active = true;
	mal_gpio_event_restore_interrupt(event_handle, &state);

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

static uint8_t get_exti_port_source(mal_gpio_port_e port) {
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
	mal_gpio_event_s *event = gpio_events[pin];
	if (EXTI_GetITStatus(exti_line) != RESET) {
		EXTI_ClearITPendingBit(exti_line);
		if (NULL != event) {
            event->callback(event->callback_handle);
		}
	}
}

mal_error_e mal_gpio_event_remove(mal_gpio_event_s *handle) {
	// Disable interrupt
    mal_gpio_interrupt_state_s state;
	mal_gpio_event_disable_interrupt(handle, &state);
	// Remove event
    gpio_events[handle->pin] = NULL;
	// Disable EXTI
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = ((uint32_t)1 << handle->pin);
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init.EXTI_LineCmd = DISABLE;
	EXTI_Init(&exti_init);
	// Enable interrupt
	mal_gpio_event_restore_interrupt(handle, &state);

	return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_gpio_event_disable_interrupt(mal_gpio_event_s *handle, mal_gpio_interrupt_state_s *state) {
	state->active = NVIC_GetActive(handle->irq);
	NVIC_DisableIRQ(handle->irq);
	__DSB();
	__ISB();
}

MAL_DEFS_INLINE void mal_gpio_event_restore_interrupt(mal_gpio_event_s *handle, mal_gpio_interrupt_state_s *state) {
	if (state->active) {
		NVIC_EnableIRQ(handle->irq);
	}
}

mal_error_e mal_gpio_deinit(mal_gpio_s *gpio) {
	// Set IO to input
	mal_gpio_init_s gpio_init;
	gpio_init.port = gpio->mal_port;
	gpio_init.pin = gpio->mal_pin;
	gpio_init.direction = MAL_GPIO_DIRECTION_IN;
	gpio_init.pupd = MAL_GPIO_PUPD_NONE;
	gpio_init.speed = UINT64_MAX;
	return mal_gpio_init(&gpio_init, gpio);
}
