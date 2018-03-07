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

#include "serial/mal_serial.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "stm32f0/stm32f0xx_usart.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx_dma.h"
#include "hspec/stm/stm32f0/mal_hspec_stm32f0_dma.h"

#define MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE    8

typedef struct {
    bool active;
    bool error;
    USART_TypeDef *usart_typedef;
    uint16_t tx_buffer[MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE];
    uint16_t rx_buffer[MAL_HSPEC_STM32F0_SERIAL_DMA_BUFFER_SIZE];
    DMA_Channel_TypeDef *tx_dma_channel;
    DMA_Channel_TypeDef *rx_dma_channel;
    uint32_t tx_dma_flag;
    uint32_t rx_dma_flag;
    bool dma_mode;
} mal_hspec_stm32f0_serial_port_s;

typedef struct MAL_SERIAL_INTERRUPT {
    bool dma_state;
    bool serial_state;
} mal_serial_interrupt_s;

static void mal_hspec_stm32f0_serial_interrupt(mal_serial_port_s *port);
static mal_error_e mal_hspec_stm32f0_serial_get_and_save_port(mal_serial_port_e port,
                                                              mal_hspec_stm32f0_serial_port_s **local_port,
                                                              mal_serial_port_s *handle);
static IRQn_Type mal_hspec_stm32f0_serial_get_irq(mal_serial_port_e port);
static void mal_hspec_stm32f0_serial_dma_callback(void *handle);

static mal_hspec_stm32f0_serial_port_s impl_port_usart1;
static mal_hspec_stm32f0_serial_port_s impl_port_usart2;
static mal_hspec_stm32f0_serial_port_s impl_port_usart3;
static mal_hspec_stm32f0_serial_port_s impl_port_usart4;

static mal_serial_port_s *port_usart1;
static mal_serial_port_s *port_usart2;
static mal_serial_port_s *port_usart3;
static mal_serial_port_s *port_usart4;

