/*
 * mal_hspec_stm32f0_spi.c
 *
 *  Created on: Mar 30, 2016
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
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

#include "mal_hspec_stm32f0_spi.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "gpio/mal_gpio.h"
#include "stm32f0/stm32f0xx_spi.h"
#include "std/mal_stdlib.h"
#include "std/mal_bool.h"

#define DATA_SIZE_MASK	0xF00

typedef struct {
	mal_hspec_spi_e interface;
	SPI_TypeDef *spi_typedef;
	// Runtime settings
	const mal_hspec_gpio_s *select;
	mal_hspec_spi_select_mode_e select_mode;
	mal_hspec_spi_select_polarity_e select_polarity;
	// Runtime variables
	volatile bool is_active;
	mal_hspec_spi_msg_s *active_msg;
	uint8_t out_data_ptr;
	uint8_t in_data_ptr;
} stm_spi_interface_s;

static mal_error_e get_local_interface(mal_hspec_spi_e interface,
									   stm_spi_interface_s **local_interface);

static mal_error_e set_select_io(stm_spi_interface_s *local_interface,
								 bool selected);

static void handle_spi_interrupt(stm_spi_interface_s *local_interface);

static void send_data(stm_spi_interface_s *local_interface);

stm_spi_interface_s spi1_interface;
stm_spi_interface_s spi2_interface;

mal_error_e mal_hspec_stm32f0_spi_master_init(mal_hspec_spi_master_init_s *init) {
	mal_error_e result;
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->mosi->port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->miso->port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->clk->port), ENABLE);
	if (NULL != init->select) {
		RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->select->port), ENABLE);
	}
	// Enable SPI clock
	if (MAL_HSPEC_SPI_1 == init->interface) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	} else {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	}
	// Select correct I2C pin functions
	mal_hspec_stm32f0_af_e mosi_af;
	mal_hspec_stm32f0_af_e miso_af;
	mal_hspec_stm32f0_af_e clk_af;
	mal_hspec_stm32f0_af_e select_af;
	if (MAL_HSPEC_SPI_1 == init->interface) {
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
	result = mal_hspec_stm32f0_get_pin_af(init->mosi, mosi_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->mosi->port), init->mosi->pin, function);
	// MISO
	result = mal_hspec_stm32f0_get_pin_af(init->miso, miso_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->miso->port), init->miso->pin, function);
	// Clk
	result = mal_hspec_stm32f0_get_pin_af(init->clk, clk_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->clk->port), init->clk->pin, function);
	// Select, only if the select mode is hardware
	if (MAL_HSPEC_SPI_SELECT_MODE_HARDWARE == init->select_mode) {
		result = mal_hspec_stm32f0_get_pin_af(init->clk, clk_af, &function);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->clk->port), init->clk->pin, function);
	// Select mode is software, initialize IO as output
	} else if (MAL_HSPEC_SPI_SELECT_MODE_SOFTWARE == init->select_mode) {
		// Initialize GPIO
		mal_hspec_gpio_init_s gpio_init;
		gpio_init.direction = MAL_HSPEC_GPIO_DIR_OUT;
		gpio_init.gpio = *init->select;
		gpio_init.output_config = MAL_HSPEC_GPIO_OUT_PP;
		gpio_init.pupd = MAL_HSPEC_GPIO_PUPD_NONE;
		gpio_init.speed = UINT64_MAX;
		result = mal_gpio_init(&gpio_init);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		// Set unselected
		if (MAL_HSPEC_SPI_SELECT_POLARITY_HIGH == init->select_polarity) {
			result = mal_gpio_set(init->select, false);
		} else {
			result = mal_gpio_set(init->select, true);
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
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->mosi->pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->mosi->port), &gpio_init);
	// MISO
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->miso->pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->miso->port), &gpio_init);
	// Clk
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->clk->pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->clk->port), &gpio_init);
	// Select, only if select mode is hardware
	if (MAL_HSPEC_SPI_SELECT_MODE_HARDWARE == init->select_mode) {
		gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->miso->pin);
		GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->miso->port), &gpio_init);
	}
	// Get I2C typedef
	SPI_TypeDef *spi_typedef;
	if (MAL_HSPEC_I2C_1 == init->interface) {
		spi_typedef = SPI1;
	} else {
		spi_typedef = SPI2;
	}
	// Clear interface
	SPI_I2S_DeInit(spi_typedef);
	// Initialize interface
	SPI_InitTypeDef spi_init;
	// Set full duplex mode
	spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	// Set as master
	spi_init.SPI_Mode = SPI_Mode_Master;
	// Set word size
	if (init->data_size < MAL_HSPEC_SPI_DATA_4_BITS || init->data_size > MAL_HSPEC_SPI_DATA_16_BITS) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	spi_init.SPI_DataSize = (uint16_t)(init->data_size - 1) << 8;
	// Set clock idle polarity
	if (MAL_HSPEC_SPI_CLK_IDLE_STATE_LOW == init->clk_idle_state) {
		spi_init.SPI_CPOL = SPI_CPOL_Low;
	} else {
		spi_init.SPI_CPOL = SPI_CPOL_High;
	}
	// Set on which edge data should be valid
	if (MAL_HSPEC_SPI_CLK_IDLE_STATE_LOW == init->clk_idle_state &&
		MAL_HSPEC_SPI_DATA_LATCH_EDGE_RISING == init->latch_edge) {
		spi_init.SPI_CPHA = SPI_CPHA_1Edge;
	} else if (MAL_HSPEC_SPI_CLK_IDLE_STATE_HIGH == init->clk_idle_state &&
			   MAL_HSPEC_SPI_DATA_LATCH_EDGE_FALLING == init->latch_edge) {
		spi_init.SPI_CPHA = SPI_CPHA_1Edge;
	} else {
		spi_init.SPI_CPHA = SPI_CPHA_2Edge;
	}
	// Set select (NSS) control mode
	switch (init->select_mode) {
		case MAL_HSPEC_SPI_SELECT_MODE_HARDWARE:
			// This MCU only supports low polarity in hardware mode
			if (MAL_HSPEC_SPI_SELECT_POLARITY_HIGH == init->select_polarity) {
				return MAL_ERROR_HARDWARE_INVALID;
			}
			spi_init.SPI_NSS = SPI_NSS_Hard;
			SPI_NSSPulseModeCmd(spi_typedef, ENABLE);
			break;
		case MAL_HSPEC_SPI_SELECT_MODE_SOFTWARE:
		case MAL_HSPEC_SPI_SELECT_MODE_USER:
		case MAL_HSPEC_SPI_SELECT_MODE_NONE:
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
	for (prescaler = 0; prescaler <= 7; prescaler++) {
		uint32_t current_clock = clocks.PCLK_Frequency >> (prescaler + 1);
		// Check if this is a match
		if (current_clock == init->clock_speed) {
			found = true;
			break;
		}
		// Check if we should keep looking
		if (current_clock < init->clock_speed) {
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	// The prescaler bits start at bit 3
	spi_init.SPI_BaudRatePrescaler = prescaler << 3;
	// Set bit order
	if (MAL_HSPEC_SPI_BIT_ORDER_MSB == init->bit_order) {
		spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
	} else {
		spi_init.SPI_FirstBit = SPI_FirstBit_LSB;
	}
	// Initialize
	SPI_Init(spi_typedef, &spi_init);
	// Set proper rx fifo threshold
	uint16_t fifo_threshold = SPI_RxFIFOThreshold_QF;
	// If words are more than a byte then we only want to know when 2 bytes are
	// in fifo. This hardware does not support words greater then 16 bits so
	// there is no need to look for higher thresholds.
	if (init->data_size > MAL_HSPEC_SPI_DATA_8_BITS) {
		fifo_threshold = SPI_RxFIFOThreshold_HF;
	}
	SPI_RxFIFOThresholdConfig(spi_typedef, fifo_threshold);
	// Initialize local interface
	stm_spi_interface_s *local_interface;
	result = get_local_interface(init->interface, &local_interface);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	local_interface->active_msg = NULL;
	local_interface->interface = init->interface;
	local_interface->is_active = false;
	local_interface->select = init->select;
	local_interface->select_mode = init->select_mode;
	local_interface->select_polarity = init->select_polarity;
	local_interface->spi_typedef = spi_typedef;
	// Configure IRQ
	//FIXME To fix with issue #19.
	NVIC_EnableIRQ(mal_hspec_stm32f0_spi_get_irq(init->interface));
	NVIC_SetPriority(mal_hspec_stm32f0_spi_get_irq(init->interface), 10);
	// Enable SPI interface
	SPI_Cmd(spi_typedef, ENABLE);

	return MAL_ERROR_OK;
}

static mal_error_e get_local_interface(mal_hspec_spi_e interface, stm_spi_interface_s **local_interface) {
	switch (interface) {
		case MAL_HSPEC_SPI_1:
			*local_interface = &spi1_interface;
			return MAL_ERROR_OK;
		case MAL_HSPEC_SPI_2:
			*local_interface = &spi2_interface;
			return MAL_ERROR_OK;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
}

mal_error_e mal_hspec_stm32f0_spi_start_transaction(mal_hspec_spi_e interface,
										   	   	    mal_hspec_spi_msg_s *msg) {
	mal_error_e result = MAL_ERROR_OK;
	// Get local interface
	stm_spi_interface_s *local_interface;
	result = get_local_interface(interface, &local_interface);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Deactivate interrupt
	bool active = mal_hspec_stm32f0_spi_disable_interrupt(interface);
	// Check if interface is available
	if (!local_interface->is_active) {
		// Set active message
		local_interface->active_msg = msg;
		// Set select IO
		result = set_select_io(local_interface, true);
		if (MAL_ERROR_OK == result) {
			// Set interface active
			local_interface->is_active = true;
			// Set data pointers
			local_interface->out_data_ptr = 0;
			local_interface->in_data_ptr = 0;
			// Send first data byte
			send_data(local_interface);
			// Set interrupts active
			SPI_I2S_ITConfig(local_interface->spi_typedef, SPI_I2S_IT_TXE, ENABLE);
			SPI_I2S_ITConfig(local_interface->spi_typedef, SPI_I2S_IT_RXNE, ENABLE);
		}
	} else {
		result = MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	// Restore interrupt
	mal_hspec_stm32f0_spi_enable_interrupt(interface, active);

	return result;
}

IRQn_Type mal_hspec_stm32f0_spi_get_irq(mal_hspec_spi_e interface) {
	switch (interface) {
		case MAL_HSPEC_SPI_1:
			return SPI1_IRQn;
		case MAL_HSPEC_SPI_2:
		default:
			return SPI2_IRQn;
	}
}

bool mal_hspec_stm32f0_spi_disable_interrupt(mal_hspec_spi_e interface) {
	IRQn_Type irq = mal_hspec_stm32f0_spi_get_irq(interface);
	bool active = NVIC_GetActive(irq);
	NVIC_DisableIRQ(irq);
	__DSB();
	__ISB();
	return active;
}

static mal_error_e set_select_io(stm_spi_interface_s *local_interface,
								 bool selected) {
	// Extract correct select IO and polarity
	const mal_hspec_gpio_s *select_io = NULL;
	mal_hspec_spi_select_polarity_e polarity;
	// Check if the active message specifies an IO
	if (NULL != local_interface->active_msg &&
		NULL != local_interface->active_msg->select) {
		select_io = local_interface->active_msg->select;
		polarity = local_interface->active_msg->select_polarity;
	} else {
		if (MAL_HSPEC_SPI_SELECT_MODE_SOFTWARE == local_interface->select_mode &&
			NULL != local_interface->select) {
			select_io = local_interface->select;
			polarity = local_interface->select_polarity;
		}
	}
	// Nothing to do if no IO is specified. This can happen when the interface
	// select is in user mode.
	if (NULL == select_io) {
		return MAL_ERROR_OK;
	}
	// Determine state IO based on select state and polarity
	bool state = selected;
	if (MAL_HSPEC_SPI_SELECT_POLARITY_LOW == polarity) {
		state = !state;
	}
	// Set IO
	return mal_gpio_set(select_io, state);
}

void SPI1_IRQHandler(void) {
	handle_spi_interrupt(&spi1_interface);
}

void SPI2_IRQHandler(void) {
	handle_spi_interrupt(&spi2_interface);
}

static void handle_spi_interrupt(stm_spi_interface_s *local_interface) {
	// Check transmit empty buffer interrupt
	if (SPI_I2S_GetITStatus(local_interface->spi_typedef, SPI_I2S_IT_TXE) == SET) {
		// Check if we still have data to send
		if (local_interface->out_data_ptr < local_interface->active_msg->data_length) {
			send_data(local_interface);
		}
	}
	// Check receiver not empty buffer interrupt
	if (SPI_I2S_GetITStatus(local_interface->spi_typedef, SPI_I2S_IT_RXNE) == SET) {
		// Read last received bytes as this is a full duplex transaction
		// Get number of bytes in queue
		int8_t fifo_size;
		uint16_t fifo_status;
		//fifo_status = SPI_GetReceptionFIFOStatus(local_interface->spi_typedef);
		fifo_status = local_interface->spi_typedef->SR;
		fifo_status &= SPI_SR_FRLVL;
		switch (fifo_status) {
			case SPI_ReceptionFIFOStatus_Full:
				fifo_size = 4;
				break;
			case SPI_ReceptionFIFOStatus_HalfFull:
				fifo_size = 2;
				break;
			case SPI_ReceptionFIFOStatus_1QuarterFull:
				fifo_size = 1;
				break;
			case SPI_ReceptionFIFOStatus_Empty:
			default:
				fifo_size = 0;
				break;
		}
		// Read data
		while (fifo_size > 0) {
			// Read all the entire data register
			uint16_t data;
			data = SPI_I2S_ReceiveData16(local_interface->spi_typedef);
			if ((local_interface->spi_typedef->CR2 & DATA_SIZE_MASK) > SPI_DataSize_8b ||
				1 == fifo_size) {
				// Get index of last byte
				uint8_t index = local_interface->in_data_ptr++;
				// Store data
				local_interface->active_msg->data[index] = data;
			} else {
				// We just read 2 bytes
				for (int i = 0; i < 2; i++) {
					// Get index of last byte
					uint8_t index = local_interface->in_data_ptr++;
					// Store data
					local_interface->active_msg->data[index] = (data >> (8 * i)) & 0xFF;
				}
			}
			// Subtract 2. This is valid even there was only 1 byte, we
			// will exit the read loop.
			fifo_size -= 2;
		}
		// Check if transaction is complete
		if (local_interface->in_data_ptr >= local_interface->active_msg->data_length) {
			// Deselect device
			set_select_io(local_interface, false);
			// Message transaction is complete
			// Execute callback
			mal_hspec_spi_msg_s *next_message = NULL;
			if (NULL != local_interface->active_msg->callback) {
				local_interface->active_msg->callback(
												local_interface->active_msg,
												&next_message);
			}
			// Check if we have a new message
			if (NULL == next_message) {
				// No new message, set interface inactive
				local_interface->is_active = false;
				// Set interrupts inactive
				SPI_I2S_ITConfig(local_interface->spi_typedef, SPI_I2S_IT_TXE, DISABLE);
				SPI_I2S_ITConfig(local_interface->spi_typedef, SPI_I2S_IT_RXNE, DISABLE);
				return;
			}
			// Set interface for next message
			local_interface->active_msg = next_message;
			// Set select IO
			set_select_io(local_interface, true);
			// Set data pointer
			local_interface->out_data_ptr = 0;
		}
	}
}

static void send_data(stm_spi_interface_s *local_interface) {
	// Get index
	uint8_t index = local_interface->out_data_ptr++;
	// Get next word
	uint16_t data = local_interface->active_msg->data[index];
	// Send the next data word
	if ((local_interface->spi_typedef->CR2 & DATA_SIZE_MASK) > SPI_DataSize_8b) {
		SPI_I2S_SendData16(local_interface->spi_typedef, data);
	} else {
		SPI_SendData8(local_interface->spi_typedef, data);
	}
}
