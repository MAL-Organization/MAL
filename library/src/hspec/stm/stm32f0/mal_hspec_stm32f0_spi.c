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

#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "std/mal_stdlib.h"
#include "std/mal_bool.h"
#include "spi/mal_spi.h"
#include "cm0/core_cm0.h"

#define DATA_SIZE_MASK	0xF00

IRQn_Type mal_hspec_stm32f0_spi_get_irq(mal_spi_e interface);

static mal_error_e set_select_io(mal_spi_s *handle, bool selected);

static void handle_spi_interrupt(mal_spi_s *handle);

static void send_data(mal_spi_s *handle);

mal_spi_s *spi1_interface;
mal_spi_s *spi2_interface;

mal_error_e mal_spi_init_master(mal_spi_master_init_s *init, mal_spi_s *handle) {
	mal_error_e result;
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->mosi_port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->miso_port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->clk_port), ENABLE);
	if (MAL_SPI_SELECT_MODE_SOFTWARE == init->select_mode || MAL_SPI_SELECT_MODE_HARDWARE == init->select_mode) {
		RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->select_port), ENABLE);
	}
	// Enable SPI clock and save handles
	if (MAL_SPI_1 == init->interface) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		handle->spi_typedef = SPI1;
		spi1_interface = handle;
	} else if (MAL_SPI_2 == init->interface) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        handle->spi_typedef = SPI2;
        spi2_interface = handle;
	} else {
	    return MAL_ERROR_HARDWARE_INVALID;
	}
	// Select correct I2C pin functions
	mal_hspec_stm32f0_af_e mosi_af;
	mal_hspec_stm32f0_af_e miso_af;
	mal_hspec_stm32f0_af_e clk_af;
	mal_hspec_stm32f0_af_e select_af;
	if (MAL_SPI_1 == init->interface) {
		mosi_af = MAL_HSPEC_STM32F0_AF_SPI1_MOSI;
		miso_af = MAL_HSPEC_STM32F0_AF_SPI1_MISO;
		clk_af = MAL_HSPEC_STM32F0_AF_SPI1_SCK;
		select_af = MAL_HSPEC_STM32F0_AF_SPI1_NSS;
	} else {
		mosi_af = MAL_HSPEC_STM32F0_AF_SPI2_MOSI;
		miso_af = MAL_HSPEC_STM32F0_AF_SPI2_MISO;
		clk_af = MAL_HSPEC_STM32F0_AF_SPI2_SCK;
		select_af = MAL_HSPEC_STM32F0_AF_SPI2_NSS;
	}
	// Configure alternate function
	uint8_t function;
	// MOSI
	result = mal_hspec_stm32f0_get_pin_af(init->mosi_port, init->mosi_pin, mosi_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->mosi_port), init->mosi_pin, function);
	// MISO
	result = mal_hspec_stm32f0_get_pin_af(init->miso_port, init->miso_pin, miso_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->miso_port), init->miso_pin, function);
	// Clk
	result = mal_hspec_stm32f0_get_pin_af(init->clk_port, init->clk_pin, clk_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->clk_port), init->clk_pin, function);
	// Select, only if the select mode is hardware
	if (MAL_SPI_SELECT_MODE_HARDWARE == init->select_mode) {
		result = mal_hspec_stm32f0_get_pin_af(init->select_port, init->select_pin, select_af, &function);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->select_port), init->select_pin, function);
	// Select mode is software, initialize IO as output
	} else if (MAL_SPI_SELECT_MODE_SOFTWARE == init->select_mode) {
		// Initialize GPIO
		mal_gpio_init_s gpio_init;
		gpio_init.direction = MAL_GPIO_DIRECTION_OUT;
		gpio_init.port = init->select_port;
		gpio_init.pin = init->select_pin;
		gpio_init.output_config = MAL_GPIO_OUT_PP;
		gpio_init.pupd = MAL_GPIO_PUPD_NONE;
		gpio_init.speed = UINT64_MAX;
		result = mal_gpio_init(&gpio_init, &handle->select);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		// Set unselected
		if (MAL_SPI_SELECT_POLARITY_HIGH == init->select_polarity) {
			result = mal_gpio_set(&handle->select, false);
		} else {
			result = mal_gpio_set(&handle->select, true);
		}
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// Configure GPIOs
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	// MOSI
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->mosi_pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->mosi_port), &gpio_init);
	// MISO
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->miso_pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->miso_port), &gpio_init);
	// Clk
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->clk_pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->clk_port), &gpio_init);
	// Select, only if select mode is hardware
	if (MAL_SPI_SELECT_MODE_HARDWARE == init->select_mode) {
		gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->miso_pin);
		GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->miso_port), &gpio_init);
	}
	// Clear interface
	SPI_I2S_DeInit(handle->spi_typedef);
	// Initialize interface
	SPI_InitTypeDef spi_init;
	// Set full duplex mode
	spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	// Set as master
	spi_init.SPI_Mode = SPI_Mode_Master;
	// Set word size
	if (init->data_size < MAL_SPI_DATA_4_BITS || init->data_size > MAL_SPI_DATA_16_BITS) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	spi_init.SPI_DataSize = (uint16_t)(init->data_size - 1) << 8;
	// Set clock idle polarity
	if (MAL_SPI_CLK_IDLE_STATE_LOW == init->clk_idle_state) {
		spi_init.SPI_CPOL = SPI_CPOL_Low;
	} else {
		spi_init.SPI_CPOL = SPI_CPOL_High;
	}
	// Set on which edge data should be valid
	if (MAL_SPI_CLK_IDLE_STATE_LOW == init->clk_idle_state &&
		MAL_SPI_DATA_LATCH_EDGE_RISING == init->latch_edge) {
		spi_init.SPI_CPHA = SPI_CPHA_1Edge;
	} else if (MAL_SPI_CLK_IDLE_STATE_HIGH == init->clk_idle_state &&
			   MAL_SPI_DATA_LATCH_EDGE_FALLING == init->latch_edge) {
		spi_init.SPI_CPHA = SPI_CPHA_1Edge;
	} else {
		spi_init.SPI_CPHA = SPI_CPHA_2Edge;
	}
	// Set select (NSS) control mode
	switch (init->select_mode) {
		case MAL_SPI_SELECT_MODE_HARDWARE:
			// This MCU only supports low polarity in hardware mode
			if (MAL_SPI_SELECT_POLARITY_HIGH == init->select_polarity) {
				return MAL_ERROR_HARDWARE_INVALID;
			}
			spi_init.SPI_NSS = SPI_NSS_Hard;
			SPI_NSSPulseModeCmd(handle->spi_typedef, ENABLE);
			break;
		case MAL_SPI_SELECT_MODE_SOFTWARE:
		case MAL_SPI_SELECT_MODE_USER:
		case MAL_SPI_SELECT_MODE_NONE:
		default:
			spi_init.SPI_NSS = SPI_NSS_Soft;
			break;
	}
	// Get SPI clock source
	// Set I2C clock source
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// Set the clock prescaler. The prescaler can only divide with powers of 2.
	// The valid values range from 2 to 256. In other words 2^1 to 2^8. The
	// prescaler value of the register is 0 based. This means you have to
	// subtract 1 to the value to set to the register.
	bool found = false;
	int prescaler = 0;
	uint64_t clock_speed = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(init->clock_speed) / 1000;
	for (prescaler = 0; prescaler <= 7; prescaler++) {
		uint32_t current_clock = clocks.PCLK_Frequency >> (prescaler + 1);
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
	spi_init.SPI_BaudRatePrescaler = (uint16_t)(prescaler << 3);
	// Set bit order
	if (MAL_SPI_BIT_ORDER_MSB == init->bit_order) {
		spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
	} else {
		spi_init.SPI_FirstBit = SPI_FirstBit_LSB;
	}
	// Initialize
	SPI_Init(handle->spi_typedef, &spi_init);
	// Set proper rx fifo threshold
	uint16_t fifo_threshold = SPI_RxFIFOThreshold_QF;
	// If words are more than a byte then we only want to know when 2 bytes are
	// in fifo. This hardware does not support words greater then 16 bits so
	// there is no need to look for higher thresholds.
	if (init->data_size > MAL_SPI_DATA_8_BITS) {
		fifo_threshold = SPI_RxFIFOThreshold_HF;
	}
	SPI_RxFIFOThresholdConfig(handle->spi_typedef, fifo_threshold);
	// Initialize handle
	handle->active_msg = NULL;
	handle->is_active = false;
	handle->select_mode = init->select_mode;
	handle->select_polarity = init->select_polarity;
	handle->irq = mal_hspec_stm32f0_spi_get_irq(init->interface);
	// Configure IRQ
	//FIXME To fix with issue #19.
	NVIC_EnableIRQ(handle->irq);
	NVIC_SetPriority(handle->irq, 10);
	// Enable SPI interface
	SPI_Cmd(handle->spi_typedef, ENABLE);

	return MAL_ERROR_OK;
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
		result = set_select_io(handle, true);
		if (MAL_ERROR_OK == result) {
			// Set interface active
			handle->is_active = true;
			// Set data pointers
			handle->out_data_ptr = 0;
			handle->in_data_ptr = 0;
			// Send first data byte
			send_data(handle);
			// Set interrupts active
			SPI_I2S_ITConfig(handle->spi_typedef, SPI_I2S_IT_TXE, ENABLE);
			SPI_I2S_ITConfig(handle->spi_typedef, SPI_I2S_IT_RXNE, ENABLE);
		}
	} else {
		result = MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	// Restore interrupt
	mal_spi_restore_interrupt(handle, &state);

	return result;
}

