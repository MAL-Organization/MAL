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

#ifndef MAL_MAL_HSPEC_STM32F7_GPIO_H
#define MAL_MAL_HSPEC_STM32F7_GPIO_H

#include "std/mal_stdint.h"
#include "gpio/mal_gpio.h"
#include "stm32f7/stm32f7xx_hal_gpio.h"

#define MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(pin) ((uint32_t)1 << (pin))

typedef struct MAL_GPIO {
    GPIO_TypeDef *hal_port;
    uint32_t hal_pin;
} mal_gpio_s;

typedef struct MAL_GPIO_EVENT {

} mal_gpio_event_s;

typedef struct MAL_GPIO_INTERRUPT_STATE {
    bool active;
} mal_gpio_interrupt_state_s;

mal_error_e mal_hspec_stm32f7_gpio_enable_clock(mal_gpio_port_e port);

GPIO_TypeDef* mal_hspec_stm32f7_gpio_get_hal_port(mal_gpio_port_e port);

mal_error_e mal_hspec_stm32f7_gpio_init_alternate(mal_gpio_port_e port, uint8_t pin, uint32_t alternate);

#endif //MAL_MAL_HSPEC_STM32F7_GPIO_H
