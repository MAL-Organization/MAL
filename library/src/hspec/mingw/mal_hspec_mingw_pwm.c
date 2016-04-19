/*
 * mal_hspec_mingw_pwm.c
 *
 *  Created on: Apr 7, 2016
 *      Author: Olivier
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

mal_error_e mal_hspec_mingw_timer_set_pwm_duty_cycle(mal_hspec_timer_e timer, const mal_hspec_gpio_s *gpio, float duty_cycle) {
	return MAL_ERROR_OK;
}
