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

/**
 * @brief Frees a used timer.
 * @param timer The timer to free.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_free(mal_timer_e timer);

/**
 * @brief Similar to mal_timer_free, but will be unmanaged. This means this
 * timer will not be flagged as busy and the use of timer ANY will not work
 * properly.
 * @param timer
 * @return
 */
mal_error_e mal_timer_free_unmanaged(mal_timer_e timer);

/**
 * @}
 */

#endif /* TIMER_MAL_TIMER_H_ */
