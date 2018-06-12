/**
 * @file mal_can.h
 * @author Olivier Allaire
 * @date June 14 2015
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
 * @brief Functions to access and configure the CAN interfaces of the MCU.
 */

#ifndef CAN_MAL_CAN_H_
#define CAN_MAL_CAN_H_

#include "mal_can_definitions.h"
#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "gpio/mal_gpio_definitions.h"
#include "std/mal_defs.h"
#include "std/mal_bool.h"

/**
 * @defgroup CAN
 * @brief @copybrief mal_can.h
 * @{
 */

/**
* CAN handle that must be defined by hardware specific implementation. Used
* to access the CAN functions.
*/
typedef struct MAL_CAN mal_can_s;

/**
 * The maximum number of bytes possible in a CAN message.
 */
#define MAL_CAN_MAX_DATA_SIZE 8

/**
 * The possible CAN interfaces.
 */
typedef enum {
    MAL_CAN_1 = 0,  //!< MAL_HSPEC_CAN_1
    MAL_CAN_2 = 1,  //!< MAL_HSPEC_CAN_2
    MAL_CAN_3 = 2,  //!< MAL_HSPEC_CAN_3
    MAL_CAN_SIZE = 3//!< MAL_HSPEC_CAN_SIZE
} mal_can_e;

/**
 * The variables related to a CAN message.
 */
typedef struct {
    uint8_t data[MAL_CAN_MAX_DATA_SIZE]; /**< The array containing the data of a message.*/
    uint8_t size; /**< The numbers of valid bytes in the data array.*/
    uint32_t id; /**< The id of the message.*/
    mal_can_id_type_e id_type; /**< The type of the id.*/
} mal_can_msg_s;

/**
 * @brief This function will be called when a CAN message is transmitted.
 * @param handle The handle given for the callback.
 * @param next_msg The next message to send.
 * @return Return a status once you executed your callback. If #MAL_ERROR_OK is
 * returned, next_msg must point to a valid message. Use #MAL_ERROR_EMPTY if
 * there is no next message.
 */
typedef mal_error_e (*mal_can_tx_callback_t)(void *handle, mal_can_msg_s *next_msg);

/**
 * @brief This function will be called when a CAN message is received.
 * @param handle The handle given for the callback.
 * @param msg The message received.
 */
typedef void (*mal_can_rx_callback_t)(void *handle, mal_can_msg_s *msg);

/**
 * The parameters to initialize a CAN interface.
 */
typedef struct {
    mal_can_e interface; /**< The CAN interface to initialize.*/
    mal_gpio_port_e tx_port; /**< The port of the TX GPIO to initialize.*/
    uint8_t tx_pin; /**< The pin of the port of the TX GPIO to initialize. */
    mal_gpio_port_e rx_port; /**< The port of the RX GPIO to initialize.*/
    uint8_t rx_pin; /**< The pin of the port of the RX GPIO to initialize. */
    uint64_t bitrate; /**< The bitrate of the CAN bus.*/
    mal_can_tx_callback_t tx_callback; /**< The callback to be executed when a message is transmitted.*/
    void *tx_callback_handle; /**< The handle that will be passed to the TX callback.*/
    mal_can_rx_callback_t rx_callback; /**< The callback to be executed when a message is received.*/
    void *rx_callback_handle; /**< The handle that will be passed to the TX callback.*/
} mal_can_init_s;

/**
 * @brief Initialize a CAN interface.
 * @param init Initialization parameters.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_init(mal_can_init_s *init, mal_can_s *handle);

/**
 * @brief Function to initialize directly the interface. Using this function
 * will reduce code size at the cost of flexibility and safety. Usually use
 * this in a code closer to final.
 * @param init Initialization parameters.
 * @param direc_init A pointer to direct initialization parameter. See the
 * hardware specific implementation to know what type this should be.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_direct_init(mal_can_init_s *init, const void *direct_init, mal_can_s *handle);

/**
 * @brief This function will uninitialize the given interface.
 * @param handle Interface to disable.
 */
mal_error_e mal_can_deinit(mal_can_s *handle);

/**
 * @brief Add a filter to a CAN interface.
 * @param handle The CAN interface to add the filter to.
 * @param filter The filter to add.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_add_filter(mal_can_s *handle, mal_can_filter_s *filter);

/**
 * @brief Remove a filter from a CAN interface.
 * @param handle The CAN interface to remove the filter from.
 * @param filter The filter to remove.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_remove_filter(mal_can_s *handle, mal_can_filter_s *filter);

/**
 * @brief Send a message on a CAN interface.
 * @param handle The CAN interface to send a message on.
 * @param msg The message to send.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_transmit(mal_can_s *handle, mal_can_msg_s *msg);

/**
 * @brief Disable interrupts for a CAN interface.
 * @param handle The interface to disable the interrupt.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_can_disable_interrupt(mal_can_s *handle);

/**
 * @brief Enable interrupts for a CAN interface.
 * @param handle The interface to enable the interrupt.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 */
MAL_DEFS_INLINE void mal_can_set_interrupt(mal_can_s *handle, bool active);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* CAN_MAL_CAN_H_ */
