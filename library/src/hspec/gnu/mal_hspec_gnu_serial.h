/*
 * Copyright (c) 2018 Olivier Allaire
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

#ifndef HSPEC_GNU_MAL_HSPEC_GNU_SERIAL_H_
#define HSPEC_GNU_MAL_HSPEC_GNU_SERIAL_H_

#include "serial/mal_serial.h"
#include "utils/mal_circular_buffer.h"
#include "std/mal_stdint.h"

#define MAL_HSPEC_GNU_SERIAL_DATA_BUFFER_SIZE 100

typedef struct MAL_SERIAL {
    mal_serial_init_s init;
    mal_circular_buffer_s tx_circular_buffer;
    uint16_t data_buffer[MAL_HSPEC_GNU_SERIAL_DATA_BUFFER_SIZE];
} mal_serial_s;

typedef struct MAL_SERIAL_INTERRUPT_STATE {

} mal_serial_interrupt_state_s;

mal_error_e mal_hspec_gnu_serial_get_tx_data(mal_serial_s *handle, uint16_t *data);

mal_error_e mal_hspec_gnu_serial_push_rx_data(mal_serial_s *handle, uint16_t data);

#endif /* HSPEC_GNU_MAL_HSPEC_GNU_SERIAL_H_ */
