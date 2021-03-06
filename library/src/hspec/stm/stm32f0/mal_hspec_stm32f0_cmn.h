/*
 * mal_hspec_stm32f0_cmn.h
 *
 *  Created on: Jun 24, 2015
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CMN_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CMN_H_

typedef enum MAL_HSPEC_STM32F0_AF {
	MAL_HSPEC_STM32F0_AF_NONE = 0,
	MAL_HSPEC_STM32F0_AF_USART2_CTS,
	MAL_HSPEC_STM32F0_AF_TIM2_CH1_ETR,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO1,
	MAL_HSPEC_STM32F0_AF_COMP1_OUT,
	MAL_HSPEC_STM32F0_AF_EVENTOUT,
	MAL_HSPEC_STM32F0_AF_USART2_RTS,
	MAL_HSPEC_STM32F0_AF_TIM2_CH2,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO2,
	MAL_HSPEC_STM32F0_AF_TIM15_CH1,
	MAL_HSPEC_STM32F0_AF_USART2_TX,
	MAL_HSPEC_STM32F0_AF_TIM2_CH3,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO3,
	MAL_HSPEC_STM32F0_AF_COMP2_OUT,
	MAL_HSPEC_STM32F0_AF_TIM15_CH2,
	MAL_HSPEC_STM32F0_AF_USART2_RX,
	MAL_HSPEC_STM32F0_AF_TIM2_CH4,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO4,
	MAL_HSPEC_STM32F0_AF_SPI1_NSS,
	MAL_HSPEC_STM32F0_AF_I2S1_WS,
	MAL_HSPEC_STM32F0_AF_USART2_CK,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO1,
	MAL_HSPEC_STM32F0_AF_TIM14_CH1,
	MAL_HSPEC_STM32F0_AF_SPI1_SCK,
	MAL_HSPEC_STM32F0_AF_I2S1_CK,
	MAL_HSPEC_STM32F0_AF_CEC,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO2,
	MAL_HSPEC_STM32F0_AF_SPI1_MISO,
	MAL_HSPEC_STM32F0_AF_I2S1_MCK,
	MAL_HSPEC_STM32F0_AF_TIM3_CH1,
	MAL_HSPEC_STM32F0_AF_TIM1_BKIN,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO3,
	MAL_HSPEC_STM32F0_AF_TIM16_CH1,
	MAL_HSPEC_STM32F0_AF_SPI1_MOSI,
	MAL_HSPEC_STM32F0_AF_I2S1_SD,
	MAL_HSPEC_STM32F0_AF_TIM3_CH2,
	MAL_HSPEC_STM32F0_AF_TIM1_CH1N,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO4,
	MAL_HSPEC_STM32F0_AF_TIM17_CH1,
	MAL_HSPEC_STM32F0_AF_MCO,
	MAL_HSPEC_STM32F0_AF_USART1_CK,
	MAL_HSPEC_STM32F0_AF_TIM1_CH1,
	MAL_HSPEC_STM32F0_AF_TIM15_BKIN,
	MAL_HSPEC_STM32F0_AF_USART1_TX,
	MAL_HSPEC_STM32F0_AF_TIM1_CH2,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO1,
	MAL_HSPEC_STM32F0_AF_TIM17_BKIN,
	MAL_HSPEC_STM32F0_AF_USART1_RX,
	MAL_HSPEC_STM32F0_AF_TIM1_CH3,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO2,
	MAL_HSPEC_STM32F0_AF_USART1_CTS,
	MAL_HSPEC_STM32F0_AF_TIM1_CH4,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO3,
	MAL_HSPEC_STM32F0_AF_USART1_RTS,
	MAL_HSPEC_STM32F0_AF_TIM1_ETR,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO4,
	MAL_HSPEC_STM32F0_AF_SWDIO,
	MAL_HSPEC_STM32F0_AF_IR_OUT,
	MAL_HSPEC_STM32F0_AF_SWCLK,
	MAL_HSPEC_STM32F0_AF_TIM3_CH3,
	MAL_HSPEC_STM32F0_AF_TIM1_CH2N,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO2,
	MAL_HSPEC_STM32F0_AF_TIM3_CH4,
	MAL_HSPEC_STM32F0_AF_TIM1_CH3N,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO3,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO4,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO1,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO2,
	MAL_HSPEC_STM32F0_AF_TIM16_BKIN,
	MAL_HSPEC_STM32F0_AF_I2C1_SMBA,
	MAL_HSPEC_STM32F0_AF_I2C1_SCL,
	MAL_HSPEC_STM32F0_AF_TIM16_CH1N,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO3,
	MAL_HSPEC_STM32F0_AF_I2C1_SDA,
	MAL_HSPEC_STM32F0_AF_TIM17_CH1N,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO4,
	MAL_HSPEC_STM32F0_AF_TSC_SYNC,
	MAL_HSPEC_STM32F0_AF_I2C2_SCL,
	MAL_HSPEC_STM32F0_AF_I2C2_SDA,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO1,
	MAL_HSPEC_STM32F0_AF_SPI2_NSS,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO2,
	MAL_HSPEC_STM32F0_AF_SPI2_SCK,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO3,
	MAL_HSPEC_STM32F0_AF_SPI2_MISO,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO4,
	MAL_HSPEC_STM32F0_AF_SPI2_MOSI,
	MAL_HSPEC_STM32F0_AF_TIM15_CH1N,
	MAL_HSPEC_STM32F0_AF_USART4_TX,
	MAL_HSPEC_STM32F0_AF_USART4_RX,
	MAL_HSPEC_STM32F0_AF_USART3_CTS,
	MAL_HSPEC_STM32F0_AF_CRS_SYNC,
	MAL_HSPEC_STM32F0_AF_CAN_RX,
	MAL_HSPEC_STM32F0_AF_CAN_TX,
	MAL_HSPEC_STM32F0_AF_USB_NOE,
	MAL_HSPEC_STM32F0_AF_USART4_RTS,
	MAL_HSPEC_STM32F0_AF_USART3_CK,
	MAL_HSPEC_STM32F0_AF_USART3_RTS,
	MAL_HSPEC_STM32F0_AF_USART4_CTS,
	MAL_HSPEC_STM32F0_AF_I2S2_WS,
	MAL_HSPEC_STM32F0_AF_USART3_TX,
	MAL_HSPEC_STM32F0_AF_I2S2_CK,
	MAL_HSPEC_STM32F0_AF_USART3_RX,
	MAL_HSPEC_STM32F0_AF_I2S2_MCK,
	MAL_HSPEC_STM32F0_AF_I2S2_SD,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO1,
	MAL_HSPEC_STM32F0_AF_USART4_CK,
	MAL_HSPEC_STM32F0_AF_TIM3_ETR,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO1,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO2,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO3,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO4,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO1,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO2,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO3,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO4,
	MAL_HSPEC_STM32F0_AF_USART6_TX,
	MAL_HSPEC_STM32F0_AF_USART6_RX,
	MAL_HSPEC_STM32F0_AF_SCL,
	MAL_HSPEC_STM32F0_AF_SDA,
	MAL_HSPEC_STM32F0_AF_USART5_TX,
	MAL_HSPEC_STM32F0_AF_USART5_RX,
	MAL_HSPEC_STM32F0_AF_USART5_CK_RTS,
	MAL_HSPEC_STM32F0_AF_TIM15
} mal_hspec_stm32f0_af_e;

#define MAL_HSPEC_STM32F0_GPIO_PORT_SIZE	16
#define MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE	8
#define MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE	2

#include "stm32f0/stm32f0xx_gpio.h"
#include "gpio/mal_gpio_definitions.h"
#include "timer/mal_timer.h"


#define MAL_HSPEC_STM32F0_GET_GPIO_PIN(pin) ((uint32_t)1 << pin)

GPIO_TypeDef* mal_hspec_stm32f0_get_gpio_typedef(mal_gpio_port_e port);

uint32_t mal_hspec_stm32f0_get_rcc_gpio_port(mal_gpio_port_e port);

mal_error_e mal_hspec_stm32f0_get_pin_af(mal_gpio_port_e port, uint8_t pin, mal_hspec_stm32f0_af_e af,
										  uint8_t *function);

/**
 * @brief Get the supported alternate functions by pin for the current MCU.
 * Must be defined for every family.
 * @param afs Alternate functions.
 */
void mal_hspec_stm32f0_get_timer_afs(const mal_hspec_stm32f0_af_e (**afs)[MAL_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_TIMER_SIZE]);

mal_error_e mal_hspec_stm32f0_get_timer_af(mal_gpio_port_e port, uint8_t pin, mal_timer_e timer, uint8_t *function);

mal_error_e mal_hspec_stm32f0_get_port_afs(mal_gpio_port_e port, const mal_hspec_stm32f0_af_e (**afs)[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE]);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CMN_H_ */
