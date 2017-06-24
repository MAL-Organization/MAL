/*
 * mal_hspec_stm32f072rb_spi.c
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

#include "mal_hspec_stm32f072rb_spi.h"
#include "std/mal_stdlib.h"

static const mal_hspec_gpio_s valid_spi1_miso_gpios[] = {
	{
		.port = MAL_GPIO_PORT_A,
		.pin = 6
	},
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 4
	}
};

static const mal_hspec_gpio_s valid_spi2_miso_gpios[] = {
	{
		.port = MAL_GPIO_PORT_C,
		.pin = 2
	},
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 14
	}
};

static const mal_hspec_gpio_s valid_spi1_mosi_gpios[] = {
	{
		.port = MAL_GPIO_PORT_A,
		.pin = 7
	},
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 5
	}
};

static const mal_hspec_gpio_s valid_spi2_mosi_gpios[] = {
	{
		.port = MAL_GPIO_PORT_C,
		.pin = 3
	},
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 15
	}
};

static const mal_hspec_gpio_s valid_spi1_clk_gpios[] = {
	{
		.port = MAL_GPIO_PORT_A,
		.pin = 5
	},
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 3
	}
};

static const mal_hspec_gpio_s valid_spi2_clk_gpios[] = {
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 10
	},
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 13
	}
};

static const mal_hspec_gpio_s valid_spi1_select_gpios[] = {
	{
		.port = MAL_GPIO_PORT_A,
		.pin = 4
	},
	{
		.port = MAL_GPIO_PORT_A,
		.pin = 15
	}
};

static const mal_hspec_gpio_s valid_spi2_select_gpios[] = {
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 12
	},
	{
		.port = MAL_GPIO_PORT_B,
		.pin = 9
	}
};

mal_error_e mal_hspec_stm32f072rb_get_valid_spi_ios(mal_hspec_spi_e interface,
													const mal_hspec_gpio_s **mosis,
													uint8_t *mosis_size,
													const mal_hspec_gpio_s **misos,
													uint8_t *misos_size,
													const mal_hspec_gpio_s **clks,
													uint8_t *clks_size,
													const mal_hspec_gpio_s **selects,
													uint8_t *selects_size) {
	switch (interface) {
		case MAL_HSPEC_SPI_1:
			*mosis = valid_spi1_mosi_gpios;
			*mosis_size = sizeof(valid_spi1_mosi_gpios) / sizeof(mal_hspec_gpio_s);
			*misos = valid_spi1_miso_gpios;
			*misos_size = sizeof(valid_spi1_miso_gpios) / sizeof(mal_hspec_gpio_s);
			*clks = valid_spi1_clk_gpios;
			*clks_size = sizeof(valid_spi1_clk_gpios) / sizeof(mal_hspec_gpio_s);
			*selects = valid_spi1_select_gpios;
			*selects_size = sizeof(valid_spi1_select_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		case MAL_HSPEC_SPI_2:
			*mosis = valid_spi2_mosi_gpios;
			*mosis_size = sizeof(valid_spi2_mosi_gpios) / sizeof(mal_hspec_gpio_s);
			*misos = valid_spi2_miso_gpios;
			*misos_size = sizeof(valid_spi2_miso_gpios) / sizeof(mal_hspec_gpio_s);
			*clks = valid_spi2_clk_gpios;
			*clks_size = sizeof(valid_spi2_clk_gpios) / sizeof(mal_hspec_gpio_s);
			*selects = valid_spi2_select_gpios;
			*selects_size = sizeof(valid_spi2_select_gpios) / sizeof(mal_hspec_gpio_s);
			break;
		default:
			*mosis = NULL;
			break;
	}
	if (NULL == *mosis) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}
