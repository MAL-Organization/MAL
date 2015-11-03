/**
 * @file mal_clock.h
 * @author Olivier Allaire
 * @date May 28 2015
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
 * @brief Functions to access the clocks of the MCU.
 */

#ifndef CLOCK_MAL_CLOCK_H_
#define CLOCK_MAL_CLOCK_H_

#include "std/mal_stdint.h"
#include "hspec/mal_hspec.h"

/**
 * @defgroup Clock
 * @brief @copybrief mal_clock.h
 * @{
 */

/**
 * @brief Will return the source clock a given timer.
 * @param timer Valid values are defined by ::mal_hspec_timer_e.
 * @param clock A pointer of type uint64_t. If the function returns
 * #MAL_ERROR_OK, clock will contain the frequency in hertz.
 * @return Return #MAL_ERROR_OK on success.
 */
#define mal_clock_get_timer_input_clk(timer, clock) mal_hspec_get_timer_input_clk(timer, clock)

/**
 * @brief Returns the system clock in hertz.
 * @return Returns the system clock in hertz.
 */
uint64_t mal_clock_get_system_clock(void);

/**
 * @}
 */

#endif /* CLOCK_MAL_CLOCK_H_ */
