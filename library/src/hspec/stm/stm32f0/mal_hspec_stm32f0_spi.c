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

mal_error_e mal_hspec_stm32f0_spi_master_init(mal_hspec_spi_init_s *init) {
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
	mal_error_e result;
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
	// Configure IRQ
	NVIC_InitTypeDef nvic_init;
	if (MAL_HSPEC_I2C_1 == init->interface) {
		nvic_init.NVIC_IRQChannel = SPI1_IRQn;
	} else {
		nvic_init.NVIC_IRQChannel = SPI2_IRQn;
	}
	//FIXME To fix with issue #19.
	nvic_init.NVIC_IRQChannelPriority = 10;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
	// Enable transmit buffer empty
	SPI_I2S_ITConfig(spi_typedef, SPI_I2S_IT_TXE, ENABLE);
	// Enable SPI interface
	SPI_Cmd(spi_typedef, ENABLE);

	return MAL_ERROR_OK;
}
