/*
 * mal_hspec_stm32f7_gpio.c
 *
 *  Created on: May 7, 2018
 *      Author: olivi
 */
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

#include "gpio/mal_gpio.h"
#include "std/mal_stdint.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "stm32f7/stm32f7xx_hal_gpio.h"

#define MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(pin) (1 << pin)

static GPIO_TypeDef* mal_hspec_stm32f7_gpio_get_hal_port(mal_gpio_port_e port);

static GPIO_TypeDef* mal_hspec_stm32f7_gpio_get_hal_port(mal_gpio_port_e port) {
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
        case MAL_GPIO_PORT_G:
            return GPIOG;
        case MAL_GPIO_PORT_H:
        default:
            return GPIOH;
    }
}

mal_error_e mal_gpio_init(mal_gpio_init_s *gpio_init) {
    // Enable clock and select port
    switch (gpio_init->gpio.port) {
        case MAL_GPIO_PORT_A:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_B:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_C:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_D:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_E:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_F:
            __HAL_RCC_GPIOF_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_G:
            __HAL_RCC_GPIOG_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_H:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Get port
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(gpio_init->gpio.port);
    // Configure GPIO
    GPIO_InitTypeDef  hal_gpio_init;
    hal_gpio_init.Pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(gpio_init->gpio.pin);
    if (MAL_GPIO_DIR_IN == gpio_init->direction) {
        hal_gpio_init.Mode = GPIO_MODE_INPUT;
    } else {
        if (MAL_GPIO_OUT_PP == gpio_init->output_config) {
            hal_gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
        } else {
            hal_gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
        }
    }
    if (MAL_GPIO_PUPD_PU == gpio_init->pupd) {
        hal_gpio_init.Pull = GPIO_PULLUP;
    } else if (MAL_GPIO_PUPD_PD == gpio_init->pupd) {
        hal_gpio_init.Pull = GPIO_PULLDOWN;
    } else {
        hal_gpio_init.Pull = GPIO_NOPULL;
    }
    HAL_GPIO_Init(hal_port, &hal_gpio_init);

    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_set(const mal_gpio_s *gpio, bool value) {
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(gpio->port);
    uint32_t hal_pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(gpio->pin);
    HAL_GPIO_WritePin(hal_port, hal_pin, value);

    return MAL_ERROR_OK;
}

bool mal_gpio_get(const mal_gpio_s *gpio) {
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(gpio->port);
    uint32_t hal_pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(gpio->pin);
    GPIO_PinState hal_pin_state = HAL_GPIO_ReadPin(hal_port, hal_pin);
    return GPIO_PIN_SET == hal_pin_state;
}

mal_error_e mal_gpio_toggle(const mal_gpio_s *gpio) {
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(gpio->port);
    uint32_t hal_pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(gpio->pin);
    HAL_GPIO_TogglePin(hal_port, hal_pin);
    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_deinit(const mal_gpio_s *gpio) {
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(gpio->port);
    uint32_t hal_pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(gpio->pin);
    HAL_GPIO_DeInit(hal_port, hal_pin);
    return MAL_ERROR_OK;
}
