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

#include "mal_hspec_stm32f7_serial.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "mal_hspec_stm32f7_gpio.h"

static mal_error_e mal_hspec_stm32f7_serial_init_io(mal_serial_port_e port, mal_gpio_port_e gpio_port, uint8_t pin);

static mal_serial_s *mal_hspec_stm32f7_serial_1;
static mal_serial_s *mal_hspec_stm32f7_serial_2;
static mal_serial_s *mal_hspec_stm32f7_serial_3;
static mal_serial_s *mal_hspec_stm32f7_serial_4;
static mal_serial_s *mal_hspec_stm32f7_serial_5;
static mal_serial_s *mal_hspec_stm32f7_serial_6;
static mal_serial_s *mal_hspec_stm32f7_serial_7;
static mal_serial_s *mal_hspec_stm32f7_serial_8;

mal_error_e mal_serial_init(mal_serial_s *handle, mal_serial_init_s *init) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    RCC_PeriphCLKInitTypeDef pclk_init;
    // Initialize serial clock and set handles
    switch (init->port) {
        case MAL_SERIAL_PORT_1:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_USART1;
            pclk_init.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_USART1_CLK_ENABLE();
            mal_hspec_stm32f7_serial_1 = handle;
            handle->hal_serial_handle.Instance = USART1;
            break;
        case MAL_SERIAL_PORT_2:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_USART2;
            pclk_init.Usart1ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_USART2_CLK_ENABLE();
            mal_hspec_stm32f7_serial_2 = handle;
            handle->hal_serial_handle.Instance = USART2;
            break;
        case MAL_SERIAL_PORT_3:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_USART3;
            pclk_init.Usart1ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_USART3_CLK_ENABLE();
            mal_hspec_stm32f7_serial_3 = handle;
            handle->hal_serial_handle.Instance = USART3;
            break;
        case MAL_SERIAL_PORT_4:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_UART4;
            pclk_init.Usart1ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_UART4_CLK_ENABLE();
            mal_hspec_stm32f7_serial_4 = handle;
            handle->hal_serial_handle.Instance = UART4;
            break;
        case MAL_SERIAL_PORT_5:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_UART5;
            pclk_init.Usart1ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_UART5_CLK_ENABLE();
            mal_hspec_stm32f7_serial_5 = handle;
            handle->hal_serial_handle.Instance = UART5;
            break;
        case MAL_SERIAL_PORT_6:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_USART6;
            pclk_init.Usart1ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_USART6_CLK_ENABLE();
            mal_hspec_stm32f7_serial_6 = handle;
            handle->hal_serial_handle.Instance = USART6;
            break;
        case MAL_SERIAL_PORT_7:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_UART7;
            pclk_init.Usart1ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_UART7_CLK_ENABLE();
            mal_hspec_stm32f7_serial_7 = handle;
            handle->hal_serial_handle.Instance = UART7;
            break;
        case MAL_SERIAL_PORT_8:
            pclk_init.PeriphClockSelection = RCC_PERIPHCLK_UART8;
            pclk_init.Usart1ClockSelection = RCC_UART8CLKSOURCE_PCLK1;
            hal_result = HAL_RCCEx_PeriphCLKConfig(&pclk_init);
            if (HAL_OK != hal_result) {
                return MAL_ERROR_CLOCK_ERROR;
            }
            __HAL_RCC_UART8_CLK_ENABLE();
            mal_hspec_stm32f7_serial_8 = handle;
            handle->hal_serial_handle.Instance = UART8;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Initialize RX and TX GPIOs
    mal_result = mal_hspec_stm32f7_serial_init_io(init->port, init->tx_port, init->tx_pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    mal_result = mal_hspec_stm32f7_serial_init_io(init->port, init->rx_port, init->rx_pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Reset interface
    hal_result = HAL_UART_DeInit(&handle->hal_serial_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Initialize UART
    handle->hal_serial_handle.Init.BaudRate = (uint32_t)init->baudrate;
    switch (init->data_size) {
        case MAL_SERIAL_DATA_9_BITS:
            handle->hal_serial_handle.Init.WordLength = UART_WORDLENGTH_9B;
            break;
        case MAL_SERIAL_DATA_8_BITS:
            handle->hal_serial_handle.Init.WordLength = UART_WORDLENGTH_8B;
            break;
        case MAL_SERIAL_DATA_7_BITS:
            handle->hal_serial_handle.Init.WordLength = UART_WORDLENGTH_7B;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    switch (init->stop_bits) {
        case MAL_SERIAL_STOP_BITS_1:
            handle->hal_serial_handle.Init.StopBits = UART_STOPBITS_1;
            break;
        case MAL_SERIAL_STOP_BITS_2:
            handle->hal_serial_handle.Init.StopBits = UART_STOPBITS_2;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    switch (init->parity) {
        case MAL_SERIAL_PARITY_NONE:
            handle->hal_serial_handle.Init.Parity = UART_PARITY_NONE;
            break;
        case MAL_SERIAL_PARITY_EVEN:
            handle->hal_serial_handle.Init.Parity = UART_PARITY_EVEN;
            break;
        case MAL_SERIAL_PARITY_ODD:
            handle->hal_serial_handle.Init.Parity = UART_PARITY_ODD;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    handle->hal_serial_handle.Init.Mode = UART_MODE_TX_RX;
    handle->hal_serial_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    handle->hal_serial_handle.Init.OverSampling = UART_OVERSAMPLING_16;
    handle->hal_serial_handle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hal_result = HAL_UART_Init(&handle->hal_serial_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_INIT_FAILED;
    }
    __HAL_UART_DISABLE(&handle->hal_serial_handle);

    __HAL_LINKDMA(huart, hdmatx, hdma_tx);

    handle->hal_dma.Instance = ;
    handle->hal_dma.Init.Channel =
    HAL_DMA_Init()

    // Enable UART
    __HAL_UART_ENABLE(&handle->hal_serial_handle);
}

static mal_error_e mal_hspec_stm32f7_serial_init_io(mal_serial_port_e port, mal_gpio_port_e gpio_port, uint8_t pin) {
    mal_error_e mal_result;
    uint32_t alternate;
    mal_result = mal_hspec_stm32f7_serial_get_alternate(port, gpio_port, pin, &alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    mal_result = mal_hspec_stm32f7_gpio_init_alternate(gpio_port, pin, alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }

    return MAL_ERROR_OK;
}