IRQn_Type mal_hspec_stm32f0_spi_get_irq(mal_spi_e interface) {
	switch (interface) {
		case MAL_SPI_1:
			return SPI1_IRQn;
		case MAL_SPI_2:
		default:
			return SPI2_IRQn;
	}
}

MAL_DEFS_INLINE void mal_spi_disable_interrupt(mal_spi_s *handle, mal_spi_interrupt_state_s *state) {
	state->active = NVIC_GetActive(handle->irq);
	NVIC_DisableIRQ(handle->irq);
	__DSB();
	__ISB();
}

MAL_DEFS_INLINE void mal_spi_restore_interrupt(mal_spi_s *handle, mal_spi_interrupt_state_s *state) {
    if (state->active) {
        NVIC_EnableIRQ(handle->irq);
    }
}

static mal_error_e set_select_io(mal_spi_s *handle, bool selected) {
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

void SPI1_IRQHandler(void) {
	handle_spi_interrupt(spi1_interface);
}

void SPI2_IRQHandler(void) {
	handle_spi_interrupt(spi2_interface);
}

static void handle_spi_interrupt(mal_spi_s *handle) {
	// Check transmit empty buffer interrupt
	if (SPI_I2S_GetITStatus(handle->spi_typedef, SPI_I2S_IT_TXE) == SET) {
		// Check if we still have data to send
		if (handle->out_data_ptr < handle->active_msg->data_length) {
			send_data(handle);
		}
	}
	// Check receiver not empty buffer interrupt
	if (SPI_I2S_GetITStatus(handle->spi_typedef, SPI_I2S_IT_RXNE) == SET) {
		// Get next index
		uint8_t index = handle->in_data_ptr++;
		// Read data
		uint16_t data;
		if ((handle->spi_typedef->CR2 & DATA_SIZE_MASK) > SPI_DataSize_8b) {
			data = SPI_I2S_ReceiveData16(handle->spi_typedef);
		} else {
			data = SPI_ReceiveData8(handle->spi_typedef);
		}
		handle->active_msg->data[index] = data;

		// Check if transaction is complete
		if (handle->in_data_ptr >= handle->active_msg->data_length) {
			// Deselect device
			set_select_io(handle, false);
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
				SPI_I2S_ITConfig(handle->spi_typedef, SPI_I2S_IT_TXE, DISABLE);
				SPI_I2S_ITConfig(handle->spi_typedef, SPI_I2S_IT_RXNE, DISABLE);
				return;
			}
			// Set interface for next message
			handle->active_msg = next_message;
			// Set select IO
			set_select_io(handle, true);
			// Set data pointer
			handle->out_data_ptr = 0;
			handle->in_data_ptr = 0;
		}
	}
}

static void send_data(mal_spi_s *handle) {
	// Get index
	uint8_t index = handle->out_data_ptr++;
	// Get next word
	uint16_t data = handle->active_msg->data[index];
	// Send the next data word
	if ((handle->spi_typedef->CR2 & DATA_SIZE_MASK) > SPI_DataSize_8b) {
		SPI_I2S_SendData16(handle->spi_typedef, data);
	} else {
		SPI_SendData8(handle->spi_typedef, (uint8_t)data);
	}
}
