/**
 * @file mal_watchdog.h
 * @author Olivier Allaire
 * @date September 22 2016
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
 * @brief Functions to use the MCU's watchdog.
 */

#ifndef WATCHDOG_MAL_WATCHDOG_H_
#define WATCHDOG_MAL_WATCHDOG_H_

/**
 * @defgroup Watchdog
 * @brief @copybrief mal_watchdog.h
 * @{
 */

/**
 * @brief This function will initialize and start the watchdog.
 * @param timeout_ms A uint32_t that contains the watchdog timeout in ms. If
 * the watchdog is not fed before this timeout is over, the MCU will reset.
 * @return Returns #MAL_ERROR_OK on success.
 * @note Watchdog clock sources may have a large delta. Check your MCU's
 * datasheet and choose a timeout accordingly.
 */
#define mal_watchdog_init(timeout_ms) mal_hspec_watchdog_init(timeout_ms)

/**
 * @brief Call this function to reset the watchdog timeout.
 * @return Returns nothing.
 */
#define mal_watchdog_feed() mal_hspec_watchdog_feed()

/**
 * @}
 */

#endif /* WATCHDOG_MAL_WATCHDOG_H_ */
