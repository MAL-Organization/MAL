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

#ifndef HSPEC_GNU_MAL_HSPEC_GNU_TIMER_H_
#define HSPEC_GNU_MAL_HSPEC_GNU_TIMER_H_

#include "timer/mal_timer.h"
#include "gpio/mal_gpio_definitions.h"

typedef struct MAL_TIMER {
    mal_timer_e timer;
} mal_timer_s;

typedef struct MAL_TIMER_PWM {

} mal_timer_pwm_s;

mal_error_e mal_hspec_gnu_timer_do_input_capture_callback(mal_timer_e timer,  mal_gpio_port_e port, uint8_t pin,
                                                          uint64_t value);

#endif /* HSPEC_GNU_MAL_HSPEC_GNU_TIMER_H_ */
