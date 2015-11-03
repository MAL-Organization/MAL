/**
 * @file mal_timer.h
 * @author Olivier Allaire
 * @date May 25 2015
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
 * @brief Functions to access and configure the timers of the MCU.
 */

#ifndef TIMER_MAL_TIMER_H_
#define TIMER_MAL_TIMER_H_

#include "hspec/mal_hspec_cmn.h"
#include "std/mal_error.h"
#include "std/mal_stdint.h"

/**
 * @defgroup Timer
 * @brief @copybrief mal_timer.h
 * @{
 */

/**
 * @brief Initialize a timer as a simple tick counter. Use ::mal_timer_get_tick
 * to read ticks.
 * @param timer The desired timer to initialize.
 * @param frequency The frequency to count at in hertz.
 * @param delta The allowed delta of frequency in hertz. This means the actual
 * frequency of the timer can be frequency +/- delta.
 * @param handle This handle will return the used timer. Useful when using
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_tick(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_e *handle);

/**
 * @brief Initialize a timer that periodically calls a function (task).
 * @param timer The desired timer to initialize.
 * @param frequency The frequency to count at in hertz.
 * @param delta delta The allowed delta of frequency in hertz. This means the actual
 * frequency of the timer can be frequency +/- delta.
 * @param callback The callback to execute at every count.
 * @param handle This handle will return the used timer. Useful when using
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_task(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback, mal_hspec_timer_e *handle);

/**
 * @brief Return the tick of a timer when initialized in tick mode.
 * @param handle The timer to check for tick.
 * @return The tick count.
 */
uint64_t mal_timer_get_tick(mal_hspec_timer_e handle);

/**
 * @brief Frees a used timer.
 * @param timer The timer to free.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_free(mal_hspec_timer_e timer);

/**
 * @}
 */

#endif /* TIMER_MAL_TIMER_H_ */
