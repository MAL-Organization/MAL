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

#ifndef MAL_MAL_HSPEC_STM32F0_GPIO_H
#define MAL_MAL_HSPEC_STM32F0_GPIO_H

#include "stm32f0/stm32f0xx_gpio.h"
#include "stm32f0/stm32f0xx.h"
#include "gpio/mal_gpio.h"

typedef struct MAL_GPIO {
    GPIO_TypeDef *port_handle;
    uint32_t gpio_pin;
    mal_gpio_port_e mal_port;
    uint8_t mal_pin;
} mal_gpio_s;

typedef struct MAL_GPIO_EVENT {
    IRQn_Type irq;
    mal_gpio_event_callback_t callback;
    void *callback_handle;
    mal_gpio_port_e port;
    uint8_t pin;
} mal_gpio_event_s;

typedef struct MAL_GPIO_INTERRUPT_STATE {
    bool active;
} mal_gpio_interrupt_state_s;

#endif //MAL_MAL_HSPEC_STM32F0_GPIO_H
