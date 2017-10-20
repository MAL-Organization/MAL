/*
 * mal_serial_buffer.c
 *
 *  Created on: Oct 19, 2017
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

#include "mal_serial_buffer.h"

static mal_error_e serial_tx_callback(mal_serial_port_e port, uint16_t *data);
static mal_error_e serial_rx_callback(mal_serial_port_e port, uint16_t data);

static mal_serial_buffer_handle_s *handles[MAL_SERIAL_PORT_SIZE];

mal_error_e mal_serial_buffer_init(mal_serial_buffer_handle_s *handle, mal_serial_buffer_init_s *init) {
    mal_error_e result;
    // Initialize tx circular buffer
    mal_circular_buffer_init(init->tx_buffer, sizeof(uint16_t), init->tx_buffer_size * 2, (mal_circular_buffer_s*)&handle->tx_buffer);
    // Initialize rx circular buffer
    mal_circular_buffer_init(init->rx_buffer, sizeof(uint16_t), init->rx_buffer_size * 2, (mal_circular_buffer_s*)&handle->rx_buffer);
    // Save interface
    handle->port = init->port;
    handles[handle->port] = handle;
    // Initialize port
    mal_serial_init_s serial_init;
    serial_init.port = init->port;
    serial_init.rx_gpio = init->rx_gpio;
    serial_init.tx_gpio = init->tx_gpio;
    serial_init.baudrate = init->baudrate;
    serial_init.data_size = init->data_size;
    serial_init.parity = init->parity;
    serial_init.stop_bits = init->stop_bits;
    serial_init.rx_callback = &serial_rx_callback;
    serial_init.tx_callback = &serial_tx_callback;
    result = mal_serial_init(&serial_init);
    if (MAL_ERROR_OK != result) {
        return result;
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_serial_buffer_write(mal_serial_buffer_handle_s *handle, uint16_t data) {
    mal_error_e result;
    bool active = mal_serial_disable_interrupt(handle->port);
    result = mal_serial_transfer(handle->port, data);
    if (MAL_ERROR_HARDWARE_UNAVAILABLE == result) {
        // Port is busy writing, write to buffer
        result = mal_circular_buffer_write((mal_circular_buffer_s*)&handle->tx_buffer, &data);
    }
    mal_serial_enable_interrupt(handle->port, active);

    return result;
}

mal_error_e mal_serial_buffer_read(mal_serial_buffer_handle_s *handle, uint16_t *data) {
    mal_error_e result;
    bool active = mal_serial_disable_interrupt(handle->port);
    result = mal_circular_buffer_read((mal_circular_buffer_s*)&handle->rx_buffer, data);
    mal_serial_enable_interrupt(handle->port, active);

    return result;
}

static mal_error_e serial_tx_callback(mal_serial_port_e port, uint16_t *data) {
    mal_error_e result;
    result = mal_circular_buffer_read((mal_circular_buffer_s*)&handles[port]->tx_buffer, data);
    if (MAL_ERROR_OK != result) {
        return MAL_ERROR_EMPTY;
    }
    return MAL_ERROR_OK;
}

static mal_error_e serial_rx_callback(mal_serial_port_e port, uint16_t data) {
    return mal_circular_buffer_write((mal_circular_buffer_s*)&handles[port]->rx_buffer, &data);
}
