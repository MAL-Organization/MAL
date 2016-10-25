/*
 * mal_hspec_stm32f072_dac.c
 *
 *  Created on: Oct 25, 2016
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

#include "mal_hspec_stm32f072_dac.h"
#include "std/mal_stdlib.h"

static const mal_hspec_gpio_s valid_dac1_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 4
	}
};

static const mal_hspec_gpio_s valid_dac2_gpios[] = {
	{
		.port = MAL_HSPEC_GPIO_PORT_A,
		.pin = 5
	}
};

mal_error_e mal_hspec_stm32f072_get_valid_dac_ios(mal_hspec_adc_e dac,
												  const mal_hspec_gpio_s **ios,
												  uint8_t *size) {
	switch (dac) {
		case MAL_HSPEC_DAC_1:
			*ios = valid_dac1_gpios;
			*size = sizeof(valid_dac1_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_DAC_2:
			*ios = valid_dac2_gpios;
			*size = sizeof(valid_dac2_gpios) / sizeof(mal_hspec_gpio_s);
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
