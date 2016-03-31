/*
 * mal_hspec_stm32f0_spi.c
 *
 *  Created on: Mar 30, 2016
 *      Author: Olivier
 */

#include "mal_hspec_stm32f0_spi.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "gpio/mal_gpio.h"
#include "stm32f0/stm32f0xx_spi.h"

mal_error_e mal_hspec_stm32f0_spi_master_init(mal_hspec_spi_init_s *init) {
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->mosi->port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->miso->port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->clk->port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->select->port), ENABLE);
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
		gpio_init.direction = MAL_GPIO_DIR_OUT;
		gpio_init.gpio = *init->select;
		gpio_init.output_config = MAL_GPIO_OUT_PP;
		gpio_init.pupd = MAL_GPIO_PUPD_NONE;
		gpio_init.speed = UINT64_MAX;
		result = mal_gpio_init(&gpio_init);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		// Set high
		result = mal_gpio_set(init->select, true);
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

	spi_init.SPI_CPHA = SPI_CPHA_2Edge;
}
