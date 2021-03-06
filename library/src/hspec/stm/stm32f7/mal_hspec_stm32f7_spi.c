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

#include "mal_hspec_stm32f7_spi.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "mal_hspec_stm32f7_gpio.h"
#include "gpio/mal_gpio.h"

#define MAL_HSPEC_STM32F7_DATA_SIZE_MASK	0xF00

static mal_error_e mal_hspec_stm32f7_spi_init_io(mal_spi_e interface, mal_gpio_port_e gpio_port, uint8_t pin);
static IRQn_Type mal_hspec_stm32f7_spi_get_irq(mal_spi_e interface);
static mal_error_e mal_hspec_stm32f7_spi_set_select_io(mal_spi_s *handle, bool selected);
static void mal_hspec_stm32f7_spi_send_data(mal_spi_s *handle);
static void mal_hspec_stm32f7_spi_handle_interrupt(mal_spi_s *handle);

static mal_spi_s *mal_hspec_stm32f7_spi_1;
static mal_spi_s *mal_hspec_stm32f7_spi_2;
static mal_spi_s *mal_hspec_stm32f7_spi_3;
static mal_spi_s *mal_hspec_stm32f7_spi_4;
static mal_spi_s *mal_hspec_stm32f7_spi_5;
static mal_spi_s *mal_hspec_stm32f7_spi_6;

