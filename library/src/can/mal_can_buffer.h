/**
 * @file mal_can_buffer.h
 * @author Olivier Allaire
 * @date July 5 2015
 * @copyright Copyright (c) 2015 Olivier Allaire
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
 * @brief Functions to queue CAN messages for a CAN interface. This is useful
 * when multiple modules will use the same CAN interface.
 */

#ifndef CAN_MAL_CAN_BUFFER_H_
#define CAN_MAL_CAN_BUFFER_H_

#include "can/mal_can.h"
#include "utils/mal_circular_buffer.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"

/**
 * @addtogroup CAN
 * @{
 */

/**
 * A handle containing all the variables related to a buffer.
 */
typedef struct {
	mal_can_s *handle; /**< The CAN interface of the buffer.*/
	volatile mal_circular_buffer_s tx_buffer; /**< The transmit buffer.*/
	volatile mal_circular_buffer_s rx_buffer; /**< The receive buffer.*/
} mal_can_buffer_handle_s;

/**
 * Initialization parameters of the CAN buffer.
 */
typedef struct {
	mal_can_e interface; /**< The CAN interface to initialize and use.*/
	mal_gpio_port_e tx_port; /**< The port of the TX GPIO to initialize.*/
	uint8_t tx_pin; /**< The pin of the port of the TX GPIO to initialize. */
	mal_gpio_port_e rx_port; /**< The port of the RX GPIO to initialize.*/
	uint8_t rx_pin; /**< The pin of the port of the RX GPIO to initialize. */
	uint64_t bitrate; /**< The bitrate of the CAN bus.*/
	uint8_t *rx_buffer; /**< This should be an array of bytes in multiple of size of ::mal_hspec_can_msg_s.*/
	uint64_t rx_buffer_size; /**< The size of the rx buffer in bytes.*/
	uint8_t *tx_buffer; /**< This should be an array of bytes in multiple of size of ::mal_hspec_can_msg_s.*/
	uint64_t tx_buffer_size; /**< The size of the tx buffer in bytes.*/
} mal_can_buffer_init_s;

/**
 * @brief This function will uninitialize a CAN buffer.
 * @param handle The handle to the CAN buffer.
 */
#define mal_can_buffer_deinit(buffer) mal_can_deinit((buffer)->handle)

/**
 * @brief Initialize a CAN buffer and a CAN interface.
 * @param handle The handle of the CAN buffer.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_buffer_init(mal_can_buffer_init_s *init, mal_can_buffer_handle_s *buffer_handle,
								 mal_can_s *can_handle);

/**
 * @brief Initialize a CAN buffer and a CAN interface using direct initialization.
 * @param handle The handle of the CAN buffer.
 * @param init The initialization parameters.
 * @param direct_init A pointer to direct initialization parameter. See the
 * hardware specific implementation to know what type this should be.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_buffer_direct_init(mal_can_buffer_init_s *init, const void *direct_init,
                                       mal_can_buffer_handle_s *buffer_handle, mal_can_s *can_handle);


/**
 * @brief Write a message to a buffer.
 * @param handle The handle of the buffer to write to.
 * @param msg The message to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_buffer_write(mal_can_buffer_handle_s *handle, mal_can_msg_s *msg);

/**
 * @brief Read a message from a buffer.
 * @param handle The handle of the buffer to read from.
 * @param msg A pointer that will contain the message.
 * @return Returns #MAL_ERROR_OK on success. If the buffer is empty, the
 * function will return #MAL_ERROR_EMPTY.
 */
mal_error_e mal_can_buffer_read(mal_can_buffer_handle_s *handle, mal_can_msg_s *msg);

/**
 * @}
 */

#endif /* CAN_MAL_CAN_BUFFER_H_ */
