/*
 * mal_hspec_stm32f072cb_i2c.c
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

#include "mal_hspec_stm32f072cb_i2c.h"

static const mal_hspec_gpio_s valid_i2c1_scl_gpios[] = {
	{
		.pin = 6,
		.port = MAL_GPIO_PORT_B
	},
	{
		.pin = 8,
		.port = MAL_GPIO_PORT_B
	}
};

static const mal_hspec_gpio_s valid_i2c1_sda_gpios[] = {
	{
		.pin = 7,
		.port = MAL_GPIO_PORT_B
	},
	{
		.pin = 9,
		.port = MAL_GPIO_PORT_B
	}
};

static const mal_hspec_gpio_s valid_i2c2_scl_gpios[] = {
	{
		.pin = 10,
		.port = MAL_GPIO_PORT_B
	},
	{
		.pin = 13,
		.port = MAL_GPIO_PORT_B
	}
};

static const mal_hspec_gpio_s valid_i2c2_sda_gpios[] = {
	{
		.pin = 11,
		.port = MAL_GPIO_PORT_B
	},
	{
		.pin = 14,
		.port = MAL_GPIO_PORT_B
	}
};

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
