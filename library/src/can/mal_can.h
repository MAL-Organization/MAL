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

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "gpio/mal_gpio.h"
#include "std/mal_defs.h"
#include "std/mal_bool.h"

/**
 * @defgroup CAN
 * @brief @copybrief mal_can.h
 * @{
 */

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
    MAL_CAN_SIZE = 2//!< MAL_HSPEC_CAN_SIZE
} mal_can_e;

/**
 * The possible ID types.
 */
typedef enum {
    MAL_CAN_ID_STANDARD,//!< MAL_HSPEC_CAN_ID_STANDARD
    MAL_CAN_ID_EXTENDED //!< MAL_HSPEC_CAN_ID_EXTENDED
} mal_can_id_type_e;

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
 * @param interface The interface the message was transmitted on.
 * @param next_msg The next message to send.
 * @return Return a status once you executed your callback. If #MAL_ERROR_OK is
 * returned, next_msg will be transmitted. Otherwise, it will not.
 */
typedef mal_error_e (*mal_can_tx_callback_t)(mal_can_e interface, mal_can_msg_s *next_msg);

/**
 * @brief This function will be called when a CAN message is received.
 * @param interface The interface the message was received on.
 * @param next_msg The message received.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_can_rx_callback_t)(mal_can_e interface, mal_can_msg_s *msg);

/**
 * The parameters to initialize a CAN interface.
 */
typedef struct {
    mal_can_e interface; /**< The CAN interface to initialize.*/
    const mal_gpio_s *tx_gpio; /**< The GPIO of the tx pin.*/
    const mal_gpio_s *rx_gpio; /**< The GPIO of the rx pin.*/
    uint64_t bitrate; /**< The bitrate of the CAN bus.*/
    mal_can_tx_callback_t tx_callback; /**< The callback to be executed when a message is transmitted.*/
    mal_can_rx_callback_t rx_callback; /**< The callback to be executed when a message is received.*/
} mal_can_init_s;

/**
 * The variables of a CAN filter.
 */
typedef struct {
    uint32_t id; /**< The id of the filter.*/
    uint32_t mask; /**< The mask of the filter. Only bits of id matching with
                        bits equal to 1 of the filter will be considered.*/
    mal_can_id_type_e id_type; /**< The type of ID to filter.*/
} mal_can_filter_s;

/**
 * @brief Function to initialize directly the interface. Using this function
 * will reduce code size at the cost of flexibility and safety. Usually use
 * this in a code closer to final.
 * @param init Initialization parameters.
 * @param direc_init A pointer to direct initialization parameter. See the
 * hardware specific implementation to know what type this should be.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_direct_init(mal_can_init_s *init, const void *direct_init);

/**
 * @brief This function will uninitialize the given interface.
 * @param interface Interface to disable. Must be of type
 * mal_hspec_can_id_type_e.
 */
void mal_can_deinit(mal_can_e interface);

/**
 * @brief Add a filter to a CAN interface.
 * @param interface The CAN interface to add the filter to. Should be of type
 * ::mal_hspec_can_e.
 * @param filter The filter of type ::mal_hspec_can_filter_s.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_add_filter(mal_can_e interface, mal_can_filter_s *filter);

/**
 * @brief Remove a filter from a CAN interface.
 * @param interface The CAN interface to remove the filter from. Should be of type
 * ::mal_hspec_can_e.
 * @param filter The filter of type ::mal_hspec_can_filter_s.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_remove_filter(mal_can_e interface, mal_can_filter_s *filter);

/**
 * @brief Send a message on a CAN interface.
 * @param The CAN interface to send a message on. Should be of type
 * ::mal_hspec_can_e.
 * @param msg The message to send of type ::mal_hspec_can_msg_s.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_transmit(mal_can_e interface, mal_can_msg_s *msg);

/**
 * @brief Disable interrupts for a CAN interface.
 * @param interface The interface to disable the interrupt. Should be of type
 * ::mal_hspec_can_e.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_can_disable_interrupt(mal_can_e interface);

/**
 * @brief Enable interrupts for a CAN interface.
 * @param interface The interface to enable the interrupt.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
MAL_DEFS_INLINE void mal_can_enable_interrupt(mal_can_e interface, bool active);

/**
 * @brief Initialize a CAN interface.
 * @param init Initialization parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_init(mal_can_init_s *init);

/**
 * @}
 */

#endif /* CAN_MAL_CAN_H_ */
