/**
 * @file mal_power.h
 * @author Olivier Allaire
 * @date January 26 2017
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
 * @brief Functions to access power functions of the MCU.
 */

#ifndef POWER_MAL_POWER_H_
#define POWER_MAL_POWER_H_

#include "hspec/mal_hspec.h"
#include "std/mal_error.h"

/**
 * @defgroup POWER
 * @brief @copybrief mal_power.h
 * @{
 */

/**
 * @brief Get the voltage of a rail of the MCU.
 * @param rail The rail the get the voltage from. Must be of type
 * #mal_hspec_power_rail_e.
 * @param value A pointer of type #mal_hspec_power_rail_value_t to store the
 * rail voltage.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_power_get_rail_voltage(rail, value) mal_hspec_power_get_rail_voltage(rail, value)

/**
 * @}
 */

#endif /* POWER_MAL_POWER_H_ */
