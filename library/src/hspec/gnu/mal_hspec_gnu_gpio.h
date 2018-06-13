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

#ifndef HSPEC_GNU_MAL_HSPEC_GNU_GPIO_H_
#define HSPEC_GNU_MAL_HSPEC_GNU_GPIO_H_

#include "gpio/mal_gpio.h"

/**
 * Number of IO per port.
 */
#define MAL_HSPEC_GNU_GPIO_PORT_SIZE    32

typedef struct MAL_GPIO {
    mal_gpio_port_e port;
    uint8_t pin;
} mal_gpio_s;

typedef struct MAL_GPIO_EVENT {
    mal_gpio_s *gpio_handle;
} mal_gpio_event_s;

typedef struct MAL_GPIO_INTERRUPT_STATE {

} mal_gpio_interrupt_state_s;

void mal_hspec_gnu_gpio_execute_callback(mal_gpio_port_e port, uint8_t pin);

mal_error_e mal_hspec_gnu_set_mocked_gpio(mal_gpio_port_e port, uint8_t pin, bool value);

#endif /* HSPEC_GNU_MAL_HSPEC_GNU_GPIO_H_ */
