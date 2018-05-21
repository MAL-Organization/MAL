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

#include "mal_hspec_gnu_serial.h"
#include "std/mal_defs.h"

mal_error_e mal_serial_init(mal_serial_s *handle, mal_serial_init_s *init) {
    // Save init
    handle->init = *init;
    // Initialise circular buffer
    mal_circular_buffer_init((void*)handle->data_buffer,
                             sizeof(uint16_t),
                             sizeof(uint16_t) * MAL_HSPEC_GNU_SERIAL_DATA_BUFFER_SIZE,
                             &handle->tx_circular_buffer);
    return MAL_ERROR_OK;
}

mal_error_e mal_serial_transfer(mal_serial_s *handle, uint16_t data) {
    mal_error_e result;
    // Write to buffer
    result = mal_circular_buffer_write(&handle->tx_circular_buffer, &data);
    if (MAL_ERROR_OK != result) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    return result;
}

mal_error_e mal_hspec_gnu_serial_get_tx_data(mal_serial_s *handle, uint16_t *data) {
    mal_error_e result;
    // Remove message from buffer
    result = mal_circular_buffer_read(&handle->tx_circular_buffer, data);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Execute tx callback
    uint16_t next_data;
    result = handle->init.tx_callback(handle->init.tx_callback_handle, &next_data);
    if (MAL_ERROR_OK == result) {
        mal_serial_transfer(handle, next_data);
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_hspec_gnu_serial_push_rx_data(mal_serial_s *handle, uint16_t data) {
    return handle->init.rx_callback(handle->init.rx_callback_handle, data);
}

MAL_DEFS_INLINE void mal_serial_disable_interrupt(mal_serial_s *handle, mal_serial_interrupt_s *state) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(state);
}

MAL_DEFS_INLINE void mal_serial_enable_interrupt(mal_serial_s *handle, mal_serial_interrupt_s *state) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(state);
}
