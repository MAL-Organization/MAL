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
#include "hspec/mal_hspec.h"

/**
 * @defgroup CAN
 * @brief @copybrief mal_can.h
 * @{
 */

/**
 * @brief Add a filter to a CAN interface.
 * @param interface The CAN interface to add the filter to. Should be of type
 * ::mal_hspec_can_e.
 * @param filter The filter of type ::mal_hspec_can_filter_s.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_can_add_filter(interface, filter) mal_hspec_can_add_filter(interface, filter)

/**
 * @brief Remove a filter from a CAN interface.
 * @param interface The CAN interface to remove the filter from. Should be of type
 * ::mal_hspec_can_e.
 * @param filter The filter of type ::mal_hspec_can_filter_s.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_can_remove_filter(interface, filter) mal_hspec_can_remove_filter(interface, filter)

/**
 * @brief Send a message on a CAN interface.
 * @param The CAN interface to send a message on. Should be of type
 * ::mal_hspec_can_e.
 * @param msg The message to send of type ::mal_hspec_can_msg_s.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_can_transmit(interface, msg) mal_hspec_can_transmit(interface, msg)

/**
 * @brief Disable interrupts for a CAN interface.
 * @param interface The interface to disable the interrupt. Should be of type
 * ::mal_hspec_can_e.
 * @return Returns true if interrupt was active before disabling it.
 */
#define mal_can_disable_interrupt(interface) mal_hspec_disable_can_interrupt(interface)

/**
 * @brief Enable interrupts for a CAN interface.
 * @param interface The interface to enable the interrupt.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
#define mal_can_enable_interrupt(interface, active) mal_hspec_enable_can_interrupt(interface, active)

/**
 * @brief Initialise a CAN interface.
 * @param init Initialisation parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_can_init(mal_hspec_can_init_s *init);

/**
 * @}
 */

#endif /* CAN_MAL_CAN_H_ */
