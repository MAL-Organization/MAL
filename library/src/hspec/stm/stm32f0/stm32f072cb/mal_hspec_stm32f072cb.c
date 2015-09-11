/*
 * mal_hspec_stm32f072cb.c
 *
 *  Created on: May 3, 2015
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

#include <stdlib.h>

#include "mal_hspec_stm32f072cb.h"

static const mal_hspec_port_e valid_ports[] = {
	MAL_HSPEC_PORT_A,
	MAL_HSPEC_PORT_B,
	MAL_HSPEC_PORT_C,
	MAL_HSPEC_PORT_D,
	MAL_HSPEC_PORT_F
};

static const uint64_t valid_pins[MAL_HSPEC_PORT_SIZE] = {
	0b1111111111111111,	// PA[0:15]
	0b1111111111111111, // PB[0:15]
	0b1110000000000000, // PC[13:15]
	0,
	0,
	0b11			// PF[0:1]
};

static const mal_hspec_timer_e valid_timers[] = {
	MAL_HSPEC_TIMER_1,
	MAL_HSPEC_TIMER_2,
	MAL_HSPEC_TIMER_3,
	MAL_HSPEC_TIMER_6,
	MAL_HSPEC_TIMER_7,
	MAL_HSPEC_TIMER_14,
	MAL_HSPEC_TIMER_15,
	MAL_HSPEC_TIMER_16,
	MAL_HSPEC_TIMER_17
};

static const mal_hspec_gpio_s valid_i2c1_scl_gpios[] = {
	{
		.pin = 6,
		.port = MAL_HSPEC_PORT_B
	},
	{
		.pin = 8,
		.port = MAL_HSPEC_PORT_B
	}
};

static const mal_hspec_gpio_s valid_i2c1_sda_gpios[] = {
	{
		.pin = 7,
		.port = MAL_HSPEC_PORT_B
	},
	{
		.pin = 9,
		.port = MAL_HSPEC_PORT_B
	}
};

static const mal_hspec_gpio_s valid_i2c2_scl_gpios[] = {
	{
		.pin = 10,
		.port = MAL_HSPEC_PORT_B
	},
	{
		.pin = 13,
		.port = MAL_HSPEC_PORT_B
	}
};

static const mal_hspec_gpio_s valid_i2c2_sda_gpios[] = {
	{
		.pin = 11,
		.port = MAL_HSPEC_PORT_B
	},
	{
		.pin = 14,
		.port = MAL_HSPEC_PORT_B
	}
};

static const mal_hspec_stm32f0_af_e port_a_valid_afs[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE] = {
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_USART2_CTS},{MAL_HSPEC_STM32F0_AF_TIM2_CH1_ETR},{MAL_HSPEC_STM32F0_AF_TSC_G1_IO1},{MAL_HSPEC_STM32F0_AF_USART4_TX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_COMP1_OUT}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_USART2_RTS},{MAL_HSPEC_STM32F0_AF_TIM2_CH2},{MAL_HSPEC_STM32F0_AF_TSC_G1_IO2},{MAL_HSPEC_STM32F0_AF_USART4_RX},{MAL_HSPEC_STM32F0_AF_TIM15_CH1N},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM15_CH1},{MAL_HSPEC_STM32F0_AF_USART2_TX},{MAL_HSPEC_STM32F0_AF_TIM2_CH3},{MAL_HSPEC_STM32F0_AF_TSC_G1_IO3},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_COMP2_OUT}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM15_CH2},{MAL_HSPEC_STM32F0_AF_USART2_RX},{MAL_HSPEC_STM32F0_AF_TIM2_CH4},{MAL_HSPEC_STM32F0_AF_TSC_G1_IO4},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_NSS,MAL_HSPEC_STM32F0_AF_I2S1_WS},{MAL_HSPEC_STM32F0_AF_USART2_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_TSC_G2_IO1},{MAL_HSPEC_STM32F0_AF_TIM14_CH1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_SCK,MAL_HSPEC_STM32F0_AF_I2S1_CK},{MAL_HSPEC_STM32F0_AF_CEC},{MAL_HSPEC_STM32F0_AF_TIM2_CH1_ETR},{MAL_HSPEC_STM32F0_AF_TSC_G2_IO2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_MISO,MAL_HSPEC_STM32F0_AF_I2S1_MCK},{MAL_HSPEC_STM32F0_AF_TIM3_CH1},{MAL_HSPEC_STM32F0_AF_TIM1_BKIN},{MAL_HSPEC_STM32F0_AF_TSC_G2_IO3},{MAL_HSPEC_STM32F0_AF_USART3_CTS},{MAL_HSPEC_STM32F0_AF_TIM16_CH1},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_COMP1_OUT}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_MOSI,MAL_HSPEC_STM32F0_AF_I2S1_SD},{MAL_HSPEC_STM32F0_AF_TIM3_CH2},{MAL_HSPEC_STM32F0_AF_TIM1_CH1N},{MAL_HSPEC_STM32F0_AF_TSC_G2_IO4},{MAL_HSPEC_STM32F0_AF_TIM14_CH1},{MAL_HSPEC_STM32F0_AF_TIM17_CH1},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_COMP2_OUT}
	},
	{
		{MAL_HSPEC_STM32F0_AF_MCO},{MAL_HSPEC_STM32F0_AF_USART1_CK},{MAL_HSPEC_STM32F0_AF_TIM1_CH1},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_CRS_SYNC},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM15_BKIN},{MAL_HSPEC_STM32F0_AF_USART1_TX},{MAL_HSPEC_STM32F0_AF_TIM1_CH2},{MAL_HSPEC_STM32F0_AF_TSC_G4_IO1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM17_BKIN},{MAL_HSPEC_STM32F0_AF_USART1_RX},{MAL_HSPEC_STM32F0_AF_TIM1_CH3},{MAL_HSPEC_STM32F0_AF_TSC_G4_IO2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_USART1_CTS},{MAL_HSPEC_STM32F0_AF_TIM1_CH4},{MAL_HSPEC_STM32F0_AF_TSC_G4_IO3},{MAL_HSPEC_STM32F0_AF_CAN_RX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_COMP1_OUT}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_USART1_RTS},{MAL_HSPEC_STM32F0_AF_TIM1_ETR},{MAL_HSPEC_STM32F0_AF_TSC_G4_IO4},{MAL_HSPEC_STM32F0_AF_CAN_TX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_COMP2_OUT}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SWDIO},{MAL_HSPEC_STM32F0_AF_IR_OUT},{MAL_HSPEC_STM32F0_AF_USB_NOE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SWCLK},{MAL_HSPEC_STM32F0_AF_USART2_TX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_NSS,MAL_HSPEC_STM32F0_AF_I2S1_WS},{MAL_HSPEC_STM32F0_AF_USART2_RX},{MAL_HSPEC_STM32F0_AF_TIM2_CH1_ETR},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_USART4_RTS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	}
};

static const mal_hspec_stm32f0_af_e port_b_valid_afs[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE] = {
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_TIM3_CH3},{MAL_HSPEC_STM32F0_AF_TIM1_CH2N},{MAL_HSPEC_STM32F0_AF_TSC_G3_IO2},{MAL_HSPEC_STM32F0_AF_USART3_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM14_CH1},{MAL_HSPEC_STM32F0_AF_TIM3_CH4},{MAL_HSPEC_STM32F0_AF_TIM1_CH3N},{MAL_HSPEC_STM32F0_AF_TSC_G3_IO3},{MAL_HSPEC_STM32F0_AF_USART3_RTS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_TSC_G3_IO4},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_SCK,MAL_HSPEC_STM32F0_AF_I2S1_CK},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_TIM2_CH2},{MAL_HSPEC_STM32F0_AF_TSC_G5_IO1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_MISO,MAL_HSPEC_STM32F0_AF_I2S1_MCK},{MAL_HSPEC_STM32F0_AF_TIM3_CH1},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_TSC_G5_IO2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_TIM17_BKIN},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI1_MOSI,MAL_HSPEC_STM32F0_AF_I2S1_SD},{MAL_HSPEC_STM32F0_AF_TIM3_CH2},{MAL_HSPEC_STM32F0_AF_TIM16_BKIN},{MAL_HSPEC_STM32F0_AF_I2C1_SMBA},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART1_TX},{MAL_HSPEC_STM32F0_AF_I2C1_SCL},{MAL_HSPEC_STM32F0_AF_TIM16_CH1N},{MAL_HSPEC_STM32F0_AF_TSC_G5_IO3},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART1_RX},{MAL_HSPEC_STM32F0_AF_I2C1_SDA},{MAL_HSPEC_STM32F0_AF_TIM17_CH1N},{MAL_HSPEC_STM32F0_AF_TSC_G5_IO4},{MAL_HSPEC_STM32F0_AF_USART4_CTS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_CEC},{MAL_HSPEC_STM32F0_AF_I2C1_SCL},{MAL_HSPEC_STM32F0_AF_TIM16_CH1},{MAL_HSPEC_STM32F0_AF_TSC_SYNC},{MAL_HSPEC_STM32F0_AF_CAN_RX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_IR_OUT},{MAL_HSPEC_STM32F0_AF_I2C1_SDA},{MAL_HSPEC_STM32F0_AF_TIM17_CH1},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_CAN_TX},{MAL_HSPEC_STM32F0_AF_SPI2_NSS,MAL_HSPEC_STM32F0_AF_I2S2_WS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_CEC},{MAL_HSPEC_STM32F0_AF_I2C2_SCL},{MAL_HSPEC_STM32F0_AF_TIM2_CH3},{MAL_HSPEC_STM32F0_AF_TSC_SYNC},{MAL_HSPEC_STM32F0_AF_USART3_TX},{MAL_HSPEC_STM32F0_AF_SPI2_SCK,MAL_HSPEC_STM32F0_AF_I2S2_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_I2C2_SDA},{MAL_HSPEC_STM32F0_AF_TIM2_CH4},{MAL_HSPEC_STM32F0_AF_TSC_G6_IO1},{MAL_HSPEC_STM32F0_AF_USART3_RX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI2_NSS,MAL_HSPEC_STM32F0_AF_I2S2_WS},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_TIM1_BKIN},{MAL_HSPEC_STM32F0_AF_TSC_G6_IO2},{MAL_HSPEC_STM32F0_AF_USART3_CK},{MAL_HSPEC_STM32F0_AF_TIM15_BKIN},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI2_SCK,MAL_HSPEC_STM32F0_AF_I2S2_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_TIM1_CH1N},{MAL_HSPEC_STM32F0_AF_TSC_G6_IO3},{MAL_HSPEC_STM32F0_AF_USART3_CTS},{MAL_HSPEC_STM32F0_AF_I2C2_SCL},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI2_MISO,MAL_HSPEC_STM32F0_AF_I2S2_MCK},{MAL_HSPEC_STM32F0_AF_TIM15_CH1},{MAL_HSPEC_STM32F0_AF_TIM1_CH2N},{MAL_HSPEC_STM32F0_AF_TSC_G6_IO4},{MAL_HSPEC_STM32F0_AF_USART3_RTS},{MAL_HSPEC_STM32F0_AF_I2C2_SDA},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_SPI2_MOSI,MAL_HSPEC_STM32F0_AF_I2S2_SD},{MAL_HSPEC_STM32F0_AF_TIM15_CH2},{MAL_HSPEC_STM32F0_AF_TIM1_CH3N},{MAL_HSPEC_STM32F0_AF_TIM15_CH1N},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	}
};

static const mal_hspec_stm32f0_af_e port_c_valid_afs[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE] = {
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_SPI2_MISO,MAL_HSPEC_STM32F0_AF_I2S2_MCK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_SPI2_MOSI,MAL_HSPEC_STM32F0_AF_I2S2_SD},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_USART3_TX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TSC_G3_IO1},{MAL_HSPEC_STM32F0_AF_USART3_RX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH3},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH4},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART4_TX},{MAL_HSPEC_STM32F0_AF_USART3_TX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART4_RX},{MAL_HSPEC_STM32F0_AF_USART3_RX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART4_CK},{MAL_HSPEC_STM32F0_AF_USART3_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	}
};

static const mal_hspec_stm32f0_af_e port_d_valid_afs[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE] = {
	{
		{MAL_HSPEC_STM32F0_AF_CAN_RX},{MAL_HSPEC_STM32F0_AF_SPI2_NSS,MAL_HSPEC_STM32F0_AF_I2S2_WS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_CAN_TX},{MAL_HSPEC_STM32F0_AF_SPI2_SCK,MAL_HSPEC_STM32F0_AF_I2S2_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_ETR},{MAL_HSPEC_STM32F0_AF_USART3_RTS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART2_CTS},{MAL_HSPEC_STM32F0_AF_SPI2_MISO,MAL_HSPEC_STM32F0_AF_I2S2_MCK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART2_RTS},{MAL_HSPEC_STM32F0_AF_SPI2_MOSI,MAL_HSPEC_STM32F0_AF_I2S2_SD},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART2_TX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART2_RX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART2_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART3_TX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART3_RX},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART3_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART3_CTS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_USART3_RTS},{MAL_HSPEC_STM32F0_AF_TSC_G8_IO1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_TSC_G8_IO2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_TSC_G8_IO3},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_CRS_SYNC},{MAL_HSPEC_STM32F0_AF_TSC_G8_IO4},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	}
};

static const mal_hspec_stm32f0_af_e port_e_valid_afs[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE] = {
	{
		{MAL_HSPEC_STM32F0_AF_TIM16_CH1},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM17_CH1},{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_ETR},{MAL_HSPEC_STM32F0_AF_TSC_G7_IO1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH1},{MAL_HSPEC_STM32F0_AF_TSC_G7_IO2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH2},{MAL_HSPEC_STM32F0_AF_TSC_G7_IO3},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH3},{MAL_HSPEC_STM32F0_AF_TSC_G7_IO4},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM3_CH4},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_ETR},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_CH1N},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_CH1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_CH2N},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_CH2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_CH3N},{MAL_HSPEC_STM32F0_AF_SPI1_NSS,MAL_HSPEC_STM32F0_AF_I2S1_WS},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_CH3},{MAL_HSPEC_STM32F0_AF_SPI1_SCK,MAL_HSPEC_STM32F0_AF_I2S1_CK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_CH4},{MAL_HSPEC_STM32F0_AF_SPI1_MISO,MAL_HSPEC_STM32F0_AF_I2S1_MCK},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM1_BKIN},{MAL_HSPEC_STM32F0_AF_SPI1_MOSI,MAL_HSPEC_STM32F0_AF_I2S1_SD},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	}
};

static const mal_hspec_stm32f0_af_e port_f_valid_afs[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE] = {
	{
		{MAL_HSPEC_STM32F0_AF_CRS_SYNC},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_EVENTOUT},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM15_CH1},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_TIM15_CH2},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	},
	{
		{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE},{MAL_HSPEC_STM32F0_AF_NONE}
	}
};

static const mal_hspec_gpio_s valid_can1_tx_gpios[] = {
	{
		.pin = 12,
		.port = MAL_HSPEC_PORT_A
	},
	{
		.pin = 9,
		.port = MAL_HSPEC_PORT_B
	}
};

static const mal_hspec_gpio_s valid_can1_rx_gpios[] = {
	{
		.pin = 11,
		.port = MAL_HSPEC_PORT_A
	},
	{
		.pin = 8,
		.port = MAL_HSPEC_PORT_B
	}
};

static const mal_hspec_gpio_s valid_adc0_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 0
	}
};

static const mal_hspec_gpio_s valid_adc1_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 1
	}
};

static const mal_hspec_gpio_s valid_adc2_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 2
	}
};

static const mal_hspec_gpio_s valid_adc3_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 3
	}
};

static const mal_hspec_gpio_s valid_adc4_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 4
	}
};

static const mal_hspec_gpio_s valid_adc5_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 5
	}
};

static const mal_hspec_gpio_s valid_adc6_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 6
	}
};

static const mal_hspec_gpio_s valid_adc7_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_A,
		.pin = 7
	}
};

static const mal_hspec_gpio_s valid_adc8_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_B,
		.pin = 0
	}
};

static const mal_hspec_gpio_s valid_adc9_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_B,
		.pin = 1
	}
};

static const mal_hspec_gpio_s valid_adc10_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_C,
		.pin = 0
	}
};

static const mal_hspec_gpio_s valid_adc11_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_C,
		.pin = 1
	}
};

static const mal_hspec_gpio_s valid_adc12_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_C,
		.pin = 2
	}
};

static const mal_hspec_gpio_s valid_adc13_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_C,
		.pin = 3
	}
};

static const mal_hspec_gpio_s valid_adc14_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_C,
		.pin = 4
	}
};

static const mal_hspec_gpio_s valid_adc15_gpios[] = {
	{
		.port = MAL_HSPEC_PORT_C,
		.pin = 5
	}
};

bool mal_hspec_stm32f072cb_is_pll_div_available(mal_hspec_system_clk_src_e source) {
	return true;
}

mal_error_e mal_hspec_stm32f072cb_get_valid_ports(const mal_hspec_port_e **ports, uint8_t *size) {
	*ports = valid_ports;
	*size = sizeof(valid_ports) / sizeof(mal_hspec_port_e);
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f072cb_get_valid_pins(const uint64_t **pins) {
	*pins = valid_pins;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f072cb_get_valid_timers(const mal_hspec_timer_e **timers, uint8_t *size) {
	*timers = valid_timers;
	*size = sizeof(valid_timers) / sizeof(mal_hspec_timer_e);
	return MAL_ERROR_OK;
}

IRQn_Type mal_hspec_stm32f072cb_get_timer_update_irq(mal_hspec_timer_e timer) {
	switch (timer) {
	case MAL_HSPEC_TIMER_1:
		return TIM1_BRK_UP_TRG_COM_IRQn;
	case MAL_HSPEC_TIMER_2:
		return TIM2_IRQn;
	case MAL_HSPEC_TIMER_3:
		return TIM3_IRQn;
	case MAL_HSPEC_TIMER_6:
		return TIM6_DAC_IRQn;
	case MAL_HSPEC_TIMER_7:
		return TIM7_IRQn;
	case MAL_HSPEC_TIMER_14:
		return TIM14_IRQn;
	case MAL_HSPEC_TIMER_15:
		return TIM15_IRQn;
	case MAL_HSPEC_TIMER_16:
		return TIM16_IRQn;
	case MAL_HSPEC_TIMER_17:
	default:
		return TIM17_IRQn;
	}
}

mal_error_e mal_hspec_stm32f072cb_get_valid_i2c_ios(mal_hspec_i2c_e interface, const mal_hspec_gpio_s **scls, uint8_t *scls_size, const mal_hspec_gpio_s **sdas, uint8_t *sdas_size) {
	switch (interface) {
	case MAL_HSPEC_I2C_1:
		*scls = valid_i2c1_scl_gpios;
		*scls_size = sizeof(valid_i2c1_scl_gpios) / sizeof(mal_hspec_gpio_s);
		*sdas = valid_i2c1_sda_gpios;
		*sdas_size = sizeof(valid_i2c1_sda_gpios) / sizeof(mal_hspec_gpio_s);
		break;
	case MAL_HSPEC_I2C_2:
		*scls = valid_i2c2_scl_gpios;
		*scls_size = sizeof(valid_i2c2_scl_gpios) / sizeof(mal_hspec_gpio_s);
		*sdas = valid_i2c2_sda_gpios;
		*sdas_size = sizeof(valid_i2c2_sda_gpios) / sizeof(mal_hspec_gpio_s);
		break;
	default:
		return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f072cb_get_port_afs(mal_hspec_port_e port, const mal_hspec_stm32f0_af_e (**afs)[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE]) {
	switch (port) {
	case MAL_HSPEC_PORT_A:
		*afs = &port_a_valid_afs;
		break;
	case MAL_HSPEC_PORT_B:
		*afs = &port_b_valid_afs;
		break;
	case MAL_HSPEC_PORT_C:
		*afs = &port_c_valid_afs;
		break;
	case MAL_HSPEC_PORT_D:
		*afs = &port_d_valid_afs;
		break;
	case MAL_HSPEC_PORT_E:
		*afs = &port_e_valid_afs;
		break;
	case MAL_HSPEC_PORT_F:
		*afs = &port_f_valid_afs;
		break;
	default:
		*afs = NULL;
		break;
	}
	if (NULL == *afs) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f072cb_get_valid_can_ios(mal_hspec_can_e interface, const mal_hspec_gpio_s **txs, uint8_t *txs_size, const mal_hspec_gpio_s **rxs, uint8_t *rxs_size) {
	if (MAL_HSPEC_CAN_1 != interface) {
		return MAL_ERROR_HARDWARE_INVALID;
	}

	*txs = valid_can1_tx_gpios;
	*txs_size = sizeof(valid_can1_tx_gpios) / sizeof(mal_hspec_gpio_s);
	*rxs = valid_can1_rx_gpios;
	*rxs_size = sizeof(valid_can1_rx_gpios) / sizeof(mal_hspec_gpio_s);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f072cb_get_valid_adc_ios(mal_hspec_adc_e adc, const mal_hspec_gpio_s **ios, uint8_t *size) {
	switch (adc) {
		case MAL_HSPEC_ADC_0:
			*ios = valid_adc0_gpios;
			*size = sizeof(valid_adc0_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_1:
			*ios = valid_adc1_gpios;
			*size = sizeof(valid_adc1_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_2:
			*ios = valid_adc2_gpios;
			*size = sizeof(valid_adc2_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_3:
			*ios = valid_adc3_gpios;
			*size = sizeof(valid_adc3_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_4:
			*ios = valid_adc4_gpios;
			*size = sizeof(valid_adc4_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_5:
			*ios = valid_adc5_gpios;
			*size = sizeof(valid_adc5_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_6:
			*ios = valid_adc6_gpios;
			*size = sizeof(valid_adc6_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_7:
			*ios = valid_adc7_gpios;
			*size = sizeof(valid_adc7_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_8:
			*ios = valid_adc8_gpios;
			*size = sizeof(valid_adc8_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_9:
			*ios = valid_adc9_gpios;
			*size = sizeof(valid_adc9_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_10:
			*ios = valid_adc10_gpios;
			*size = sizeof(valid_adc10_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_11:
			*ios = valid_adc11_gpios;
			*size = sizeof(valid_adc11_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_12:
			*ios = valid_adc12_gpios;
			*size = sizeof(valid_adc12_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_13:
			*ios = valid_adc13_gpios;
			*size = sizeof(valid_adc13_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_14:
			*ios = valid_adc14_gpios;
			*size = sizeof(valid_adc14_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_ADC_15:
			*ios = valid_adc15_gpios;
			*size = sizeof(valid_adc15_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		default:
			*ios = NULL;
			break;
	}
	if (NULL == *ios) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}
