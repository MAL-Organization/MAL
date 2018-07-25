/*
 * Copyright (c) 2018 Olivier Allaire
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

#include "timer/mal_timer.h"
#include "std/mal_defs.h"

mal_error_e mal_timer_init_pwm(mal_timer_init_pwm_s *init, mal_timer_s *handle, mal_timer_pwm_s *pwm_handle) {
    MAL_DEFS_UNUSED(init);
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(pwm_handle);
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_set_pwm_duty_cycle(mal_timer_pwm_s *handle, mal_ratio_t duty_cycle) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(duty_cycle);
	return MAL_ERROR_OK;
}