mal_error_e mal_spi_init_master(mal_spi_master_init_s *init, mal_spi_s *handle) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    uint64_t pclk;
    // Initialize serial clock, set handles and get clock speed
    switch (init->interface) {
        case MAL_SPI_1:
            __HAL_RCC_SPI1_CLK_ENABLE();
            mal_hspec_stm32f7_spi_1 = handle;
            handle->hal_spi_handle.Instance = SPI1;
            pclk = (uint64_t)HAL_RCC_GetPCLK2Freq();
            break;
        case MAL_SPI_2:
            __HAL_RCC_SPI2_CLK_ENABLE();
            mal_hspec_stm32f7_spi_2 = handle;
            handle->hal_spi_handle.Instance = SPI2;
            pclk = (uint64_t)HAL_RCC_GetPCLK1Freq();
            break;
        case MAL_SPI_3:
            __HAL_RCC_SPI3_CLK_ENABLE();
            mal_hspec_stm32f7_spi_3 = handle;
            handle->hal_spi_handle.Instance = SPI3;
            pclk = (uint64_t)HAL_RCC_GetPCLK1Freq();
            break;
        case MAL_SPI_4:
            __HAL_RCC_SPI4_CLK_ENABLE();
            mal_hspec_stm32f7_spi_4 = handle;
            handle->hal_spi_handle.Instance = SPI4;
            pclk = (uint64_t)HAL_RCC_GetPCLK2Freq();
            break;
        case MAL_SPI_5:
            __HAL_RCC_SPI5_CLK_ENABLE();
            mal_hspec_stm32f7_spi_5 = handle;
            handle->hal_spi_handle.Instance = SPI5;
            pclk = (uint64_t)HAL_RCC_GetPCLK2Freq();
            break;
        case MAL_SPI_6:
            __HAL_RCC_SPI6_CLK_ENABLE();
            mal_hspec_stm32f7_spi_6 = handle;
            handle->hal_spi_handle.Instance = SPI6;
            pclk = (uint64_t)HAL_RCC_GetPCLK2Freq();
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Initialize MISO, MOSI, CLK and select GPIOs
    mal_result = mal_hspec_stm32f7_spi_init_io(init->interface, init->miso_port, init->miso_pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    mal_result = mal_hspec_stm32f7_spi_init_io(init->interface, init->mosi_port, init->mosi_pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    mal_result = mal_hspec_stm32f7_spi_init_io(init->interface, init->clk_port, init->clk_pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    if (MAL_SPI_SELECT_MODE_HARDWARE == init->select_mode) {
        mal_result = mal_hspec_stm32f7_spi_init_io(init->interface, init->select_port, init->select_pin);
        if (MAL_ERROR_OK != mal_result) {
            return mal_result;
        }
    } else if (MAL_SPI_SELECT_MODE_SOFTWARE == init->select_mode) {
        // Initialize GPIO
        mal_gpio_init_s gpio_init;
        gpio_init.direction = MAL_GPIO_DIRECTION_OUT;
        gpio_init.port = init->select_port;
        gpio_init.pin = init->select_pin;
        gpio_init.output_config = MAL_GPIO_OUT_PP;
        gpio_init.pupd = MAL_GPIO_PUPD_NONE;
        gpio_init.speed = UINT64_MAX;
        mal_result = mal_gpio_init(&gpio_init, &handle->select);
        if (MAL_ERROR_OK != mal_result) {
            return mal_result;
        }
        // Set unselected
        if (MAL_SPI_SELECT_POLARITY_HIGH == init->select_polarity) {
            mal_result = mal_gpio_set(&handle->select, false);
        } else {
            mal_result = mal_gpio_set(&handle->select, true);
        }
        if (MAL_ERROR_OK != mal_result) {
            return mal_result;
        }
    }
    // Reset interface
    hal_result = HAL_SPI_DeInit(&handle->hal_spi_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Initialize interface
    handle->hal_spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
    handle->hal_spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // Set full duplex mode
    handle->hal_spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
    // Set as master
    handle->hal_spi_handle.Init.Mode = SPI_MODE_MASTER;
    // Set word size
    if (init->data_size < MAL_SPI_DATA_4_BITS || init->data_size > MAL_SPI_DATA_16_BITS) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    handle->hal_spi_handle.Init.DataSize = (uint16_t)(init->data_size - 1) << 8;
    // Set clock idle polarity
    if (MAL_SPI_CLK_IDLE_STATE_LOW == init->clk_idle_state) {
        handle->hal_spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    } else {
        handle->hal_spi_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    }
    // Set on which edge data should be valid
    if (MAL_SPI_CLK_IDLE_STATE_LOW == init->clk_idle_state &&
        MAL_SPI_DATA_LATCH_EDGE_RISING == init->latch_edge) {
        handle->hal_spi_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    } else if (MAL_SPI_CLK_IDLE_STATE_HIGH == init->clk_idle_state &&
               MAL_SPI_DATA_LATCH_EDGE_FALLING == init->latch_edge) {
        handle->hal_spi_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    } else {
        handle->hal_spi_handle.Init.CLKPhase = SPI_PHASE_2EDGE;
    }
    // Set select (NSS) control mode
    switch (init->select_mode) {
        case MAL_SPI_SELECT_MODE_HARDWARE:
            // This MCU only supports low polarity in hardware mode
            if (MAL_SPI_SELECT_POLARITY_HIGH == init->select_polarity) {
                return MAL_ERROR_HARDWARE_INVALID;
            }
            handle->hal_spi_handle.Init.NSS = SPI_NSS_HARD_OUTPUT;
            handle->hal_spi_handle.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
            break;
        case MAL_SPI_SELECT_MODE_SOFTWARE:
        case MAL_SPI_SELECT_MODE_USER:
        case MAL_SPI_SELECT_MODE_NONE:
        default:
            handle->hal_spi_handle.Init.NSS = SPI_NSS_SOFT;
            handle->hal_spi_handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
            break;
    }
    // Get SPI clock source
    // Set the clock prescaler. The prescaler can only divide with powers of 2.
    // The valid values range from 2 to 256. In other words 2^1 to 2^8. The
    // prescaler value of the register is 0 based. This means you have to
    // subtract 1 to the value to set to the register.
    bool found = false;
    int prescaler = 0;
    uint64_t clock_speed = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(init->clock_speed) / 1000;
    for (prescaler = 0; prescaler <= 7; prescaler++) {
        uint64_t current_clock = pclk >> (prescaler + 1);
        // Check if this is a match
        if (current_clock == clock_speed) {
            found = true;
            break;
        }
        // Check if we should keep looking
        if (current_clock < clock_speed) {
            break;
        }
    }
    if (!found) {
        return MAL_ERROR_CLOCK_ERROR;
    }
    // The prescaler bits start at bit 3
    handle->hal_spi_handle.Init.BaudRatePrescaler = (uint32_t)(prescaler << 3);
    // Set bit order
    if (MAL_SPI_BIT_ORDER_MSB == init->bit_order) {
        handle->hal_spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    } else {
        handle->hal_spi_handle.Init.FirstBit = SPI_FIRSTBIT_LSB;
    }
    // Initialize
    hal_result = HAL_SPI_Init(&handle->hal_spi_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_INIT_FAILED;
    }
    // Set proper rx fifo threshold
    // If words are more than a byte then we only want to know when 2 bytes are
    // in fifo. This hardware does not support words greater then 16 bits so
    // there is no need to look for higher thresholds.
    if (init->data_size > MAL_SPI_DATA_8_BITS) {
        CLEAR_BIT(handle->hal_spi_handle.Instance->CR2, SPI_RXFIFO_THRESHOLD);
    } else {
        SET_BIT(handle->hal_spi_handle.Instance->CR2, SPI_RXFIFO_THRESHOLD);
    }
    // Initialize handle
    handle->active_msg = NULL;
    handle->is_active = false;
    handle->select_mode = init->select_mode;
    handle->select_polarity = init->select_polarity;
    handle->irq = mal_hspec_stm32f7_spi_get_irq(init->interface);
    // Configure IRQ
    //FIXME To fix with issue #19.
    NVIC_EnableIRQ(handle->irq);
    NVIC_SetPriority(handle->irq, 10);
    // Enable SPI interface
    __HAL_SPI_ENABLE(&handle->hal_spi_handle);

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_spi_init_io(mal_spi_e interface, mal_gpio_port_e gpio_port, uint8_t pin) {
    mal_error_e mal_result;
    uint32_t alternate;
    mal_result = mal_hspec_stm32f7_spi_get_alternate(interface, gpio_port, pin, &alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    mal_result = mal_hspec_stm32f7_gpio_init_alternate(gpio_port, pin, alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }

    return MAL_ERROR_OK;
}

static IRQn_Type mal_hspec_stm32f7_spi_get_irq(mal_spi_e interface) {
    switch (interface) {
        case MAL_SPI_1:
            return SPI1_IRQn;
        case MAL_SPI_2:
            return SPI2_IRQn;
        case MAL_SPI_3:
            return SPI3_IRQn;
        case MAL_SPI_4:
            return SPI4_IRQn;
        case MAL_SPI_5:
            return SPI5_IRQn;
        case MAL_SPI_6:
        default:
            return SPI6_IRQn;
    }
}

mal_error_e mal_spi_start_transaction(mal_spi_s *handle, mal_spi_msg_s *msg) {
    mal_error_e result;
    // Deactivate interrupt
    mal_spi_interrupt_state_s state;
    mal_spi_disable_interrupt(handle, &state);
    // Check if interface is available
    if (!handle->is_active) {
        // Set active message
        handle->active_msg = msg;
        // Set select IO
        result = mal_hspec_stm32f7_spi_set_select_io(handle, true);
        if (MAL_ERROR_OK == result) {
            // Set interface active
            handle->is_active = true;
            // Set data pointers
            handle->out_data_ptr = 0;
            handle->in_data_ptr = 0;
            // Send first data byte
            mal_hspec_stm32f7_spi_send_data(handle);
            // Set interrupts active
            __HAL_SPI_ENABLE_IT(&handle->hal_spi_handle, SPI_IT_TXE);
            __HAL_SPI_ENABLE_IT(&handle->hal_spi_handle, SPI_IT_RXNE);
        }
    } else {
        result = MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Restore interrupt
    mal_spi_restore_interrupt(handle, &state);

    return result;
}

static mal_error_e mal_hspec_stm32f7_spi_set_select_io(mal_spi_s *handle, bool selected) {
    // Extract correct select IO and polarity
    mal_gpio_s *select_io = NULL;
    mal_spi_select_polarity_e polarity = MAL_SPI_SELECT_POLARITY_LOW;
    // Check if the active message specifies an IO
    if (NULL != handle->active_msg &&
        NULL != handle->active_msg->select) {
        select_io = handle->active_msg->select;
        polarity = handle->active_msg->select_polarity;
    } else {
        if (MAL_SPI_SELECT_MODE_SOFTWARE == handle->select_mode) {
            select_io = &handle->select;
            polarity = handle->select_polarity;
        }
    }
    // Nothing to do if no IO is specified. This can happen when the interface
    // select is in user mode.
    if (NULL == select_io) {
        return MAL_ERROR_OK;
    }
    // Determine state IO based on select state and polarity
    bool state = selected;
    if (MAL_SPI_SELECT_POLARITY_LOW == polarity) {
        state = !state;
    }
    // Set IO
    return mal_gpio_set(select_io, state);
}

static void mal_hspec_stm32f7_spi_send_data(mal_spi_s *handle) {
    // Get index
    uint8_t index = handle->out_data_ptr++;
    // Get next word
    uint16_t data = handle->active_msg->data[index];
    // Send the next data word
    if ((handle->hal_spi_handle.Instance->CR2 & MAL_HSPEC_STM32F7_DATA_SIZE_MASK) > SPI_DATASIZE_8BIT) {
        handle->hal_spi_handle.Instance->DR = data;
    } else {
        *((__IO uint8_t *)&handle->hal_spi_handle.Instance->DR) = (uint8_t)data;
    }
}

MAL_DEFS_INLINE void mal_spi_disable_interrupt(mal_spi_s *handle, mal_spi_interrupt_state_s *state) {
    state->active = (bool)NVIC_GetEnableIRQ(handle->irq);
    NVIC_DisableIRQ(handle->irq);
    __DSB();
    __ISB();
}

MAL_DEFS_INLINE void mal_spi_restore_interrupt(mal_spi_s *handle, mal_spi_interrupt_state_s *state) {
    if (state->active) {
        NVIC_EnableIRQ(handle->irq);
    }
}

static void mal_hspec_stm32f7_spi_handle_interrupt(mal_spi_s *handle) {
    // Check transmit empty buffer interrupt
    if (__HAL_SPI_GET_FLAG(&handle->hal_spi_handle, SPI_FLAG_TXE)) {
        // Check if we still have data to send
        if (handle->out_data_ptr < handle->active_msg->data_length) {
            mal_hspec_stm32f7_spi_send_data(handle);
        }
    }
    // Check receiver not empty buffer interrupt
    if (__HAL_SPI_GET_FLAG(&handle->hal_spi_handle, SPI_FLAG_RXNE)) {
        // Get next index
        uint8_t index = handle->in_data_ptr++;
        // Read data
        uint16_t data;
        if ((handle->hal_spi_handle.Instance->CR2 & MAL_HSPEC_STM32F7_DATA_SIZE_MASK) > SPI_DATASIZE_8BIT) {
            data = (uint16_t)handle->hal_spi_handle.Instance->DR;
        } else {
            data = *(__IO uint8_t *)&handle->hal_spi_handle.Instance->DR;
        }
        handle->active_msg->data[index] = data;

        // Check if transaction is complete
        if (handle->in_data_ptr >= handle->active_msg->data_length) {
            // Deselect device
            mal_hspec_stm32f7_spi_set_select_io(handle, false);
            // Message transaction is complete
            // Execute callback
            mal_spi_msg_s *next_message = NULL;
            if (NULL != handle->active_msg->callback) {
                handle->active_msg->callback(handle->active_msg->handle, handle->active_msg, &next_message);
            }
            // Check if we have a new message
            if (NULL == next_message) {
                // No new message, set interface inactive
                handle->is_active = false;
                // Set interrupts inactive
                __HAL_SPI_DISABLE_IT(&handle->hal_spi_handle, SPI_IT_TXE);
                __HAL_SPI_DISABLE_IT(&handle->hal_spi_handle, SPI_IT_RXNE);
                return;
            }
            // Set interface for next message
            handle->active_msg = next_message;
            // Set select IO
            mal_hspec_stm32f7_spi_set_select_io(handle, true);
            // Set data pointer
            handle->out_data_ptr = 0;
            handle->in_data_ptr = 0;
        }
    }
}

void SPI1_IRQHandler(void) {
    mal_hspec_stm32f7_spi_handle_interrupt(mal_hspec_stm32f7_spi_1);
}

void SPI2_IRQHandler(void) {
    mal_hspec_stm32f7_spi_handle_interrupt(mal_hspec_stm32f7_spi_2);
}

void SPI3_IRQHandler(void) {
    mal_hspec_stm32f7_spi_handle_interrupt(mal_hspec_stm32f7_spi_3);
}

void SPI4_IRQHandler(void) {
    mal_hspec_stm32f7_spi_handle_interrupt(mal_hspec_stm32f7_spi_4);
}

void SPI5_IRQHandler(void) {
    mal_hspec_stm32f7_spi_handle_interrupt(mal_hspec_stm32f7_spi_5);
}

void SPI6_IRQHandler(void) {
    mal_hspec_stm32f7_spi_handle_interrupt(mal_hspec_stm32f7_spi_6);
}
