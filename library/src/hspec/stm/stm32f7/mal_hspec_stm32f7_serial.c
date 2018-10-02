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
#include "mal_hspec_stm32f7_dma.h"

static mal_error_e mal_hspec_stm32f7_serial_init_io(mal_serial_port_e port, mal_gpio_port_e gpio_port, uint8_t pin);
static void mal_hspec_stm32f7_serial_handle_rx_dma(mal_serial_s *handle);
static void mal_hspec_stm32f7_serial_tx_dma_callback(void *handle);
static void mal_hspec_stm32f7_serial_rx_dma_callback(void *handle);
static IRQn_Type mal_hspec_stm32f7_serial_get_irq(mal_serial_port_e port);
static void mal_hspec_stm32f7_serial_interrupt(mal_serial_s *handle);

static mal_serial_s *mal_hspec_stm32f7_serial_1;
static mal_serial_s *mal_hspec_stm32f7_serial_2;
static mal_serial_s *mal_hspec_stm32f7_serial_3;
static mal_serial_s *mal_hspec_stm32f7_serial_4;
static mal_serial_s *mal_hspec_stm32f7_serial_5;
static mal_serial_s *mal_hspec_stm32f7_serial_6;
static mal_serial_s *mal_hspec_stm32f7_serial_7;
static mal_serial_s *mal_hspec_stm32f7_serial_8;

