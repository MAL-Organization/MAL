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
#include "std/mal_types.h"
#include "std/mal_error.h"

/**
 * @defgroup Clock
 * @brief @copybrief mal_clock.h
 * @{
 */

/**
 * @brief Possible clock sources for the MCU
 */
typedef enum {
    MAL_SYSTEM_CLK_SRC_INTERNAL, //!< Source is an internal oscillator.
    MAL_SYSTEM_CLK_SRC_EXTERNAL, //!< Source is an external crystal or oscillator.
    MAL_SYSTEM_CLK_SRC_AUTO      //!< MAL will select the source. Note it is safer to define a known source.
} mal_system_clk_src_e;

/**
* Hardware clock configuration that must be defined by hardware specific implementation.
*/
typedef struct MAL_CLOCK_CONFIG mal_clock_config_s;

/**
 * @brief Structure to define the system clock source.
 */
typedef struct {
    mal_hertz_t frequency; /**< The frequency to run at.*/
    mal_system_clk_src_e src; /**< The source of the frequency.*/
    const mal_clock_config_s *clock_config; /**< If left NULL, the configuration is automatic. If specified, see hardware specific implementation for details.*/
} mal_system_clk_s;

/**
 * @brief Do not use this function outside of startup. Will do all calls to
 * initialize system clock.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_clock_initialise_system_clock(void);

/**
 * @brief Set the system clock. Note that unless you want to change it at run
 * time, the clock is automatically initialized at boot.
 * @param clk The clock to set.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_clock_set_system_clock(const mal_system_clk_s *clk);

/**
 * @brief Similar to mal_clock_set_system_clock but using this directly will
 * break mal_clock_get_system_clock. Use mal_clock_set_system_clock instead.
 * @param clk The clock paramaters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_clock_set_system_clock_unmanaged(const mal_system_clk_s *clk);

/**
 * Get the external frequency that can feed the system clock.
 * @return Returns the external frequency.
 */
mal_hertz_t mal_clock_get_external_clock_frequency(void);

/**
 * @brief Returns the system clock frequency.
 * @return Returns the system clock in frequency.
 */
mal_hertz_t mal_clock_get_system_clock(void);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* CLOCK_MAL_CLOCK_H_ */
