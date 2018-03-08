/*
 * mal_hspec_stm32f0_serial.h
 *
 *  Created on: Mar 7, 2018
 *      Author: Olivier
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_SERIAL_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_SERIAL_H_

#include "serial/mal_serial.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx.h"
#include "std/mal_stdint.h"

#define MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE    8

typedef struct MAL_SERIAL {
    // Basic serial variables
    mal_serial_port_e port;
    mal_serial_rx_callback_t rx_callback;
    mal_serial_tx_callback_t tx_callback;
    // Interrupts statuses
    bool active;
    bool error;
    bool dma_mode;
    IRQn_Type dma_tx_irq;
    uint32_t nvic_mask;
    // STM registers
    USART_TypeDef *usart_typedef;
    // DMA variables
    uint16_t tx_buffer[MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE];
    uint16_t rx_buffer_1[MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE];
    uint16_t rx_buffer_2[MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE];
    volatile bool using_rx_buffer_1;
    DMA_Channel_TypeDef *tx_dma_channel;
    DMA_Channel_TypeDef *rx_dma_channel;
    uint32_t tx_dma_flag;
    uint32_t rx_dma_flag;
} mal_serial_s;

typedef struct MAL_SERIAL_INTERRUPT {
    uint32_t mask;
} mal_serial_interrupt_s;

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_SERIAL_H_ */