mal_error_e mal_serial_init(mal_serial_port_s *handle, mal_serial_init_s *init) {
    mal_error_e result;
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
    // Initialize port
    mal_hspec_stm32f0_serial_port_s *serial_port;
    result = mal_hspec_stm32f0_serial_get_and_save_port(init->port, &serial_port, handle);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    handle->port = init->port;
    serial_port->usart_typedef = usart_typedef;
    handle->tx_callback = init->tx_callback;
    handle->rx_callback = init->rx_callback;
    serial_port->active = false;
    serial_port->error = false;
    handle->impl = &serial_port;
    // Try to reserve DMA channels
    result = mal_hspec_stm32f0_dma_get_serial_channel(handle->port,
                                                      &serial_port->tx_dma_channel,
                                                      &serial_port->rx_dma_channel);
    // Prefectch IRQ since it is common to both
    IRQn_Type uart_irq = mal_hspec_stm32f0_serial_get_irq(handle->port);
    if (MAL_ERROR_OK != result) {
        // Could not get proper DMA channels. Free reserved channels
        mal_hspec_stm32f0_dma_free_channel(serial_port->tx_dma_channel);
        mal_hspec_stm32f0_dma_free_channel(serial_port->rx_dma_channel);
        // Set interrupt mode
        serial_port->dma_mode = false;
        // Enable interrupts
        NVIC_EnableIRQ(uart_irq);
        USART_ITConfig(serial_port->usart_typedef, USART_IT_RXNE, ENABLE);
    } else {
        // Initialize DMA. See https://stm32f4-discovery.net/2017/07/stm32-tutorial-efficiently-receive-uart-data-using-dma/.
        // Remap DMA channels
        mal_hspec_stm32f0_dma_remap_serial(serial_port->tx_dma_channel, handle->port);
        mal_hspec_stm32f0_dma_remap_serial(serial_port->rx_dma_channel, handle->port);
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
        dma_init.DMA_MemoryBaseAddr = (uint32_t)serial_port->tx_buffer;
        dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
        dma_init.DMA_Priority = DMA_Priority_Low;
        dma_init.DMA_PeripheralBaseAddr = (uint32_t)&usart_typedef->TDR;
        DMA_Init(serial_port->tx_dma_channel, &dma_init);
        // Configure RX channel
        dma_init.DMA_MemoryBaseAddr = (uint32_t)serial_port->rx_buffer;
        dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
        dma_init.DMA_Priority = DMA_Priority_Medium;
        dma_init.DMA_PeripheralBaseAddr = (uint32_t)&usart_typedef->RDR;
        DMA_Init(serial_port->rx_dma_channel, &dma_init);
        // Set mode and dma flags
        serial_port->dma_mode = true;
        serial_port->tx_dma_flag = mal_hspec_stm32f0_dma_get_flag(serial_port->tx_dma_channel);
        serial_port->rx_dma_flag = mal_hspec_stm32f0_dma_get_flag(serial_port->rx_dma_channel);
        // Enable DMA RX interrupt
        mal_hspec_stm32f0_dma_set_callback(serial_port->tx_dma_channel,
                                           &mal_hspec_stm32f0_serial_dma_callback,
                                           handle);
        mal_hspec_stm32f0_dma_set_callback(serial_port->rx_dma_channel,
                                           &mal_hspec_stm32f0_serial_dma_callback,
                                           handle);
        NVIC_EnableIRQ(mal_hspec_stm32f0_dma_get_irq(serial_port->rx_dma_channel));
        NVIC_EnableIRQ(uart_irq);
        USART_ITConfig(serial_port->usart_typedef, USART_IT_IDLE, ENABLE);
        DMA_Cmd(serial_port->rx_dma_channel, ENABLE);
        // Enable DMA for UART
        USART_DMACmd(serial_port->usart_typedef, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
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
        (USART3->ISR & USART_ISR_ORE)) {
        mal_hspec_stm32f0_serial_interrupt(port_usart3);
    }
    // Check USART4
    if (USART_GetITStatus(USART4, USART_IT_TXE) == SET ||
        USART_GetITStatus(USART4, USART_IT_RXNE) == SET ||
        (USART4->ISR & USART_ISR_ORE)) {
        mal_hspec_stm32f0_serial_interrupt(port_usart4);
    }
}

static void mal_hspec_stm32f0_serial_interrupt(mal_serial_port_s *port) {
    uint16_t data;
    mal_error_e result;
    mal_hspec_stm32f0_serial_port_s *serial_port = port->impl;
    // Check if transmit completed
    if (USART_GetITStatus(serial_port->usart_typedef, USART_IT_TXE) == SET) {
        // Execute callback
        result = port->tx_callback(port->port, &data);
        // Send next word if given
        if (MAL_ERROR_OK == result) {
            USART_SendData(serial_port->usart_typedef, data);
        } else {
            // Disable tx interrupt
            USART_ITConfig(serial_port->usart_typedef, USART_IT_TXE, DISABLE);
            serial_port->active = false;
        }
    }
    // Check if receive completed
    if (USART_GetITStatus(serial_port->usart_typedef, USART_IT_RXNE) == SET) {
        // Read data
        data = USART_ReceiveData(serial_port->usart_typedef);
        // Execute callback
        port->rx_callback(port->port, data);
    }
    // Check for errors
    if (serial_port->usart_typedef->ISR & USART_ISR_ORE) {
        serial_port->error = true;
        USART_ClearITPendingBit(serial_port->usart_typedef, USART_IT_ORE);
    }
}

mal_error_e mal_serial_transfer(mal_serial_port_s *handle, uint16_t data) {
    mal_serial_port_s *serial_handle = handle;
    mal_hspec_stm32f0_serial_port_s *serial_port = serial_handle->impl;
    // Check if interface is active
    if (serial_port->active) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Disable interrupts
    mal_serial_interrupt_s state;
    mal_serial_disable_interrupt(handle, &state);
    // Start transfer and enable interrupt
    USART_SendData(serial_port->usart_typedef, data);
    USART_ITConfig(serial_port->usart_typedef, USART_IT_TXE, ENABLE);
    serial_port->active = true;
    mal_serial_enable_interrupt(handle, &state);

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f0_serial_get_and_save_port(mal_serial_port_e port,
                                                              mal_hspec_stm32f0_serial_port_s **local_port,
                                                              mal_serial_port_s *handle) {
    switch (port) {
        case MAL_SERIAL_PORT_1:
            *local_port = &impl_port_usart1;
            port_usart1 = handle;
            break;
        case MAL_SERIAL_PORT_2:
            *local_port = &impl_port_usart2;
            port_usart2 = handle;
            break;
        case MAL_SERIAL_PORT_3:
            *local_port = &impl_port_usart3;
            port_usart3 = handle;
            break;
        case MAL_SERIAL_PORT_4:
            *local_port = &impl_port_usart4;
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
                return 29;
        }
}

MAL_DEFS_INLINE void mal_serial_disable_interrupt(mal_serial_port_s *handle, mal_serial_interrupt_s *state) {
    IRQn_Type irq = mal_hspec_stm32f0_serial_get_irq(handle->port);
    state->serial_state = NVIC_GetActive(irq);
    NVIC_DisableIRQ(irq);
    __DSB();
    __ISB();
}

MAL_DEFS_INLINE void mal_serial_enable_interrupt(mal_serial_port_s *handle, mal_serial_interrupt_s *state) {
    if (state->serial_state) {
        IRQn_Type irq = mal_hspec_stm32f0_serial_get_irq(handle->port);
        NVIC_EnableIRQ(irq);
    }
}

static void mal_hspec_stm32f0_serial_dma_callback(void *handle) {
    mal_serial_port_s *serial_handle = handle;
    mal_hspec_stm32f0_serial_port_s *serial_port = serial_handle->impl;
    // Disable serial interrupts
    mal_serial_interrupt_s state;
    mal_serial_disable_interrupt(serial_handle, &state);
    // Check for RX transfer complete
    if (SET == DMA_GetITStatus(serial_port->rx_dma_flag)) {

    }
}
