/**
 * @file mal_error.h
 * @author Olivier Allaire
 * @date May 3 2015
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
 * @brief Defines possible error codes returned by all functions in MAL.
 */

#ifndef STD_MAL_ERNO_H_
#define STD_MAL_ERNO_H_

/**
 * Possible error codes returned by all MAL functions.
 */
typedef enum {
	MAL_ERROR_OK = 0,                   //!< MAL_ERROR_OK
	MAL_ERROR_HARDWARE_INVALID = -1,    //!< MAL_ERROR_HARDWARE_INVALID
	MAL_ERROR_CLOCK_ERROR = -2,         //!< MAL_ERROR_CLOCK_ERROR
	MAL_ERROR_HARDWARE_UNAVAILABLE = -3,//!< MAL_ERROR_HARDWARE_UNAVAILABLE
	MAL_ERROR_INIT_FAILED = -4,         //!< MAL_ERROR_INIT_FAILED
	MAL_ERROR_FULL = -5,                //!< MAL_ERROR_FULL
	MAL_ERROR_EMPTY = -6,               //!< MAL_ERROR_EMPTY
	MAL_ERROR_USER = -7                 //!< MAL_ERROR_USER
} mal_error_e;

#endif /* STD_MAL_ERNO_H_ */
