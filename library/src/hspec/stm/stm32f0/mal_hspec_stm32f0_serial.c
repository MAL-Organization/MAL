/*
 * mal_hspec_stm32f0_serial.c
 *
 *  Created on: Oct 18, 2017
 *      Author: Olivier
 */
/*
 * Copyright (c) 2017 Olivier Allaire
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

#include "mal_hspec_stm32f0_serial.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "stm32f0/stm32f0xx_usart.h"
#include "stm32f0/stm32f0xx_dma.h"
#include "hspec/stm/stm32f0/mal_hspec_stm32f0_dma.h"
#include "hspec/stm/stm32f0/mal_hspec_stm32f0_nvic.h"
#include "std/mal_defs.h"

static void mal_hspec_stm32f0_serial_interrupt(mal_serial_s *port);
static mal_error_e mal_hspec_stm32f0_serial_save_handle(mal_serial_port_e port, mal_serial_s *handle);
static IRQn_Type mal_hspec_stm32f0_serial_get_irq(mal_serial_port_e port);
static void mal_hspec_stm32f0_serial_dma_callback(void *handle);
static void mal_hspec_stm32f0_serial_handle_rx_dma(mal_serial_s *handle);

static mal_serial_s *port_usart1;
static mal_serial_s *port_usart2;
static mal_serial_s *port_usart3;
static mal_serial_s *port_usart4;

mal_error_e mal_serial_init(mal_serial_s *handle, mal_serial_init_s *init) {
    mal_error_e result;
    // Enable GPIO clocks
    RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->tx_port), ENABLE);
    RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->rx_port), ENABLE);
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
    result = mal_hspec_stm32f0_get_pin_af(init->tx_port, init->tx_pin, tx_af, &function);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->tx_port), init->tx_pin, function);

    result = mal_hspec_stm32f0_get_pin_af(init->rx_port, init->rx_pin, rx_af, &function);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->rx_port), init->rx_pin, function);
    // Configure GPIOs
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->tx_pin);
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->tx_port), &gpio_init);

    gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->rx_pin);
    GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->rx_port), &gpio_init);
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
    usart_init.USART_BaudRate = (uint32_t)init->baudrate;
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
    // Initialize port
    result = mal_hspec_stm32f0_serial_save_handle(init->port, handle);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    handle->port = init->port;
    handle->usart_typedef = usart_typedef;
    handle->tx_callback = init->tx_callback;
    handle->tx_callback_handle = init->tx_callback_handle;
    handle->rx_callback = init->rx_callback;
    handle->rx_callback_handle = init->rx_callback_handle;
    handle->active = false;
    handle->error = false;
    handle->nvic_mask = 0;
    // Try to reserve DMA channels
    result = mal_hspec_stm32f0_dma_get_serial_channel(handle->port,
                                                      &handle->tx_dma_channel,
                                                      &handle->rx_dma_channel);
    // Prefectch IRQ since it is common to both
    IRQn_Type uart_irq = mal_hspec_stm32f0_serial_get_irq(handle->port);
    handle->nvic_mask = mal_hspec_stm32f0_nvic_add_irq(uart_irq, handle->nvic_mask);
    if (MAL_ERROR_OK != result) {
        // Could not get proper DMA channels. Free reserved channels
        mal_hspec_stm32f0_dma_free_channel(handle->tx_dma_channel);
        mal_hspec_stm32f0_dma_free_channel(handle->rx_dma_channel);
        // Set interrupt mode
        handle->dma_mode = false;
        // Enable interrupts
        NVIC_EnableIRQ(uart_irq);
        USART_ITConfig(handle->usart_typedef, USART_IT_RXNE, ENABLE);
    } else {
        // Initialize DMA. See https://stm32f4-discovery.net/2017/07/stm32-tutorial-efficiently-receive-uart-data-using-dma/.
        // Enable DMA clocks
        mal_hspec_stm32f0_dma_enable_clock(handle->tx_dma_channel);
        mal_hspec_stm32f0_dma_enable_clock(handle->rx_dma_channel);
        // Remap DMA channels
        mal_hspec_stm32f0_dma_remap_serial(handle->tx_dma_channel, handle->port);
        mal_hspec_stm32f0_dma_remap_serial(handle->rx_dma_channel, handle->port);
        // Common parameters for RX and TX
        DMA_InitTypeDef dma_init;
        DMA_StructInit(&dma_init);
        dma_init.DMA_BufferSize = MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE;
        dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init.DMA_Mode = DMA_Mode_Normal;
        dma_init.DMA_M2M = DMA_M2M_Disable;
        // Configure TX channel
        dma_init.DMA_MemoryBaseAddr = (uint32_t)handle->tx_buffer;
        dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
        dma_init.DMA_Priority = DMA_Priority_Low;
        dma_init.DMA_PeripheralBaseAddr = (uint32_t)&usart_typedef->TDR;
        DMA_Init(handle->tx_dma_channel, &dma_init);
        // Configure RX channel
        handle->using_rx_buffer_1 = true;
        dma_init.DMA_MemoryBaseAddr = (uint32_t)handle->rx_buffer_1;
        dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
        dma_init.DMA_Priority = DMA_Priority_High;
        dma_init.DMA_PeripheralBaseAddr = (uint32_t)&usart_typedef->RDR;
        DMA_Init(handle->rx_dma_channel, &dma_init);
        // Set mode and dma flags
        handle->dma_mode = true;
        handle->tx_dma_flag = mal_hspec_stm32f0_dma_get_flag(handle->tx_dma_channel);
        handle->rx_dma_flag = mal_hspec_stm32f0_dma_get_flag(handle->rx_dma_channel);
        // Fetch DMA Irqs
        IRQn_Type rx_irq = mal_hspec_stm32f0_dma_get_irq(handle->rx_dma_channel);
        handle->nvic_mask = mal_hspec_stm32f0_nvic_add_irq(rx_irq, handle->nvic_mask);
        handle->dma_tx_irq = mal_hspec_stm32f0_dma_get_irq(handle->tx_dma_channel);
        handle->nvic_mask = mal_hspec_stm32f0_nvic_add_irq(handle->dma_tx_irq, handle->nvic_mask);
        // Enable DMA RX interrupt
        mal_hspec_stm32f0_dma_set_callback(handle->tx_dma_channel,
                                           &mal_hspec_stm32f0_serial_dma_callback,
                                           handle);
        mal_hspec_stm32f0_dma_set_callback(handle->rx_dma_channel,
                                           &mal_hspec_stm32f0_serial_dma_callback,
                                           handle);
        NVIC_EnableIRQ(handle->dma_tx_irq);
        NVIC_EnableIRQ(rx_irq);
        DMA_ITConfig(handle->rx_dma_channel, DMA_IT_TC, ENABLE);
        NVIC_EnableIRQ(uart_irq);
        USART_ITConfig(handle->usart_typedef, USART_IT_IDLE, ENABLE);
        DMA_Cmd(handle->rx_dma_channel, ENABLE);
        // Enable DMA for UART
        USART_DMACmd(handle->usart_typedef, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
    }
    // Enable interface
    USART_Cmd(usart_typedef, ENABLE);

    return MAL_ERROR_OK;
}

void USART1_IRQHandler(void) {
    mal_hspec_stm32f0_serial_interrupt(port_usart1);
}

void USART2_IRQHandler(void) {
    mal_hspec_stm32f0_serial_interrupt(port_usart2);
}

void USART3_4_IRQHandler(void) {
    // Check USART3
    if (USART_GetITStatus(USART3, USART_IT_TXE) == SET ||
        USART_GetITStatus(USART3, USART_IT_RXNE) == SET ||
        USART_GetITStatus(USART3, USART_IT_IDLE) == SET ||
        (USART3->ISR & USART_ISR_ORE)) {
        mal_hspec_stm32f0_serial_interrupt(port_usart3);
    }
    // Check USART4
    if (USART_GetITStatus(USART4, USART_IT_TXE) == SET ||
        USART_GetITStatus(USART4, USART_IT_RXNE) == SET ||
        USART_GetITStatus(USART4, USART_IT_IDLE) == SET ||
        (USART4->ISR & USART_ISR_ORE)) {
        mal_hspec_stm32f0_serial_interrupt(port_usart4);
    }
}

static void mal_hspec_stm32f0_serial_interrupt(mal_serial_s *handle) {
    uint16_t data;
    mal_error_e result;
    // Check if transmit completed
    if (USART_GetITStatus(handle->usart_typedef, USART_IT_TXE) == SET) {
        // Execute callback
        result = handle->tx_callback(handle->tx_callback_handle, &data);
        // Send next word if given
        if (MAL_ERROR_OK == result) {
            USART_SendData(handle->usart_typedef, data);
        } else {
            // Disable tx interrupt
            USART_ITConfig(handle->usart_typedef, USART_IT_TXE, DISABLE);
            handle->active = false;
        }
    }
    // Check if receive completed
    if (USART_GetITStatus(handle->usart_typedef, USART_IT_RXNE) == SET) {
        // Read data
        data = USART_ReceiveData(handle->usart_typedef);
        // Execute callback
        handle->rx_callback(handle->rx_callback_handle, data);
    }
    // Check for errors
    if (handle->usart_typedef->ISR & USART_ISR_ORE) {
        handle->error = true;
        USART_ClearITPendingBit(handle->usart_typedef, USART_IT_ORE);
    }
    // Check for idle
    if (USART_GetITStatus(handle->usart_typedef, USART_IT_IDLE) == SET) {
        USART_ClearITPendingBit(handle->usart_typedef, USART_IT_IDLE);
        // Disable serial interrupts
        mal_serial_interrupt_s state;
        mal_serial_disable_interrupt(handle, &state);
        // Check for RX transfer complete
        mal_hspec_stm32f0_serial_handle_rx_dma(handle);
        // Restore interrupts
        mal_serial_enable_interrupt(handle, &state);
    }
}

mal_error_e mal_serial_transfer(mal_serial_s *handle, uint16_t data) {
    // Check if interface is active
    if (handle->active) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Disable interrupts
    mal_serial_interrupt_s state;
    mal_serial_disable_interrupt(handle, &state);
    if (handle->dma_mode) {
        // Fill data buffer
        mal_error_e result;
        uint8_t data_count = 0;
        handle->tx_buffer[data_count++] = data;
        do {
            result = handle->tx_callback(handle->tx_callback_handle, &data);
            if (MAL_ERROR_OK == result) {
                handle->tx_buffer[data_count++] = data;
            }
        } while ((data_count < MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE) &&
                 (MAL_ERROR_OK == result));
        // Start new transfer
        handle->tx_dma_channel->CNDTR = data_count;
        DMA_Cmd(handle->tx_dma_channel, ENABLE);
        // Enable interrupt
        DMA_ITConfig(handle->tx_dma_channel, DMA_IT_TC, ENABLE);
    } else {
        // Start transfer and enable interrupt
        USART_SendData(handle->usart_typedef, data);
        USART_ITConfig(handle->usart_typedef, USART_IT_TXE, ENABLE);
    }
    handle->active = true;
    mal_serial_enable_interrupt(handle, &state);

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f0_serial_save_handle(mal_serial_port_e port, mal_serial_s *handle) {
    switch (port) {
        case MAL_SERIAL_PORT_1:
            port_usart1 = handle;
            break;
        case MAL_SERIAL_PORT_2:
            port_usart2 = handle;
            break;
        case MAL_SERIAL_PORT_3:
            port_usart3 = handle;
            break;
        case MAL_SERIAL_PORT_4:
            port_usart4 = handle;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

static IRQn_Type mal_hspec_stm32f0_serial_get_irq(mal_serial_port_e port) {
    switch (port) {
            case MAL_SERIAL_PORT_1:
                return USART1_IRQn;
            case MAL_SERIAL_PORT_2:
                return USART2_IRQn;
            case MAL_SERIAL_PORT_3:
            case MAL_SERIAL_PORT_4:
            default:
                // 29 equates to USART3_4_IRQn. However, the name of the constant
                // changes based on the MCU because it is not available on all of
                // them. It is simpler to use the constant directly. If the MCU
                // does not support these ports, the code will not get here.
                return (IRQn_Type)29;
        }
}

MAL_DEFS_INLINE void mal_serial_disable_interrupt(mal_serial_s *handle, mal_serial_interrupt_s *state) {
    state->mask = mal_hspec_stm32f0_nvic_get_activity(handle->nvic_mask);
    mal_hspec_stm32f0_nvic_clear(handle->nvic_mask);
    __DSB();
    __ISB();
}

MAL_DEFS_INLINE void mal_serial_enable_interrupt(mal_serial_s *handle, mal_serial_interrupt_s *state) {
    MAL_DEFS_UNUSED(handle);
    mal_hspec_stm32f0_nvic_set(state->mask);
}

static void mal_hspec_stm32f0_serial_dma_callback(void *handle) {
    mal_serial_s *serial_handle = handle;
    // Disable serial interrupts
    mal_serial_interrupt_s state;
    mal_serial_disable_interrupt(serial_handle, &state);
    // Check for RX transfer complete
    if (SET == DMA_GetITStatus(serial_handle->rx_dma_flag)) {
        mal_hspec_stm32f0_serial_handle_rx_dma(serial_handle);
    }
    // Check for TX transfer complete
    if (SET == DMA_GetITStatus(serial_handle->tx_dma_flag)) {
        // Clear interrupt
        DMA_ClearITPendingBit(serial_handle->tx_dma_flag);
        // Disable channel to allow reconfiguration
        DMA_Cmd(serial_handle->tx_dma_channel, DISABLE);
        // Get new data to transfer
        mal_error_e result;
        uint16_t data;
        uint8_t data_count = 0;
        do {
            result = serial_handle->tx_callback(serial_handle->tx_callback_handle, &data);
            if (MAL_ERROR_OK == result) {
                serial_handle->tx_buffer[data_count++] = data;
            }
        } while ((data_count < MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE) &&
                 (MAL_ERROR_OK == result));
        if (data_count) {
            // Start new transfer
            serial_handle->tx_dma_channel->CNDTR = data_count;
            DMA_Cmd(serial_handle->tx_dma_channel, ENABLE);
        } else {
            // Set interface inactive
            NVIC_DisableIRQ(serial_handle->dma_tx_irq);
            DMA_ITConfig(serial_handle->tx_dma_channel, DMA_IT_TC, DISABLE);
            serial_handle->active = false;
        }
    }
    // Restore interrupts
    mal_serial_enable_interrupt(serial_handle, &state);
}

static void mal_hspec_stm32f0_serial_handle_rx_dma(mal_serial_s *handle) {
    // Save transfered data
    uint8_t data_count = (uint8_t)(MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE - handle->rx_dma_channel->CNDTR);
    // Clear interrupt
    DMA_ClearITPendingBit(handle->rx_dma_flag);
    // Disable channel to allow reconfiguration
    DMA_Cmd(handle->rx_dma_channel, DISABLE);
    // Start new transfer
    uint16_t *old_buffer;
    if (handle->using_rx_buffer_1) {
        handle->rx_dma_channel->CMAR = (uint32_t)handle->rx_buffer_2;
        old_buffer = handle->rx_buffer_1;
    } else {
        handle->rx_dma_channel->CMAR = (uint32_t)handle->rx_buffer_1;
        old_buffer = handle->rx_buffer_2;
    }
    handle->using_rx_buffer_1 = !handle->using_rx_buffer_1;
    handle->rx_dma_channel->CNDTR = MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE;
    DMA_Cmd(handle->rx_dma_channel, ENABLE);
    // Notify reception
    uint8_t index;
    for (index = 0; index < data_count; index++) {
        handle->rx_callback(handle->rx_callback_handle, old_buffer[index]);
    }
}