mal_error_e mal_serial_init(mal_serial_s *handle, mal_serial_init_s *init) {
    uint32_t i;
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
    // After the selection of the clock source, it appears that a delay is needed otherwise some USART initialisations
    // are corrupted. I am out of time to investigate this. I hate to leave a poor man's sleep here, but it works :(
    // - Olivier
    i = 1000000;
    while (i--!=0);
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
    handle->hal_serial_handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    hal_result = HAL_UART_Init(&handle->hal_serial_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_INIT_FAILED;
    }
    __HAL_UART_DISABLE(&handle->hal_serial_handle);
    // Initialize port
    handle->active = false;
    handle->error = false;
    handle->tx_callback = init->tx_callback;
    handle->tx_callback_handle = init->tx_callback_handle;
    handle->rx_callback = init->rx_callback;
    handle->rx_callback_handle = init->rx_callback_handle;
    // Get UART IRQ required for both DMA and interrupt mode
    handle->uart_irq = mal_hspec_stm32f7_serial_get_irq(init->port);
    // Try to reserve DMA channels
    mal_result = mal_hspec_stm32f7_dma_get_serial_stream(init->port, &handle->tx_dma_stream, &handle->rx_dma_stream);
    if (MAL_ERROR_OK != mal_result) {
        // Could not get proper DMA channels. Free reserved channels
        mal_hspec_stm32f7_dma_free_channel(handle->tx_dma_stream);
        mal_hspec_stm32f7_dma_free_channel(handle->rx_dma_stream);
        // Set interrupt mode
        handle->dma_mode = false;
        // Enable interrupts
        NVIC_EnableIRQ(handle->uart_irq);
        __HAL_UART_ENABLE_IT(&handle->hal_serial_handle, UART_IT_RXNE);
    } else {
        // Initialize DMA
        // Enable DMA clocks
        mal_hspec_stm32f7_dma_enable_clock(handle->tx_dma_stream);
        mal_hspec_stm32f7_dma_enable_clock(handle->rx_dma_stream);
        // Initialize TX
        handle->hal_tx_dma.Instance = handle->tx_dma_stream->hal_stream;
        handle->hal_tx_dma.Init.Channel = handle->tx_dma_stream->location->channel;
        handle->hal_tx_dma.Init.Direction = DMA_MEMORY_TO_PERIPH;
        handle->hal_tx_dma.Init.PeriphInc = DMA_PINC_DISABLE;
        handle->hal_tx_dma.Init.MemInc = DMA_MINC_ENABLE;
        handle->hal_tx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        handle->hal_tx_dma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        handle->hal_tx_dma.Init.Mode = DMA_NORMAL;
        handle->hal_tx_dma.Init.Priority = DMA_PRIORITY_LOW;
        handle->hal_tx_dma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        hal_result = HAL_DMA_Init(&handle->hal_tx_dma);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_INIT_FAILED;
        }
        // Initialize RX
        handle->hal_rx_dma.Instance = handle->rx_dma_stream->hal_stream;
        handle->hal_rx_dma.Init.Channel = handle->rx_dma_stream->location->channel;
        handle->hal_rx_dma.Init.Direction = DMA_PERIPH_TO_MEMORY;
        handle->hal_rx_dma.Init.PeriphInc = DMA_PINC_DISABLE;
        handle->hal_rx_dma.Init.MemInc = DMA_MINC_ENABLE;
        handle->hal_rx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        handle->hal_rx_dma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        handle->hal_rx_dma.Init.Mode = DMA_NORMAL;
        handle->hal_rx_dma.Init.Priority = DMA_PRIORITY_HIGH;
        handle->hal_rx_dma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        hal_result = HAL_DMA_Init(&handle->hal_rx_dma);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_INIT_FAILED;
        }
        // Fetch DMA Irqs and flags
        handle->tx_dma_flag = __HAL_DMA_GET_TC_FLAG_INDEX(&handle->hal_tx_dma);
        handle->rx_dma_flag = __HAL_DMA_GET_TC_FLAG_INDEX(&handle->hal_rx_dma);
        IRQn_Type rx_irq = mal_hspec_stm32f7_dma_get_irq(handle->rx_dma_stream);
        handle->dma_tx_irq = mal_hspec_stm32f7_dma_get_irq(handle->tx_dma_stream);
        // Set DMA callbacks
        mal_hspec_stm32f7_dma_set_callback(handle->tx_dma_stream,
                                           &mal_hspec_stm32f7_serial_tx_dma_callback,
                                           handle);
        mal_hspec_stm32f7_dma_set_callback(handle->rx_dma_stream,
                                           &mal_hspec_stm32f7_serial_rx_dma_callback,
                                           handle);
        NVIC_EnableIRQ(handle->dma_tx_irq);
        NVIC_EnableIRQ(rx_irq);
        handle->dma_mode = true;
        __HAL_UART_CLEAR_IT(&handle->hal_serial_handle, UART_CLEAR_IDLEF);
        __HAL_UART_ENABLE_IT(&handle->hal_serial_handle, UART_IT_IDLE);
        NVIC_EnableIRQ(handle->uart_irq);
        // Start reception
        handle->using_rx_buffer_1 = true;
        mal_hspec_stm32f7_dma_start(handle->rx_dma_stream,
                                    &handle->hal_rx_dma,
                                    (uint32_t)&handle->hal_serial_handle.Instance->RDR,
                                    (uint32_t)handle->rx_buffer_1,
                                    MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE);
        // Enable DMA for UART
        SET_BIT(handle->hal_serial_handle.Instance->CR3, USART_CR3_DMAT);
        SET_BIT(handle->hal_serial_handle.Instance->CR3, USART_CR3_DMAR);
    }
    // Enable UART
    __HAL_UART_ENABLE(&handle->hal_serial_handle);
    return MAL_ERROR_OK;
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

static void mal_hspec_stm32f7_serial_rx_dma_callback(void *handle) {
    mal_serial_s *serial_handle = handle;
    // Disable interrupt
    mal_serial_interrupt_state_s state;
    mal_serial_disable_interrupt(serial_handle, &state);
    // Handle interrupt
    mal_hspec_stm32f7_serial_handle_rx_dma(serial_handle);
    // Restore interrupts
    mal_serial_restore_interrupt(serial_handle, &state);
}

