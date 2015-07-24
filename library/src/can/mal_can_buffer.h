/*
 * mal_can_buffer.h
 *
 *  Created on: Jul 5, 2015
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

#ifndef CAN_MAL_CAN_BUFFER_H_
#define CAN_MAL_CAN_BUFFER_H_

#include "hspec/mal_hspec.h"
#include "utils/mal_circular_buffer.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"

typedef struct {
	mal_hspec_can_e interface;
	volatile mal_circular_buffer_s tx_buffer;
	volatile mal_circular_buffer_s rx_buffer;
} mal_can_buffer_handle_s;

typedef struct {
	mal_hspec_can_e interface;
	mal_hspec_gpio_s *tx_gpio;
	mal_hspec_gpio_s *rx_gpio;
	uint64_t bitrate;
	uint8_t *rx_buffer;
	uint64_t rx_buffer_size;
	uint8_t *tx_buffer;
	uint64_t tx_buffer_size;
} mal_can_buffer_init_s;

mal_error_e mal_can_buffer_init(mal_can_buffer_handle_s *handle, mal_can_buffer_init_s *init);

mal_error_e mal_can_buffer_write(mal_can_buffer_handle_s *handle, mal_hspec_can_msg_s *msg);

mal_error_e mal_can_buffer_read(mal_can_buffer_handle_s *handle, mal_hspec_can_msg_s *msg);

#endif /* CAN_MAL_CAN_BUFFER_H_ */
