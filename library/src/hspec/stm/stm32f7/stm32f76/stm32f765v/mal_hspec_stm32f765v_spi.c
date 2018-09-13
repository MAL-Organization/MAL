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

#include "hspec/stm/stm32f7/mal_hspec_stm32f7_spi.h"

mal_error_e mal_hspec_stm32f7_spi_get_alternate(mal_spi_e interface, mal_gpio_port_e gpio_port, uint8_t pin,
                                                uint32_t *alternate) {
    switch (interface) {
        case MAL_SPI_1:
            *alternate = GPIO_AF5_SPI1;
            return MAL_ERROR_OK;
        case MAL_SPI_2:
            if (MAL_GPIO_PORT_B == gpio_port && 4 == pin) {
                *alternate = GPIO_AF7_SPI2;
            } else {
                *alternate = GPIO_AF5_SPI2;
            }
            return MAL_ERROR_OK;
        case MAL_SPI_3:
            if (MAL_GPIO_PORT_B == gpio_port && 2 == pin) {
                *alternate = GPIO_AF7_SPI3;
            } else if (MAL_GPIO_PORT_D == gpio_port && 6 == pin) {
                *alternate = GPIO_AF5_SPI3;
            } else {
                *alternate = GPIO_AF6_SPI3;
            }
            return MAL_ERROR_OK;
        case MAL_SPI_4:
            *alternate = GPIO_AF5_SPI4;
            return MAL_ERROR_OK;
        case MAL_SPI_6:
            if (MAL_GPIO_PORT_A == gpio_port && 15 == pin) {
                *alternate = GPIO_AF7_SPI6;
            } else if (MAL_GPIO_PORT_G == gpio_port && (8 == pin || 12 == pin || 13 == pin || 14 == pin)) {
                *alternate = GPIO_AF5_SPI6;
            } else {
                *alternate = GPIO_AF8_SPI6;
            }
            return MAL_ERROR_OK;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}
