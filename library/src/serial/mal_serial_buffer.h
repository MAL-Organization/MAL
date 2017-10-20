/**
 * @file mal_serial_buffer.h
 * @author Olivier Allaire
 * @date October 19 2017
 * @copyright Copyright (c) 2017 Olivier Allaire
 * @par This file is part of MAL.
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
 * @brief Functions to queue words for a serial port.
 */

#ifndef SERIAL_MAL_SERIAL_BUFFER_H_
#define SERIAL_MAL_SERIAL_BUFFER_H_

#include "mal_serial.h"
#include "std/mal_error.h"
#include "utils/mal_circular_buffer.h"

/**
 * @addtogroup Serial
 * @{
 */

/**
 * The initialization parameters of a serial buffer.
 */
typedef struct {
    mal_serial_port_e port; /**< The port to initialize.*/
    const mal_gpio_s *rx_gpio; /**< The GPIO for the rx pin.*/
    const mal_gpio_s *tx_gpio; /**< The GPIO for the tx pin.*/
    uint64_t baudrate; /**< The baudrate.*/
    mal_serial_data_size_e data_size; /**< The word size.*/
    mal_serial_stop_bits_e stop_bits; /**< Number of stop bits.*/
    mal_serial_parity_e parity; /**< The parity setting.*/
    uint16_t *rx_buffer; /**< The rx buffer array.*/
    uint64_t rx_buffer_size; /**< The size of the rx buffer in words.*/
    uint16_t *tx_buffer; /**< The tx buffer array.*/
    uint64_t tx_buffer_size; /**< The size of the tx buffer in words.*/
} mal_serial_buffer_init_s;

/**
 * A structure that contains all the necessary variables for the serial buffer.
 */
typedef struct {
    mal_serial_port_e port; /**< The serial port.*/
    volatile mal_circular_buffer_s rx_buffer; /**< A circular buffer.*/
    volatile mal_circular_buffer_s tx_buffer; /**< A circular buffer.*/
} mal_serial_buffer_handle_s;

/**
 * @brief Initialize a serial buffer and the given port.
 * @param handle The handle of the serial buffer.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_serial_buffer_init(mal_serial_buffer_handle_s *handle, mal_serial_buffer_init_s *init);


/**
 * @brief Write a write to the transmit buffer.
 * @param handle The handle of the buffer to write to.
 * @param data The data to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_serial_buffer_write(mal_serial_buffer_handle_s *handle, uint16_t data);

/**
 * @brief Read data from a buffer.
 * @param handle The handle of the buffer to read from.
 * @param data A pointer that will contain the data.
 * @return Returns #MAL_ERROR_OK on success. If the buffer is empty, the
 * function will return #MAL_ERROR_EMPTY.
 */
mal_error_e mal_serial_buffer_read(mal_serial_buffer_handle_s *handle, uint16_t *data);

/**
 * @}
 */

#endif /* SERIAL_MAL_SERIAL_BUFFER_H_ */
