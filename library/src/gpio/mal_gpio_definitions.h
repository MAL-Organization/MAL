/**
 * @file mal_gpio_definitions.h
 * @author Olivier Allaire
 * @date May 17 2018
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

#ifndef MAL_MAL_GPIO_DEFINITIONS_H
#define MAL_MAL_GPIO_DEFINITIONS_H

/**
 * @addtogroup GPIO
 * @{
 */

/**
 * Possible GPIO ports.
 */
typedef enum {
    MAL_GPIO_PORT_A = 0,  //!< MAL_PORT_A
    MAL_GPIO_PORT_B = 1,  //!< MAL_PORT_B
    MAL_GPIO_PORT_C = 2,  //!< MAL_PORT_C
    MAL_GPIO_PORT_D = 3,  //!< MAL_PORT_D
    MAL_GPIO_PORT_E = 4,  //!< MAL_PORT_E
    MAL_GPIO_PORT_F = 5,  //!< MAL_PORT_F
    MAL_GPIO_PORT_G = 6,  //!< MAL_PORT_G
    MAL_GPIO_PORT_H = 7,  //!< MAL_PORT_H
    MAL_GPIO_PORT_I = 8,  //!< MAL_PORT_I
    MAL_GPIO_PORT_SIZE = 9 //!< MAL_PORT_SIZE
} mal_gpio_port_e;

/**
 * @brief GPIO event callback.
 * @param handle The handle given during initialization.
 */
typedef void (*mal_gpio_event_callback_t)(void *handle);

/** @}*/

#endif //MAL_MAL_GPIO_DEFINITIONS_H
