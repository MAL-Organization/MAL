/*
 * mal_hspec_mingw_serial.c
 *
 *  Created on: Nov 6, 2017
 *      Author: Olivier
 */
/*
 * Copyright (c) 2017 Olivier Allaire
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

#include "mal_hspec_mingw_serial.h"
#include "utils/mal_circular_buffer.h"

#define MINGW_SERIAL_DATA_BUFFER_SIZE   100

typedef struct {
    mal_serial_init_s init;
    mal_circular_buffer_s tx_circular_buffer;
    uint16_t data_buffer[MINGW_SERIAL_DATA_BUFFER_SIZE];
} mingw_serial_interface_s;

static mingw_serial_interface_s serial_interfaces[MAL_SERIAL_PORT_SIZE];

mal_error_e mal_serial_init(mal_serial_init_s *init) {
    // Save init
    serial_interfaces[init->port].init = *init;
    // Initialise circular buffer
    mal_circular_buffer_init((void*)serial_interfaces[init->port].data_buffer,
                             sizeof(uint16_t),
                             sizeof(uint16_t) * MINGW_SERIAL_DATA_BUFFER_SIZE,
                             &serial_interfaces[init->port].tx_circular_buffer);
    return MAL_ERROR_OK;
}

mal_error_e mal_serial_transfer(mal_serial_port_e port, uint16_t data) {
    mal_error_e result;
    // Write to buffer
    result = mal_circular_buffer_write(&serial_interfaces[port].tx_circular_buffer, &data);
    if (MAL_ERROR_OK != result) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    return result;
}

mal_error_e mal_hspec_mingw_serial_get_tx_data(mal_serial_port_e port, uint16_t *data) {
    mal_error_e result;
    // Remove message from buffer
    result = mal_circular_buffer_read(&serial_interfaces[port].tx_circular_buffer, data);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Execute tx callback
    uint16_t next_data;
    result = serial_interfaces[port].init.tx_callback(port, &next_data);
    if (MAL_ERROR_OK == result) {
        mal_serial_transfer(port, next_data);
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_serial_push_rx_data(mal_serial_port_e port, uint16_t data) {
    return serial_interfaces[port].init.rx_callback(port, data);
}

MAL_DEFS_INLINE bool mal_serial_disable_interrupt(mal_serial_port_e port) {
    return false;
}

MAL_DEFS_INLINE void mal_serial_enable_interrupt(mal_serial_port_e port, bool active) {
    // Nothing to do
}
