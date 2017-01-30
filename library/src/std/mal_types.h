/**
 * @file mal_types.h
 * @author Olivier Allaire
 * @date January 30 2017
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
 * @brief Defines types used by multiple modules of MAL.
 */
#ifndef STD_MAL_TYPES_H_
#define STD_MAL_TYPES_H_

/**
 * This type is used to represent a value in volts. If MAL_FLOAT is
 * defined, this type defines volts directly using a float. Otherwise, it
 * contains millivolts. For example, let's take a value of 1.8V. If
 * MAL_FLOAT is defined, the value will be 1.8f. If not, the value will be
 * 1800.
 */
#ifdef MAL_FLOAT
typedef float mal_volts_t;
#else
typedef int64_t mal_volts_t;
#endif

/**
 * Macro to convert a mal_volts_t value to millivolts. This will always convert
 * 1.8V to 1800 regardless of if MAL_FLOAT is defined.
 */
#ifdef MAL_FLOAT
#define MAL_TYPES_MAL_VOLTS_TO_MILLIVOLTS(value)	((int64_t)(value * 1000.0f))
#else
#define MAL_TYPES_MAL_VOLTS_TO_MILLIVOLTS(value)	(value)
#endif

/**
 * Macro to convert a mal_volts_t value to volts. This will always convert
 * 1.8V to 1.8f regardless of if MAL_FLOAT is defined.
 */
#ifdef MAL_FLOAT
#define MAL_TYPES_MAL_VOLTS_TO_VOLTS(value)			(value)
#else
#define MAL_TYPES_MAL_VOLTS_TO_VOLTS(value)			((float)value / 1000.0f)
#endif

/**
 * Macro to convert a volts (float) to mal_volts_t. This will always convert
 * 1.8V (1.8f) to the correct mal_volts_t type.
 */
#ifdef MAL_FLOAT
#define MAL_TYPES_VOLTS_TO_MAL_VOLTS(value)			(value)
#else
#define MAL_TYPES_VOLTS_TO_MAL_VOLTS(value)			((mal_volts_t)(value * 1000.0f))
#endif

/**
 * Macro to convert a millivolts (int) to mal_volts_t. This will always convert
 * 1.8V (1800) to the correct mal_volts_t type.
 */
#ifdef MAL_FLOAT
#define MAL_TYPES_VOLTS_TO_MAL_VOLTS(value)			((float)value / 1000.0f)
#else
#define MAL_TYPES_VOLTS_TO_MAL_VOLTS(value)			(value)
#endif

#endif /* STD_MAL_TYPES_H_ */
