/*
 * mal_hspec_stm32f0_serial.c
 *
 *  Created on: Oct 18, 2017
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015-2017 Olivier Allaire
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

#include "serial/mal_serial.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "stm32f0/stm32f0xx_usart.h"

mal_error_e mal_serial_init(mal_hspec_serial_init_s *init) {
    // Enable GPIO clocks
    RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->tx_gpio->port), ENABLE);
    RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->rx_gpio->port), ENABLE);
    // Enable serial clock
    switch (init->port) {
        case MAL_SERIAL_PORT_1:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
            break;
        case MAL_SERIAL_PORT_2:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
            break;
        case MAL_SERIAL_PORT_3:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
            break;
        case MAL_SERIAL_PORT_4:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART4, ENABLE);
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Configure alternate function
    mal_error_e result;
    uint8_t function;
    mal_hspec_stm32f0_af_e tx_af;
    mal_hspec_stm32f0_af_e rx_af;
    switch (init->port) {
        case MAL_SERIAL_PORT_1:
            tx_af = MAL_HSPEC_STM32F0_AF_USART1_TX;
            rx_af = MAL_HSPEC_STM32F0_AF_USART1_RX;
            break;
        case MAL_SERIAL_PORT_2:
            tx_af = MAL_HSPEC_STM32F0_AF_USART2_TX;
            rx_af = MAL_HSPEC_STM32F0_AF_USART2_RX;
            break;
        case MAL_SERIAL_PORT_3:
            tx_af = MAL_HSPEC_STM32F0_AF_USART3_TX;
            rx_af = MAL_HSPEC_STM32F0_AF_USART3_RX;
            break;
        case MAL_SERIAL_PORT_4:
            tx_af = MAL_HSPEC_STM32F0_AF_USART4_TX;
            rx_af = MAL_HSPEC_STM32F0_AF_USART4_RX;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    result = mal_hspec_stm32f0_get_pin_af(init->tx_gpio, tx_af, &function);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->tx_gpio->port), init->tx_gpio->pin, function);

    result = mal_hspec_stm32f0_get_pin_af(init->rx_gpio, rx_af, &function);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->rx_gpio->port), init->rx_gpio->pin, function);
    // Configure GPIOs
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->tx_gpio->pin);
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->tx_gpio->port), &gpio_init);

    gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->rx_gpio->pin);
    GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->rx_gpio->port), &gpio_init);
    // Get correct typedef
    USART_TypeDef *usart_typedef;
    switch (init->port) {
        case MAL_SERIAL_PORT_1:
            usart_typedef = USART1;
            break;
        case MAL_SERIAL_PORT_2:
            usart_typedef = USART2;
            break;
        case MAL_SERIAL_PORT_3:
            usart_typedef = USART3;
            break;
        case MAL_SERIAL_PORT_4:
            usart_typedef = USART4;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Reset interface
    USART_DeInit(usart_typedef);
    // Initialize interface
    USART_InitTypeDef usart_init;
    usart_init.USART_BaudRate = init->baudrate;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    switch (init->parity) {
        case MAL_SERIAL_PARITY_ODD:
            usart_init.USART_Parity = USART_Parity_Odd;
            break;
        case MAL_SERIAL_PARITY_EVEN:
            usart_init.USART_Parity = USART_Parity_Even;
            break;
        case MAL_SERIAL_PARITY_NONE:
        default:
            usart_init.USART_Parity = USART_Parity_No;
            break;
    }
    switch (init->stop_bits) {
        case MAL_SERIAL_STOP_BITS_2:
            usart_init.USART_StopBits = USART_StopBits_2;
            break;
        case MAL_SERIAL_STOP_BITS_1:
        default:
            usart_init.USART_StopBits = USART_StopBits_1;
            break;
    }
    switch (init->data_size) {
        case MAL_SERIAL_DATA_7_BITS:
            usart_init.USART_WordLength = USART_WordLength_7b;
            break;
        case MAL_SERIAL_DATA_9_BITS:
            usart_init.USART_WordLength = USART_WordLength_9b;
            break;
        case MAL_SERIAL_DATA_8_BITS:
        default:
            usart_init.USART_WordLength = USART_WordLength_8b;
            break;
    }
    USART_Init(usart_typedef, &usart_init);
    // Enable interrupts
    switch (init->port) {
        case MAL_SERIAL_PORT_1:
            NVIC_EnableIRQ(USART1_IRQn);
            break;
        case MAL_SERIAL_PORT_2:
            NVIC_EnableIRQ(USART2_IRQn);
            break;
        case MAL_SERIAL_PORT_3:
        case MAL_SERIAL_PORT_4:
            // 30 equates to USART3_4_IRQn. However, the name of the constant
            // changes based on the MCU because it is not available on all of
            // them. It is simpler to use the constant directly. If the MCU
            // does not support CAN, the code will not get here.
            NVIC_EnableIRQ(29);
            break;
        default:
            break;
    }
    // Enable interface
    USART_Cmd(usart_typedef, ENABLE);

    return MAL_ERROR_OK;
}