static void mal_hspec_stm32f7_serial_tx_dma_callback(void *handle) {
    mal_serial_s *serial_handle = handle;
    // Disable serial interrupts
    mal_serial_interrupt_state_s state;
    mal_serial_disable_interrupt(serial_handle, &state);
    // Disable channel to allow reconfiguration
    HAL_DMA_Abort(&serial_handle->hal_tx_dma);
    // Get new data to transfer
    mal_error_e result;
    uint16_t data;
    uint8_t data_count = 0;
    do {
        result = serial_handle->tx_callback(serial_handle->tx_callback_handle, &data);
        if (MAL_ERROR_OK == result) {
            serial_handle->tx_buffer[data_count++] = data;
        }
    } while ((data_count < MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE) &&
             (MAL_ERROR_OK == result));
    if (data_count) {
        // Start new transfer
        mal_hspec_stm32f7_dma_start(serial_handle->tx_dma_stream,
                                    &serial_handle->hal_tx_dma,
                                    (uint32_t)serial_handle->tx_buffer,
                                    (uint32_t)&serial_handle->hal_serial_handle.Instance->TDR,
                                    data_count);
    } else {
        // Set interface inactive
        NVIC_DisableIRQ(serial_handle->dma_tx_irq);
        serial_handle->active = false;
    }
    // Restore interrupts
    mal_serial_restore_interrupt(serial_handle, &state);
}

static void mal_hspec_stm32f7_serial_handle_rx_dma(mal_serial_s *handle) {
    // Save transfered data
    uint32_t counter = __HAL_DMA_GET_COUNTER(&handle->hal_rx_dma);
    uint8_t data_count = (uint8_t)(MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE - counter);
    // Disable channel to allow reconfiguration
    HAL_DMA_Abort(&handle->hal_rx_dma);
    // Start new transfer
    volatile uint16_t *new_buffer;
    volatile uint16_t *old_buffer;
    if (handle->using_rx_buffer_1) {
        new_buffer = handle->rx_buffer_2;
        old_buffer = handle->rx_buffer_1;
    } else {
        new_buffer = handle->rx_buffer_1;
        old_buffer = handle->rx_buffer_2;
    }
    handle->using_rx_buffer_1 = !handle->using_rx_buffer_1;
    mal_hspec_stm32f7_dma_start(handle->rx_dma_stream,
                                &handle->hal_rx_dma,
                                (uint32_t)&handle->hal_serial_handle.Instance->RDR,
                                (uint32_t)new_buffer,
                                MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE);
    // Notify reception
    uint8_t index;
    for (index = 0; index < data_count; index++) {
        handle->rx_callback(handle->rx_callback_handle, old_buffer[index]);
    }
}

void USART1_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_1);
}

void USART2_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_2);
}

void USART3_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_3);
}

void UART4_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_4);
}

void UART5_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_5);
}

void USART6_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_6);
}

void UART7_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_7);
}

void UART8_IRQHandler(void) {
    mal_hspec_stm32f7_serial_interrupt(mal_hspec_stm32f7_serial_8);
}

