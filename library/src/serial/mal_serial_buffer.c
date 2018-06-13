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

static mal_error_e serial_tx_callback(void *handle, uint16_t *data);
static mal_error_e serial_rx_callback(void *handle, uint16_t data);

mal_error_e mal_serial_buffer_init(mal_serial_buffer_handle_s *handle, mal_serial_buffer_init_s *init) {
    mal_error_e result;
    // Initialize tx circular buffer
    mal_circular_buffer_init(init->tx_buffer, sizeof(uint16_t), init->tx_buffer_size * 2, (mal_circular_buffer_s*)&handle->tx_buffer);
    // Initialize rx circular buffer
    mal_circular_buffer_init(init->rx_buffer, sizeof(uint16_t), init->rx_buffer_size * 2, (mal_circular_buffer_s*)&handle->rx_buffer);
    // Initialize port
    mal_serial_init_s serial_init;
    serial_init.port = init->port;
    serial_init.rx_port = init->rx_port;
    serial_init.rx_pin = init->rx_pin;
    serial_init.tx_port = init->tx_port;
    serial_init.tx_pin = init->tx_pin;
    serial_init.baudrate = init->baudrate;
    serial_init.data_size = init->data_size;
    serial_init.parity = init->parity;
    serial_init.stop_bits = init->stop_bits;
    serial_init.rx_callback = &serial_rx_callback;
    serial_init.rx_callback_handle = handle;
    serial_init.tx_callback = &serial_tx_callback;
    serial_init.tx_callback_handle = handle;
    result = mal_serial_init(&handle->serial_handle, &serial_init);
    if (MAL_ERROR_OK != result) {
        return result;
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_serial_buffer_write(mal_serial_buffer_handle_s *handle, uint16_t data) {
    mal_error_e result;
    mal_serial_interrupt_state_s state;
    mal_serial_disable_interrupt(&handle->serial_handle, &state);
    result = mal_serial_transfer(&handle->serial_handle, data);
    if (MAL_ERROR_HARDWARE_UNAVAILABLE == result) {
        // Port is busy writing, write to buffer
        result = mal_circular_buffer_write((mal_circular_buffer_s*)&handle->tx_buffer, &data);
    }
    mal_serial_restore_interrupt(&handle->serial_handle, &state);

    return result;
}

mal_error_e mal_serial_buffer_read(mal_serial_buffer_handle_s *handle, uint16_t *data) {
    mal_error_e result;
    mal_serial_interrupt_state_s state;
    mal_serial_disable_interrupt(&handle->serial_handle, &state);
    result = mal_circular_buffer_read((mal_circular_buffer_s*)&handle->rx_buffer, data);
    mal_serial_restore_interrupt(&handle->serial_handle, &state);

    return result;
}

static mal_error_e serial_tx_callback(void *handle, uint16_t *data) {
    mal_error_e result;
    mal_serial_buffer_handle_s *buffer = handle;
    result = mal_circular_buffer_read((mal_circular_buffer_s*)&buffer->tx_buffer, data);
    if (MAL_ERROR_OK != result) {
        return MAL_ERROR_EMPTY;
    }
    return MAL_ERROR_OK;
}

static mal_error_e serial_rx_callback(void *handle, uint16_t data) {
    mal_serial_buffer_handle_s *buffer = handle;
    return mal_circular_buffer_write((mal_circular_buffer_s*)&buffer->rx_buffer, &data);
}

uint64_t mal_serial_buffer_get_rx_size(mal_serial_buffer_handle_s *handle) {
    uint64_t result;
    mal_serial_interrupt_state_s state;
    mal_serial_disable_interrupt(&handle->serial_handle, &state);
    result = handle->rx_buffer.size;
    mal_serial_restore_interrupt(&handle->serial_handle, &state);

    return result;
}

mal_error_e mal_serial_buffer_peek(mal_serial_buffer_handle_s *handle, uint64_t index, uint16_t *data) {
    mal_error_e result;
    mal_serial_interrupt_state_s state;
    mal_serial_disable_interrupt(&handle->serial_handle, &state);
    result = mal_circular_buffer_peek((mal_circular_buffer_s*)&handle->rx_buffer, index, data);
    mal_serial_restore_interrupt(&handle->serial_handle, &state);

    return result;
}
