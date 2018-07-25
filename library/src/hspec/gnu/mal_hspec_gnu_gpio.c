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
#include "std/mal_defs.h"

typedef struct {
	bool input;
	bool output;
	mal_gpio_direction_e direction;
	mal_gpio_event_e event; /**< The event that will trigger.*/
	mal_gpio_event_callback_t callback; /**< The callback to execute upon event.*/
	void *callback_handle;
} mal_hspec_gpio_info_s;

static mal_hspec_gpio_info_s gpio_array[MAL_GPIO_PORT_SIZE][MAL_HSPEC_GNU_GPIO_PORT_SIZE];

mal_error_e mal_gpio_init(mal_gpio_init_s *init, mal_gpio_s *handle) {
    // Set local IO
	gpio_array[init->port][init->pin].output = false;
	gpio_array[init->port][init->pin].direction = init->direction;
	gpio_array[init->port][init->pin].event = MAL_GPIO_EVENT_BOTH;
	gpio_array[init->port][init->pin].callback = NULL;
    gpio_array[init->port][init->pin].callback_handle = NULL;
    // Set handle
    handle->port = init->port;
    handle->pin = init->pin;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_gnu_set_mocked_gpio(mal_gpio_port_e port, uint8_t pin, bool value) {
	if (gpio_array[port][pin].direction == MAL_GPIO_DIRECTION_OUT) {
			return MAL_ERROR_HARDWARE_INVALID;
	}
	// Save old value
	bool old_value = gpio_array[port][pin].input;
	// Set value
	gpio_array[port][pin].input = value;
	// Check event and execute event type has happen
	switch (gpio_array[port][pin].event) {
		case (MAL_GPIO_EVENT_FALLING):
				if (old_value == 1 && value == 0) {
                    mal_hspec_gnu_gpio_execute_callback(port, pin);
				}
				break;
		case (MAL_GPIO_EVENT_RISING):
				if (old_value == 0 && value == 1) {
                    mal_hspec_gnu_gpio_execute_callback(port, pin);
				}
				break;
		case (MAL_GPIO_EVENT_BOTH):
				if (old_value != value) {
                    mal_hspec_gnu_gpio_execute_callback(port, pin);
				}
				break;
		default:
				break;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_gpio_set(mal_gpio_s *gpio, bool value) {
	gpio_array[gpio->port][gpio->pin].output = value;
	if (gpio_array[gpio->port][gpio->pin].direction == MAL_GPIO_DIRECTION_OUT) {
		gpio_array[gpio->port][gpio->pin].input = value;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_gpio_toggle(mal_gpio_s *gpio) {
	return mal_gpio_set(gpio, !gpio_array[gpio->port][gpio->pin].output);
}

mal_error_e mal_gpio_get(mal_gpio_s *gpio, bool *value) {
    *value = gpio_array[gpio->port][gpio->pin].input;
	return MAL_ERROR_OK;
}

mal_error_e mal_gpio_event_init(mal_gpio_event_init_s *init, mal_gpio_s *gpio_handle, mal_gpio_event_s *event_handle) {
    mal_gpio_init_s gpio_init;
    gpio_init.port = init->port;
    gpio_init.pin = init->pin;
    gpio_init.speed = init->speed;
    gpio_init.pupd = init->pupd;
    gpio_init.direction = MAL_GPIO_DIRECTION_IN;
    mal_gpio_init(&gpio_init, gpio_handle);

	gpio_array[init->port][init->pin].event = init->event;
	gpio_array[init->port][init->pin].callback = init->callback;

	event_handle->gpio_handle = gpio_handle;

	return MAL_ERROR_OK;
}

void mal_hspec_gnu_gpio_execute_callback(mal_gpio_port_e port, uint8_t pin) {
	// Fetch callback
	mal_gpio_event_callback_t cb = gpio_array[port][pin].callback;
	if (NULL != cb) {
	    // Fetch handle
        void *handle = gpio_array[port][pin].callback_handle;
		// Execute
		cb(handle);
	}
}

mal_error_e mal_gpio_event_remove(mal_gpio_event_s *handle) {
	gpio_array[handle->gpio_handle->port][handle->gpio_handle->pin].event = MAL_GPIO_EVENT_BOTH;
	gpio_array[handle->gpio_handle->port][handle->gpio_handle->pin].callback = NULL;

	return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_gpio_event_disable_interrupt(mal_gpio_event_s *handle, mal_gpio_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
	MAL_DEFS_UNUSED(state);
}

MAL_DEFS_INLINE void mal_gpio_event_restore_interrupt(mal_gpio_event_s *handle, mal_gpio_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(state);
}
