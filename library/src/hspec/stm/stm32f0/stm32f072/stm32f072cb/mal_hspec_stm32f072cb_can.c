/*
 * mal_hspec_stm32f072cb_can.c
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

#include "mal_hspec_stm32f072cb_can.h"

static const mal_hspec_gpio_s valid_can1_tx_gpios[] = {
	{
		.pin = 12,
		.port = MAL_HSPEC_GPIO_PORT_A
	},
	{
		.pin = 9,
		.port = MAL_HSPEC_GPIO_PORT_B
	}
};

static const mal_hspec_gpio_s valid_can1_rx_gpios[] = {
	{
		.pin = 11,
		.port = MAL_HSPEC_GPIO_PORT_A
	},
	{
		.pin = 8,
		.port = MAL_HSPEC_GPIO_PORT_B
	}
};

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
