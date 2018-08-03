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

#include "std/mal_stdint.h"

/**
 * @defgroup TYPES
 * @brief @copybrief mal_types.h
 * @{
 */

/**
 * @cond 0
 * @internal
 * Macro to convert a type value to an int with 10^-3 representation. This will
 * always convert 1.8 to 1800 regardless of if MAL_FLOAT is defined.
 */
#ifdef MAL_FLOAT
#define __MAL_TYPES_TYPE_TO_MILLI(type, value)			((type)((value) * 1000.0f))
#else
#define __MAL_TYPES_TYPE_TO_MILLI(type, value)			(type)(value)
#endif
/**
 * @endcond
 */

/**
 * @cond 0
 * Macro to convert an int with 10^-3 representation to a float. This will
 * always convert 1.8 (1800) to 1.8f regardless of if MAL_FLOAT is defined.
 */
#ifdef MAL_FLOAT
#define __MAL_TYPES_TYPE_TO_FLOAT(value)					(value)
#else
#define __MAL_TYPES_TYPE_TO_FLOAT(value)					((float)value / 1000.0f)
#endif
/**
 * @endcond
 */

/**
 * @cond 0
 * Macro to convert a float to a type value which is an int with 10^-3
 * representation when MAL_FLOAT is not defined. This will always convert 1.8
 * (1.8f) to the correct type representation.
 */
#ifdef MAL_FLOAT
#define __MAL_TYPES_FLOAT_TO_TYPE(type, value)				(value)
#else
#define __MAL_TYPES_FLOAT_TO_TYPE(type, value)				((type)(value * 1000.0f))
#endif
/**
 * @endcond
 */

/**
 * @cond 0
 * Macro to convert an int with 10^-3 representation to a type value which is
 * an int with 10^-3 representation when MAL_FLOAT is not defined. This will
 * always convert 1.8 (1800) to the correct type representation.
 */
#ifdef MAL_FLOAT
#define __MAL_TYPES_MILLI_TO_TYPE(value)					((float)(value) / 1000.0f)
#else
#define __MAL_TYPES_MILLI_TO_TYPE(value)					(value)
#endif
/**
 * @endcond
 */

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
#define MAL_TYPES_MAL_VOLTS_TO_MILLIVOLTS(value)				__MAL_TYPES_TYPE_TO_MILLI(uint64_t, value)

/**
 * Macro to convert a mal_volts_t value to volts. This will always convert
 * 1.8V to 1.8f regardless of if MAL_FLOAT is defined.
 */
#define MAL_TYPES_MAL_VOLTS_TO_VOLTS(value)						__MAL_TYPES_TYPE_TO_FLOAT(value)

/**
 * Macro to convert a volts (float) to mal_volts_t. This will always convert
 * 1.8V (1.8f) to the correct mal_volts_t type.
 */
#define MAL_TYPES_VOLTS_TO_MAL_VOLTS(value)						__MAL_TYPES_FLOAT_TO_TYPE(mal_volts_t, value)

/**
 * Macro to convert a millivolts (int) to mal_volts_t. This will always convert
 * 1.8V (1800) to the correct mal_volts_t type.
 */
#define MAL_TYPES_MILLIVOLTS_TO_MAL_VOLTS(value)				__MAL_TYPES_MILLI_TO_TYPE(value)

/**
 * Type for frequency values. When MAL_FLOAT is defined, the type is float,
 * otherwise it is int64_t. When MAL_FLOAT is defined, this type contains Hz.
 * Otherwise, this number contains mHz. For example here is a case where a
 * timer is initialized with a target frequency of 1 kHz with a delta 0.1 Hz.
 * When MAL_FLOAT is defined, frequency will be 1000.0f and delta will be 0.1f.
 * If MAL_FLOAT is not defined, frequency will be 1000000 and delta will be
 * 100.
 */
#ifdef MAL_FLOAT
typedef float mal_hertz_t;
#else
typedef int64_t mal_hertz_t;
#endif

/**
 * Macro to convert a mal_hertz_t value to millihertz. This will always convert
 * 1.8Hz to 1800 regardless of if MAL_FLOAT is defined.
 */
#define MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(value)				__MAL_TYPES_TYPE_TO_MILLI(uint64_t, value)

/**
 * Macro to convert a mal_hertz_t value to hertz. This will always convert
 * 1.8Hz to 1.8f regardless of if MAL_FLOAT is defined.
 */
