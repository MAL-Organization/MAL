/*
 * mal_hspec_stm32f072rb_adc.c
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

#include "mal_hspec_stm32f072rb_adc.h"
#include "std/mal_stdlib.h"

static const mal_hspec_gpio_s valid_adc0_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 0
	}
};

static const mal_hspec_gpio_s valid_adc1_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 1
	}
};

static const mal_hspec_gpio_s valid_adc2_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 2
	}
};

static const mal_hspec_gpio_s valid_adc3_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 3
	}
};

static const mal_hspec_gpio_s valid_adc4_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 4
	}
};

static const mal_hspec_gpio_s valid_adc5_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 5
	}
};

static const mal_hspec_gpio_s valid_adc6_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 6
	}
};

static const mal_hspec_gpio_s valid_adc7_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 7
	}
};

static const mal_hspec_gpio_s valid_adc8_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 0
	}
};

static const mal_hspec_gpio_s valid_adc9_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_B,
		.pin = 1
	}
};

static const mal_hspec_gpio_s valid_adc10_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_C,
		.pin = 0
	}
};

static const mal_hspec_gpio_s valid_adc11_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_C,
		.pin = 1
	}
};

static const mal_hspec_gpio_s valid_adc12_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_C,
		.pin = 2
	}
};

static const mal_hspec_gpio_s valid_adc13_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_C,
		.pin = 3
	}
};

static const mal_hspec_gpio_s valid_adc14_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_C,
		.pin = 4
	}
};

static const mal_hspec_gpio_s valid_adc15_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_C,
		.pin = 5
	}
};

mal_error_e mal_hspec_stm32f072rb_get_valid_adc_ios(mal_hspec_adc_e adc, const mal_hspec_gpio_s **ios, uint8_t *size) {
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
