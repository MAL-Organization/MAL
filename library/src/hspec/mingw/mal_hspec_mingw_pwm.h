/*
 * mal_hspec_mingw_pwm.h
 *
 *  Created on: Apr 7, 2016
 *      Author: Olivier
 */

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_PWM_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_PWM_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_pwm_ios(mal_hspec_timer_e timer, const mal_hspec_gpio_s **ios, uint8_t *size);

mal_error_e mal_hspec_mingw_timer_pwm_init(mal_hspec_timer_pwm_init_s *init);

mal_error_e mal_hspec_mingw_timer_set_pwm_duty_cycle(mal_hspec_timer_e timer, const mal_hspec_gpio_s *gpio, float duty_cycle);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_PWM_H_ */
