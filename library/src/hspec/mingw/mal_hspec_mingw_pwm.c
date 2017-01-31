/*
 * mal_hspec_mingw_pwm.c
 *
 *  Created on: Apr 7, 2016
 *      Author: Olivier
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

#include "mal_hspec_mingw_pwm.h"
#include "mal_hspec_mingw_cmn.h"

mal_error_e mal_hspec_mingw_get_valid_pwm_ios(mal_hspec_timer_e timer, const mal_hspec_gpio_s **ios, uint8_t *size) {
	mal_hspec_mingw_cmn_valid_ios(ios, size);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_timer_pwm_init(mal_hspec_timer_pwm_init_s *init) {
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_timer_set_pwm_duty_cycle(mal_hspec_timer_e timer, const mal_hspec_gpio_s *gpio, mal_ratio_t duty_cycle) {
	return MAL_ERROR_OK;
}
