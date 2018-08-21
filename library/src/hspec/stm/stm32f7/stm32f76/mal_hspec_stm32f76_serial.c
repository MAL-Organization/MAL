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

#include "hspec/stm/stm32f7/mal_hspec_stm32f7_serial.h"

mal_error_e mal_hspec_stm32f7_serial_get_alternate(mal_serial_port_e port, mal_gpio_port_e gpio_port, uint8_t pin,
                                                   uint32_t *alternate) {
    switch (port) {
        case MAL_SERIAL_PORT_1:
            if (MAL_GPIO_PORT_B == gpio_port && (14 == pin || 15 == pin)) {
                *alternate = GPIO_AF4_USART1;
            } else {
                *alternate = GPIO_AF7_USART1;
            }
            return MAL_ERROR_OK;
        case MAL_SERIAL_PORT_2:
            *alternate = GPIO_AF7_USART2;
            return MAL_ERROR_OK;
        case MAL_SERIAL_PORT_3:
            *alternate = GPIO_AF7_USART3;
            return MAL_ERROR_OK;
        case MAL_SERIAL_PORT_4:
            if (MAL_GPIO_PORT_A == gpio_port && (11 == pin || 12 == pin)) {
                *alternate = GPIO_AF6_UART4;
            } else {
                *alternate = GPIO_AF8_UART4;
            }
            return MAL_ERROR_OK;
        case MAL_SERIAL_PORT_5:
            if (MAL_GPIO_PORT_B == gpio_port && (5 == pin || 6 == pin)) {
                *alternate = GPIO_AF1_UART5;
            } else if ((MAL_GPIO_PORT_B == gpio_port && (8 == pin || 9 == pin)) ||
                       (MAL_GPIO_PORT_C == gpio_port && (8 == pin || 9 == pin))) {
                *alternate = GPIO_AF7_UART5;
            } else {
                *alternate = GPIO_AF8_UART5;
            }
            return MAL_ERROR_OK;
        case MAL_SERIAL_PORT_6:
            *alternate = GPIO_AF8_USART6;
            return MAL_ERROR_OK;
        case MAL_SERIAL_PORT_7:
            if ((MAL_GPIO_PORT_A == gpio_port && (8 == pin || 15 == pin)) ||
                (MAL_GPIO_PORT_B == gpio_port && (3 == pin || 4 == pin))) {
                *alternate = GPIO_AF12_UART7;
            } else {
                *alternate = GPIO_AF8_UART7;
            }
            return MAL_ERROR_OK;
        case MAL_SERIAL_PORT_8:
            *alternate = GPIO_AF8_UART8;
            return MAL_ERROR_OK;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}