static void mal_hspec_stm32f7_serial_interrupt(mal_serial_s *handle) {
    uint16_t data;
    mal_error_e result;
    // Check if transmit completed
    if (__HAL_UART_GET_FLAG(&handle->hal_serial_handle, UART_FLAG_TXE) && !handle->dma_mode) {
        // Execute callback
        result = handle->tx_callback(handle->tx_callback_handle, &data);
        // Send next word if given
        if (MAL_ERROR_OK == result) {
            handle->hal_serial_handle.Instance->TDR = data;
        } else {
            // Disable tx interrupt
            __HAL_UART_DISABLE_IT(&handle->hal_serial_handle, UART_IT_TXE);
            handle->active = false;
        }
    }
    // Check if receive completed
    if (__HAL_UART_GET_FLAG(&handle->hal_serial_handle, UART_FLAG_RXNE) && !handle->dma_mode) {
        // Read data
        data = (uint16_t)(handle->hal_serial_handle.Instance->RDR & 0x01FF);
        // Execute callback
        handle->rx_callback(handle->rx_callback_handle, data);
    }
    // Check for errors
    if (__HAL_UART_GET_FLAG(&handle->hal_serial_handle, UART_FLAG_ORE)) {
        handle->error = true;
        __HAL_UART_CLEAR_IT(&handle->hal_serial_handle, UART_CLEAR_OREF);
    }
    // Check for idle
    if (__HAL_UART_GET_FLAG(&handle->hal_serial_handle, UART_FLAG_IDLE) && handle->dma_mode) {
        __HAL_UART_CLEAR_IT(&handle->hal_serial_handle, UART_CLEAR_IDLEF);
        if (__HAL_DMA_GET_COUNTER(&handle->hal_rx_dma) != MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE) {
            // Disable serial interrupts
            mal_serial_interrupt_state_s state;
            mal_serial_disable_interrupt(handle, &state);
            // Check for RX transfer complete
                mal_hspec_stm32f7_serial_handle_rx_dma(handle);
            // Restore interrupts
            mal_serial_restore_interrupt(handle, &state);
        }
    }
}

static IRQn_Type mal_hspec_stm32f7_serial_get_irq(mal_serial_port_e port) {
    switch (port) {
        case MAL_SERIAL_PORT_1:
            return USART1_IRQn;
        case MAL_SERIAL_PORT_2:
            return USART2_IRQn;
        case MAL_SERIAL_PORT_3:
            return USART3_IRQn;
        case MAL_SERIAL_PORT_4:
            return UART4_IRQn;
        case MAL_SERIAL_PORT_5:
            return UART5_IRQn;
        case MAL_SERIAL_PORT_6:
            return USART6_IRQn;
        case MAL_SERIAL_PORT_7:
            return UART7_IRQn;
        case MAL_SERIAL_PORT_8:
        default:
            return UART8_IRQn;
    }
}

mal_error_e mal_serial_transfer(mal_serial_s *handle, uint16_t data) {
    // Check if interface is active
    if (handle->active) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Disable interrupts
    mal_serial_interrupt_state_s state;
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
        } while ((data_count < MAL_HSPEC_STM32F7_SERIAL_DMA_BUFFER_SIZE) &&
                 (MAL_ERROR_OK == result));
        // Start new transfer
        mal_hspec_stm32f7_dma_start(handle->tx_dma_stream,
                                    &handle->hal_tx_dma,
                                    (uint32_t)handle->tx_buffer,
                                    (uint32_t)&handle->hal_serial_handle.Instance->TDR,
                                    data_count);
    } else {
        // Start transfer and enable interrupt
        handle->hal_serial_handle.Instance->TDR = data;
        __HAL_UART_ENABLE_IT(&handle->hal_serial_handle, UART_IT_TXE);
    }
    handle->active = true;
    mal_serial_restore_interrupt(handle, &state);

    return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_serial_disable_interrupt(mal_serial_s *handle, mal_serial_interrupt_state_s *state) {
    // Check UART IRQ
    state->uart_state = NVIC_GetEnableIRQ(handle->uart_irq);
    NVIC_DisableIRQ(handle->uart_irq);
    // Check DMA IRQ
    if (handle->dma_mode) {
        state->dma_state = NVIC_GetEnableIRQ(handle->dma_tx_irq);
        NVIC_DisableIRQ(handle->dma_tx_irq);
    }
    __DSB();
    __ISB();
}

MAL_DEFS_INLINE void mal_serial_restore_interrupt(mal_serial_s *handle, mal_serial_interrupt_state_s *state) {
    if (state->uart_state) {
        NVIC_EnableIRQ(handle->uart_irq);
    }
    if (handle->dma_mode && state->dma_state) {
        NVIC_EnableIRQ(handle->dma_tx_irq);
    }
}
