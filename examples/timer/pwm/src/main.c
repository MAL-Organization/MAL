/*
 * Copyright (c) 2019 Olivier Allaire
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

#include "std/mal_bool.h"
#include "timer/mal_timer.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "OCDFAInspection"
int main(void) {
    mal_error_e result;
    mal_timer_s timer;
    mal_timer_pwm_s pwm;
    mal_timer_init_pwm_s init;
    // Initialise timer to 1000Hz with 50% duty cycle.
    init.timer = MAL_TIMER_2;
    init.frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(1000000ULL);
    init.delta = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(0);
    init.port = MAL_GPIO_PORT_A;
    init.pin = 3;
    result = mal_timer_init_pwm(&init, &timer, &pwm);
    if (MAL_ERROR_OK != result) {
        while (true);
    }
    result = mal_timer_set_pwm_duty_cycle(&pwm, MAL_TYPES_INT_RATIO_TO_RATIO(50, 100));
    if (MAL_ERROR_OK != result) {
        while (true);
    }
    // Wait
    uint64_t count = 200000000ULL;
    while (count--);
    // Change the duty cycle
    result = mal_timer_set_pwm_duty_cycle(&pwm, MAL_TYPES_INT_RATIO_TO_RATIO(25, 100));
    if (MAL_ERROR_OK != result) {
        while (true);
    }
    // Wait
    count = 200000000ULL;
    while (count--);
    // Change frequency
    result = mal_timer_set_frequency(&timer, MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(500000),
                                     MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(1000));
    if (MAL_ERROR_OK != result) {
        while (true);
    }
    result = mal_timer_set_pwm_duty_cycle(&pwm, MAL_TYPES_INT_RATIO_TO_RATIO(25, 100));
    if (MAL_ERROR_OK != result) {
        while (true);
    }

    while (true);

    return 0;
}
#pragma clang diagnostic pop
