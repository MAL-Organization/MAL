/*
 * mal_hspec_stm32f072cb_timer.c
 *
 *  Created on: Sep 7, 2016
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

#include "mal_hspec_stm32f072cb_timer.h"
#include "std/mal_stdlib.h"

static const mal_hspec_gpio_s valid_timer1_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 7
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 0
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 1
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 13
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 14
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 15
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 8
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 9
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 10
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 11
	}
};

static const mal_hspec_gpio_s valid_timer1_input_capture_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 8
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 9
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 10
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 11
	}
};

static const mal_hspec_gpio_s valid_timer2_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 1
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 2
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 3
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 10
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 11
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 3
	}
};

static const mal_hspec_gpio_s valid_timer3_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 6
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 7
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 0
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 1
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 4
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 5
	}
};

static const mal_hspec_gpio_s valid_timer14_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 4
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 7
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 1
	}
};

static const mal_hspec_gpio_s valid_timer15_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 1
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 2
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 3
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 14
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 15
	}
};

static const mal_hspec_gpio_s valid_timer15_input_capture_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 2
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 3
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 14
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 15
	}
};

static const mal_hspec_gpio_s valid_timer16_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 6
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 6
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 8
	}
};

static const mal_hspec_gpio_s valid_timer16_input_capture_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 6
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 8
	}
};

static const mal_hspec_gpio_s valid_timer17_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 7
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 7
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 9
	}
};

static const mal_hspec_gpio_s valid_timer17_input_capture_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 7
	},
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 9
	}
};

static const mal_hspec_stm32f0_af_e timer_channels_afs[MAL_HSPEC_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_TIMER_SIZE] = {
		{ // Port A
			{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PA0
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM2_CH2, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM15_CH1N},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM2_CH3, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM15_CH1},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM2_CH4, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM15_CH2},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM14_CH1},
			{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PA5
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM3_CH1, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM16_CH1},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH1N, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM3_CH2, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM14_CH1, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM17_CH1},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH1},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH2},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH3},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH4},
			{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PA12
			{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PA13
			{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PA14
			{MAL_HSPEC_STM32F0_AF_NONE} // No timer channels on PA15
		},
		{ // Port B
			{MAL_HSPEC_STM32F0_AF_TIM1_CH2N, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM3_CH3},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH3N, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM3_CH4, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM14_CH1},
			{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PB2
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM2_CH2},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM3_CH1},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM3_CH2},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM16_CH1N},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM17_CH1N},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM16_CH1},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM17_CH1},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM2_CH3},
			{MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM2_CH4},
			{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PB12
			{MAL_HSPEC_STM32F0_AF_TIM1_CH1N},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH2N, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM15_CH1},
			{MAL_HSPEC_STM32F0_AF_TIM1_CH3N, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_NONE, MAL_HSPEC_STM32F0_AF_TIM15_CH2}
		},
		{ // PORT C
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC0
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC1
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC2
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC3
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC4
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC5
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC6
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC7
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC8
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC9
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC10
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC11
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC12
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC13
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PC14
				{MAL_HSPEC_STM32F0_AF_NONE} // No timer channels on PC15
		},
		{ // PORT D
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD0
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD1
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD2
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD3
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD4
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD5
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD6
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD7
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD8
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD9
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD10
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD11
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD12
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD13
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PD14
				{MAL_HSPEC_STM32F0_AF_NONE} // No timer channels on PD15
		},
		{ // PORT E
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE0
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE1
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE2
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE3
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE4
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE5
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE6
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE7
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE8
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE9
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE10
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE11
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE12
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE13
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PE14
				{MAL_HSPEC_STM32F0_AF_NONE} // No timer channels on PE15
		},
		{ // PORT F
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF0
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF1
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF2
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF3
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF4
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF5
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF6
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF7
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF8
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF9
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF10
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF11
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF12
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF13
				{MAL_HSPEC_STM32F0_AF_NONE}, // No timer channels on PF14
				{MAL_HSPEC_STM32F0_AF_NONE} // No timer channels on PF15
		}
};

mal_error_e mal_hspec_stm32f072cb_get_valid_channel_ios(mal_hspec_timer_e timer, const mal_hspec_gpio_s **ios, uint8_t *size) {
	switch (timer) {
		case MAL_HSPEC_TIMER_1:
			*ios = valid_timer1_gpios;
			*size = sizeof(valid_timer1_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_2:
			*ios = valid_timer2_gpios;
			*size = sizeof(valid_timer2_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_3:
			*ios = valid_timer3_gpios;
			*size = sizeof(valid_timer3_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_14:
			*ios = valid_timer14_gpios;
			*size = sizeof(valid_timer14_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_15:
			*ios = valid_timer15_gpios;
			*size = sizeof(valid_timer15_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_16:
			*ios = valid_timer16_gpios;
			*size = sizeof(valid_timer16_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_17:
			*ios = valid_timer17_gpios;
			*size = sizeof(valid_timer17_gpios) / sizeof(mal_hspec_gpio_s);
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

mal_error_e mal_hspec_stm32f072cb_get_valid_input_capture_ios(mal_hspec_timer_e timer, const mal_hspec_gpio_s **ios, uint8_t *size) {
	switch (timer) {
		case MAL_HSPEC_TIMER_1:
			*ios = valid_timer1_input_capture_gpios;
			*size = sizeof(valid_timer1_input_capture_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_2:
			// All available channels are valid input capture IOs
			*ios = valid_timer2_gpios;
			*size = sizeof(valid_timer2_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_3:
			// All available channels are valid input capture IOs
			*ios = valid_timer3_gpios;
			*size = sizeof(valid_timer3_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_14:
			// All available channels are valid input capture IOs
			*ios = valid_timer14_gpios;
			*size = sizeof(valid_timer14_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_15:
			*ios = valid_timer15_input_capture_gpios;
			*size = sizeof(valid_timer15_input_capture_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_16:
			*ios = valid_timer16_input_capture_gpios;
			*size = sizeof(valid_timer16_input_capture_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_TIMER_17:
			*ios = valid_timer17_input_capture_gpios;
			*size = sizeof(valid_timer17_input_capture_gpios) / sizeof(mal_hspec_gpio_s);
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

void mal_hspec_stm32f072cb_get_timer_afs(const mal_hspec_stm32f0_af_e (**afs)[MAL_HSPEC_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_TIMER_SIZE]) {
	*afs = &timer_channels_afs;
}
