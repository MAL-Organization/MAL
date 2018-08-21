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

#include "serial/mal_serial_buffer.h"
#include "std/mal_bool.h"

#define BUFFER_SIZE 30

static mal_serial_buffer_handle_s serial_handle;
static uint16_t rx_buffer[BUFFER_SIZE];
static uint16_t tx_buffer[BUFFER_SIZE];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "OCDFAInspection"
int main(void) {
    mal_error_e result;

    mal_serial_buffer_init_s init;
    init.port = MAL_SERIAL_PORT_7;
    init.rx_port = MAL_GPIO_PORT_F;
    init.rx_pin = 6;
    init.tx_port = MAL_GPIO_PORT_F;
    init.tx_pin = 7;
    init.baudrate = 115200;
    init.data_size = MAL_SERIAL_DATA_8_BITS;
    init.stop_bits = MAL_SERIAL_STOP_BITS_1;
    init.parity = MAL_SERIAL_PARITY_NONE;
    init.rx_buffer = rx_buffer;
    init.rx_buffer_size = BUFFER_SIZE;
    init.tx_buffer = tx_buffer;
    init.tx_buffer_size = BUFFER_SIZE;
    result = mal_serial_buffer_init(&serial_handle, &init);
    if (MAL_ERROR_OK != result) {
        while (true);
    }

    while (true) {
        uint16_t data;
        result = mal_serial_buffer_read(&serial_handle, &data);
        if (MAL_ERROR_OK != result) {
            continue;
        }
        result = mal_serial_buffer_write(&serial_handle, data);
        if (MAL_ERROR_OK != result) {
            while (true);
        }
    }

    return 0;
}
#pragma clang diagnostic pop
