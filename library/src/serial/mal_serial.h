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
#include "gpio/mal_gpio_definitions.h"
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
    MAL_SERIAL_PORT_5 = 4,  //!< Port 4
    MAL_SERIAL_PORT_6 = 5,  //!< Port 4
    MAL_SERIAL_PORT_7 = 6,  //!< Port 4
    MAL_SERIAL_PORT_8 = 7,  //!< Port 4
    MAL_SERIAL_PORT_SIZE = 8//!< Number of possible ports
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
 * Serial handle that must be defined by hardware specific implementation. Used
 * to access the serial functions.
 */
typedef struct MAL_SERIAL mal_serial_s;

/**
 * @brief Callback on byte transmitted.
 * @param handle The given handle during initialization.
 * @param data The next data to transfer.
 * @return Return a status once you executed your callback. If the value is not
 * #MAL_ERROR_OK, it is assumed that data must not be sent.
 */
typedef mal_error_e (*mal_serial_tx_callback_t)(void *handle, uint16_t *data);

/**
 * @brief Callback on byte received.
 * @param handle The given handle during initialization.
 * @param data The data received.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_serial_rx_callback_t)(void *handle, uint16_t data);

/**
 * Parameters to initialize a serial port.
 */
typedef struct {
    mal_serial_port_e port; /**< The port to initialize.*/
    mal_gpio_port_e rx_port; /**< The port of the RX GPIO.*/
    uint8_t rx_pin; /**< The pin of the port of the RX GPIO.*/
    mal_gpio_port_e tx_port; /**< The port of the TX GPIO.*/
    uint8_t tx_pin; /**< The pin of the port of the TX GPIO.*/
    uint64_t baudrate; /**< The baudrate.*/
    mal_serial_data_size_e data_size; /**< The word size.*/
    mal_serial_stop_bits_e stop_bits; /**< Number of stop bits.*/
    mal_serial_parity_e parity; /**< The parity setting.*/
    mal_serial_tx_callback_t tx_callback; /**< Transmit completed callback.*/
    void *tx_callback_handle; /**< Will be given during the execution of tx_callback.*/
    mal_serial_rx_callback_t rx_callback; /**< Receive completed callback.*/
    void *rx_callback_handle; /**< Will be given during the execution of rx_callback.*/
} mal_serial_init_s;

/**
 * This structure is used to retain interrupt status between disable and
 * enable. Must be defined by the hardware specific implementation.
 */
typedef struct MAL_SERIAL_INTERRUPT_STATE mal_serial_interrupt_state_s;

/**
 * @brief Initialize the given serial interface with the given parameters.
 * @param handle The handle to use for serial functions.
 * @param init Initialization parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_serial_init(mal_serial_s *handle, mal_serial_init_s *init);

/**
 * @brief Dispose of the serial interface to free the hardware.
 * @param handle The serial interface to dispose of.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_serial_dispose(mal_serial_s *handle);

/**
 * @brief Send data on the given port. Note that this is not a blocking call.
 * Use the callback to get the result.
 * @param handle The port to use.
 * @param data The data to send.
 * @return @MAL_ERROR_OK on success. If the port is busy, returns
 * #MAL_ERROR_HARDWARE_UNAVAILABLE.
 */
mal_error_e mal_serial_transfer(mal_serial_s *handle, uint16_t data);

/**
 * @brief Disable a serial port interrupt.
 * @param handle The port to disable the interrupt from.
 * @param state The state to use to restore interrupts.
 */
MAL_DEFS_INLINE void mal_serial_disable_interrupt(mal_serial_s *handle, mal_serial_interrupt_state_s *state);

/**
 * @brief Enable a serial port interrupt.
 * @param handle The port to enable the interrupt from.
 * @param state The state given by the disable function.
 */
MAL_DEFS_INLINE void mal_serial_restore_interrupt(mal_serial_s *handle, mal_serial_interrupt_state_s *state);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* SERIAL_MAL_SERIAL_H_ */