#define MAL_TYPES_MAL_HERTZ_TO_HERTZ(value)						__MAL_TYPES_TYPE_TO_FLOAT(value)

/**
 * Macro to convert a hertz (float) to mal_hertz_t. This will always convert
 * 1.8Hz (1.8f) to the correct mal_hertz_t type.
 */
#define MAL_TYPES_HERTZ_TO_MAL_HERTZ(value)						__MAL_TYPES_FLOAT_TO_TYPE(mal_hertz_t, value)

/**
 * Macro to convert a millihertz (int) to mal_hertz_t. This will always convert
 * 1.8Hz (1800) to the correct mal_hertz_t type.
 */
#define MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(value)				__MAL_TYPES_MILLI_TO_TYPE(value)

/**
 * This type is used to represent ratio values. When MAL_FLOAT is defined,
 * accepted values are between 0.0f and 1.0f. Otherwise, this value is a
 * uint32_t. Ratio is scaled from 0 to 65535.
 */
#ifdef MAL_FLOAT
typedef float mal_ratio_t;
#else
typedef uint32_t mal_ratio_t;
#endif

/**
 * This type is used to normalize ratio values. When MAL_FLOAT is defined,
 * it is 1.0f. Otherwise, this value is 65535.
 */
#ifdef MAL_FLOAT
#define MAL_TYPES_RATIO_NORMALIZER								1.0f
#else
#define MAL_TYPES_RATIO_NORMALIZER								UINT16_MAX
#endif

/**
 * @brief Macro to compute the ratio of value regardless of if MAL_FLOAT is
 * defined.
 * @param ratio The ratio of type mal_ratio_t
 * @param value The value to compute a ratio of.
 * @return The result will be a float.
 */
#define MAL_TYPES_RATIO_OF_FLOAT_VALUE(ratio, value)			(((float)(ratio) * (float)(value)) / (float)MAL_TYPES_RATIO_NORMALIZER)

/**
 * @brief Macro to compute the ratio of value regardless of if MAL_FLOAT is
 * defined.
 * @param ratio The ratio of type mal_ratio_t
 * @param int_type The type of value. Must an int type.
 * @param value The value to compute a ratio of.
 * @return The result will be the given int_type.
 */
#ifdef MAL_FLOAT
#define MAL_TYPES_RATIO_OF_INT_VALUE(ratio, int_type, value)	((int_type)(((ratio) * (float)(value)) / MAL_TYPES_RATIO_NORMALIZER))
#else
#define	MAL_TYPES_RATIO_OF_INT_VALUE(ratio, int_type, value)	(((int_type)ratio * value) / (int_type)MAL_TYPES_RATIO_NORMALIZER)
#endif

/**
 * @brief Macro to convert a custom float ratio (ratio / norm) to a
 * mal_ratio_t.
 * @param ratio The custom ratio value of type float.
 * @param norm The normalizing value of the ratio. This value should be a
 * float. Ratio should be a value between 0 and norm.
 * @return The result will be a mal_ratio_t.
 */
#define MAL_TYPES_FLOAT_RATIO_TO_RATIO(ratio, norm)				((mal_ratio_t)(((ratio) * (float)MAL_TYPES_RATIO_NORMALIZER) / (norm)))

/**
 * @brief Macro to convert a custom int ratio (ratio / norm) to a
 * mal_ratio_t.
 * @param ratio The custom ratio value of type int. Note this can be any type
 * of int (int8_t, unint_16_t, etc.).
 * @param norm The normalizing value of the ratio. This value should be an int.
 * Note this can be any type of int (int8_t, unint_16_t, etc.). Ratio should be
 * a value between 0 and norm.
 * @return The result will be a mal_ratio_t.
 */
#ifdef MAL_FLOAT
#define MAL_TYPES_INT_RATIO_TO_RATIO(ratio, norm)				((mal_ratio_t)(((float)(ratio) * MAL_TYPES_RATIO_NORMALIZER) / (float)(norm)))
#else
#define MAL_TYPES_INT_RATIO_TO_RATIO(ratio, norm)				(((mal_ratio_t)ratio * MAL_TYPES_RATIO_NORMALIZER) / (mal_ratio_t)norm)
#endif

/**
 * @}
 */

#endif /* STD_MAL_TYPES_H_ */
