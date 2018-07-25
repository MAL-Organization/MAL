/**
 * @file mal_can_definitions.h
 * @author Olivier Allaire
 * @date June 12 2018
 * @copyright Copyright (c) 2018 Olivier Allaire
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
 * @brief Hardware independent definitions for GPIOs.
 */

#ifndef MAL_MAL_CAN_DEFINITIONS_H
#define MAL_MAL_CAN_DEFINITIONS_H

#include "std/mal_stdint.h"

/**
 * @addtogroup CAN
 * @{
 */

/**
* The possible ID types.
*/
typedef enum {
    MAL_CAN_ID_STANDARD,//!< MAL_HSPEC_CAN_ID_STANDARD
    MAL_CAN_ID_EXTENDED //!< MAL_HSPEC_CAN_ID_EXTENDED
} mal_can_id_type_e;

/**
 * The variables of a CAN filter.
 */
typedef struct {
    uint32_t id; /**< The id of the filter.*/
    uint32_t mask; /**< The mask of the filter. Only bits of id matching with
                        bits equal to 1 of the filter will be considered.*/
    mal_can_id_type_e id_type; /**< The type of ID to filter.*/
} mal_can_filter_s;

/** @}*/

#endif //MAL_MAL_CAN_DEFINITIONS_H
