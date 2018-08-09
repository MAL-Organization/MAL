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

#ifndef HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_SERIAL_H_
#define HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_SERIAL_H_

#include "stm32f7/stm32f7xx_hal_uart.h"
#include "serial/mal_serial.h"
#include "mal_hspec_stm32f7_dma_definitions.h"

#define MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE    8

typedef struct MAL_SERIAL {
    // Basic serial variables
    bool active;
    bool error;
    bool dma_mode;
    mal_serial_tx_callback_t tx_callback;
    void *tx_callback_handle;
    mal_serial_rx_callback_t rx_callback;
    void *rx_callback_handle;
    IRQn_Type uart_irq;
    // STM variables
    UART_HandleTypeDef hal_serial_handle;
    DMA_HandleTypeDef hal_tx_dma;
    DMA_HandleTypeDef hal_rx_dma;
    // DMA variables
    uint16_t tx_buffer[MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE];
    volatile uint16_t rx_buffer_1[MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE];
    volatile uint16_t rx_buffer_2[MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE];
    volatile bool using_rx_buffer_1;
    mal_hspec_stm32f7_dma_stream_s *tx_dma_stream;
    mal_hspec_stm32f7_dma_stream_s *rx_dma_stream;
    uint32_t tx_dma_flag;
    uint32_t rx_dma_flag;
    IRQn_Type dma_tx_irq;
} mal_serial_s;

typedef struct MAL_SERIAL_INTERRUPT_STATE {
    uint32_t uart_state;
    uint32_t dma_state;
} mal_serial_interrupt_state_s;

mal_error_e mal_hspec_stm32f7_serial_get_alternate(mal_serial_port_e port, mal_gpio_port_e gpio_port, uint8_t pin,
                                                   uint32_t *alternate);

#endif /* HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_SERIAL_H_ */
