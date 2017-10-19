/**
 * @file mal_serial.h
 * @author Olivier Allaire
 * @date May 30 2015
 * @copyright Copyright (c) 2015-2017 Olivier Allaire
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
 * @brief Functions to access and configure the serial interfaces of the MCU.
 */

#ifndef SERIAL_MAL_SERIAL_H_
#define SERIAL_MAL_SERIAL_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "gpio/mal_gpio.h"
#include "std/mal_defs.h"

/**
 * @defgroup Serial
 * @brief @copybrief mal_serial.h
 * @{
 */

/**
 * Possible serial ports.
 */
typedef enum {
    MAL_SERIAL_PORT_1 = 0,  //!< Port 1
    MAL_SERIAL_PORT_2 = 1,  //!< Port 2
    MAL_SERIAL_PORT_3 = 2,  //!< Port 3
    MAL_SERIAL_PORT_4 = 3,  //!< Port 4
    MAL_SERIAL_PORT_SIZE = 4//!< Number of possible ports
} mal_serial_port_e;

/**
 * Possible word width.
 */
typedef enum {
    MAL_SERIAL_DATA_7_BITS,//!< 7 bit words
    MAL_SERIAL_DATA_8_BITS,//!< 8 bit words
    MAL_SERIAL_DATA_9_BITS //!< 9 bit words
} mal_serial_data_size_e;

/**
 * Possible number of stop bits.
 */
typedef enum {
    MAL_SERIAL_STOP_BITS_1,//!< 1 stop bit
    MAL_SERIAL_STOP_BITS_2 //!< 2 stop bits
} mal_serial_stop_bits_e;

/**
 * Possible parity values.
 */
typedef enum {
    MAL_SERIAL_PARITY_EVEN, //!< With parity
    MAL_SERIAL_PARITY_ODD,  //!< With parity
    MAL_SERIAL_PARITY_NONE  //!< Without parity
} mal_serial_parity_e;

/**
 * @brief Callback on byte transmitted.
 * @param data The next data to transfer.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_serial_tx_callback_t)(uint16_t *data);

/**
 * @brief Callback on byte received.
 * @param data The data received..
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_serial_rx_callback_t)(uint16_t data);

/**
 * Parameters to initialize a serial port.
 */
typedef struct {
    mal_serial_port_e port; /**< The port to initialize.*/
    const mal_gpio_s *rx_gpio; /**< The GPIO for the rx pin.*/
    const mal_gpio_s *tx_gpio; /**< The GPIO for the tx pin.*/
    uint64_t baudrate; /**< The baudrate.*/
    mal_serial_data_size_e data_size; /**< The word size.*/
    mal_serial_stop_bits_e stop_bits; /**< Number of stop bits.*/
    mal_serial_parity_e parity; /**< The parity setting.*/
    mal_serial_tx_callback_t tx_callback; /**< Transmit completed callback.*/
    mal_serial_rx_callback_t rx_callback; /**< Receive completed callback.*/
} mal_serial_init_s;

/**
 * @brief Initialize the given serial interface with the given parameters.
 * @param init Initialization parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_serial_init(mal_serial_init_s *init);

/**
 * @brief Send data on the given port. Note that this is not a blocking call.
 * Use the callback to get the result.
 * @param port The port to use.
 * @param data The data to send.
 * @return @MAL_ERROR_OK on success. If the port is busy, returns
 * #MAL_ERROR_HARDWARE_UNAVAILABLE.
 */
mal_error_e mal_serial_transfer(mal_serial_port_e port, uint16_t data);

/**
 * @brief Disable a serial port interrupt.
 * @param port The port to disable the interrupt from.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_serial_disable_interrupt(mal_serial_port_e port);

/**
 * @brief Enable a serial port interrupt.
 * @param port The port to enable the interrupt from.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 */
MAL_DEFS_INLINE void mal_serial_enable_interrupt(mal_serial_port_e port, bool active);

/**
 * @}
 */

#endif /* SERIAL_MAL_SERIAL_H_ */
