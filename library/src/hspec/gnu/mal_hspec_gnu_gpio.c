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

#include "mal_hspec_gnu_gpio.h"
#include "std/mal_stdlib.h"

typedef struct {
	bool input;
	bool output;
	mal_gpio_dir_e direction;
	mal_gpio_event_e event; /**< The event that will trigger.*/
	mal_gpio_event_callback_t callback; /**< The callback to execute upon event.*/
} mal_hspec_gpio_info_s;

static mal_hspec_gpio_info_s gpio_array[MAL_GPIO_PORT_SIZE][PORT_SIZE];

mal_error_e mal_gpio_init(mal_gpio_init_s *gpio_init) {
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].output = false;
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].direction = gpio_init->direction;
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].event = MAL_GPIO_EVENT_BOTH;
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].callback = NULL;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_gnu_set_mocked_gpio(const mal_gpio_s *gpio, bool value) {
	if (gpio_array[gpio->port][gpio->pin].direction == MAL_GPIO_DIR_OUT) {
			return MAL_ERROR_HARDWARE_INVALID;
	}
	// Save old value
	bool old_value = gpio_array[gpio->port][gpio->pin].input;
	// Set value
	gpio_array[gpio->port][gpio->pin].input = value;
	// Check event and execute event type has happen
	switch (gpio_array[gpio->port][gpio->pin].event) {
		case (MAL_GPIO_EVENT_FALLING):
				if (old_value == 1 && value == 0) {
                    mal_hspec_gnu_gpio_execute_callback(*gpio);
				}
				break;
		case (MAL_GPIO_EVENT_RISING):
				if (old_value == 0 && value == 1) {
                    mal_hspec_gnu_gpio_execute_callback(*gpio);
				}
				break;
		case (MAL_GPIO_EVENT_BOTH):
				if (old_value != value) {
                    mal_hspec_gnu_gpio_execute_callback(*gpio);
				}
				break;
		default:
				break;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_gpio_set(const mal_gpio_s *gpio, bool value) {
	gpio_array[gpio->port][gpio->pin].output = value;
	if (gpio_array[gpio->port][gpio->pin].direction == MAL_GPIO_DIR_OUT) {
		gpio_array[gpio->port][gpio->pin].input = value;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_gpio_toggle(const mal_gpio_s *gpio) {
	return mal_gpio_set(gpio, !gpio_array[gpio->port][gpio->pin].output);
}

bool mal_gpio_get(const mal_gpio_s *gpio) {
	return gpio_array[gpio->port][gpio->pin].input;
}

mal_error_e mal_gpio_event_init(mal_gpio_init_s *gpio_init, mal_gpio_event_init_s *event_init) {
	gpio_array[event_init->gpio->port][event_init->gpio->pin].event = event_init->event;
	gpio_array[event_init->gpio->port][event_init->gpio->pin].callback = event_init->callback;

	return MAL_ERROR_OK;
}

void mal_hspec_gnu_gpio_execute_callback(mal_gpio_s gpio) {
	if (NULL != gpio_array[gpio.port][gpio.pin].callback) {
		// Fetch callback
		mal_gpio_event_callback_t cb = gpio_array[gpio.port][gpio.pin].callback;
		// Execute
		cb(&gpio);
	}
}

mal_error_e mal_gpio_event_remove(const mal_gpio_s *gpio) {
	gpio_array[gpio->port][gpio->pin].event = MAL_GPIO_EVENT_BOTH;
	gpio_array[gpio->port][gpio->pin].callback = NULL;

	return MAL_ERROR_OK;
}

MAL_DEFS_INLINE bool mal_gpio_event_disable_interrupt(const mal_gpio_s *gpio) {
    return false;
}

MAL_DEFS_INLINE void mal_gpio_event_enable_interrupt(const mal_gpio_s *gpio, bool active) {
    return;
}
