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
#include "std/mal_bool.h"

typedef struct MAL_TIMER {
    mal_timer_e timer;
    bool used;
} mal_timer_s;

typedef struct MAL_TIMER_PWM {

} mal_timer_pwm_s;

typedef struct MAL_TIMER_INTERRUPT_STATE {

} mal_timer_interrupt_state_s;

typedef struct MAL_TIMER_INPUT_CAPTURE {

} mal_timer_input_capture_s;

/**
 * Is true by default. When true, the passage of time will be simulated by a thread. If false, user must manually
 * control the timer.
 */
extern bool mal_hspec_gnu_timer_emulated;

mal_error_e mal_hspec_gnu_timer_do_input_capture_callback(mal_timer_e timer,  mal_gpio_port_e port, uint8_t pin,
                                                          uint64_t value);

uint32_t mal_hspec_gnu_timer_get_count(mal_timer_e timer);

void mal_hspec_gnu_timer_set_count(mal_timer_e timer, uint32_t count);

void mal_hspec_gnu_timer_restore_default_emulation(void);

#endif /* HSPEC_GNU_MAL_HSPEC_GNU_TIMER_H_ */
