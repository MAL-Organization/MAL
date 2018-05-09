/*
 * mal_timer2.c
 *
 *  Created on: May 8, 2018
 *      Author: olivi
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
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
 */

#include "timer/mal_timer2.h"

static mal_error_e mal_timer_get_available_timer(mal_timer_e *timer);

static mal_timer_state_s timer_states[MAL_TIMER_SIZE];

static mal_error_e mal_timer_get_available_timer(mal_timer_e *timer) {
  int i;
  for (i = 0; i < MAL_TIMER_SIZE; i++) {
      if (timer_states[i].is_available) {
          *timer = i;
          return MAL_ERROR_OK;
      }
  }

  return MAL_ERROR_HARDWARE_UNAVAILABLE;
}

mal_error_e mal_timer_reserve(mal_timer_e timer, mal_timer_e *reserved_timer) {
    mal_error_e result;
    // Check if timer is specified
    if (MAL_TIMER_ANY == timer) {
        result = mal_timer_get_available_timer(&timer);
        if (MAL_ERROR_OK != result) {
            return result;
        }
    }
    // Reserve timer
    if (timer_states[timer].is_available) {
        timer_states[timer].is_available = false;
        *reserved_timer = timer;
    } else {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count_mask(mal_timer_e timer, uint64_t *mask) {
    mal_error_e result;
    // Get timer resolution
    uint8_t resolution;
    result = mal_timer_get_resolution(timer, &resolution);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Compute mask
    if (resolution >= 64) {
        *mask = UINT64_MAX;
    } else {
        *mask = ((((uint64_t)1)<<((uint64_t)resolution)) - ((uint64_t)1));
    }
    return MAL_ERROR_OK;
}
