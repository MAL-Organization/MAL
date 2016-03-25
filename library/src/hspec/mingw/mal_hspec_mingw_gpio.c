/*
 * mal_hspec_mingw_gpio.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Kevin
 */

#include "mal_hspec_mingw_gpio.h"
#include "mal_hspec_mingw_cmn.h"
#include "std/mal_stdlib.h"

typedef struct {
	bool input;
	bool output;
	mal_hspec_gpio_dir_e direction;
	mal_hspec_gpio_event_e event; /**< The event that will trigger.*/
	mal_hspec_gpio_event_callback_t callback; /**< The callback to execute upon event.*/
} mal_hspec_gpio_info_s;

static mal_hspec_gpio_info_s gpio_array[MAL_HSPEC_PORT_SIZE][PORT_SIZE];

mal_error_e mal_hspec_mingw_gpio_init(mal_hspec_gpio_init_s *gpio_init) {
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].input = false;
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].output = false;
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].direction = gpio_init->direction;
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].event = MAL_HSPEC_GPIO_EVENT_BOTH;
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin].callback = NULL;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_get_valid_ports(const mal_hspec_port_e **ports, uint8_t *size) {
	mal_hspec_mingw_cmn_valid_ports(ports, size);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_get_valid_pins(const uint64_t **pins) {
	mal_hspec_mingw_cmn_valid_pins(pins);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_set_mocked_gpio(const mal_hspec_gpio_s *gpio, bool value) {
	if (gpio_array[gpio->port][gpio->pin].direction == MAL_GPIO_DIR_OUT) {
			return MAL_ERROR_HARDWARE_INVALID;
	}
	// Save old value
	bool old_value = gpio_array[gpio->port][gpio->pin].input;
	// Set value
	gpio_array[gpio->port][gpio->pin].input = value;
	// Check event and execute event type has happen
	switch (gpio_array[gpio->port][gpio->pin].event) {
		case (MAL_HSPEC_GPIO_EVENT_FALLING):
				if (old_value == 1 && value == 0) {
					mal_hspec_mingw_gpio_do_async(*gpio);
				}
				break;
		case (MAL_HSPEC_GPIO_EVENT_RISING):
				if (old_value == 0 && value == 1) {
					mal_hspec_mingw_gpio_do_async(*gpio);
				}
				break;
		default:
				mal_hspec_mingw_gpio_do_async(*gpio);
				break;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_set_gpio(const mal_hspec_gpio_s *gpio, bool value) {
	if (gpio_array[gpio->port][gpio->pin].direction == MAL_GPIO_DIR_IN) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	gpio_array[gpio->port][gpio->pin].input = value;
	gpio_array[gpio->port][gpio->pin].output = value;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_toggle_gpio(const mal_hspec_gpio_s *gpio) {
	if (gpio_array[gpio->port][gpio->pin].direction == MAL_GPIO_DIR_IN) {
			return MAL_ERROR_HARDWARE_INVALID;
	}

	mal_hspec_mingw_set_gpio(gpio, !gpio_array[gpio->port][gpio->pin].output);

	return MAL_ERROR_OK;
}

bool mal_hspec_mingw_get_gpio(const mal_hspec_gpio_s *gpio) {
	return gpio_array[gpio->port][gpio->pin].input;
}

mal_error_e mal_hspec_mingw_gpio_event_init(mal_hspec_gpio_event_init_s *init) {
	gpio_array[init->gpio->port][init->gpio->pin].event = init->event;
	gpio_array[init->gpio->port][init->gpio->pin].callback = init->callback;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_gpio_async_event(mal_hspec_gpio_s *gpio, mal_hspec_gpio_event_callback_t callback) {
	if (NULL == gpio_array[gpio->port][gpio->pin].callback) {
		gpio_array[gpio->port][gpio->pin].callback = callback;
		return MAL_ERROR_OK;
	}
	return MAL_ERROR_HARDWARE_UNAVAILABLE;
}

void mal_hspec_mingw_gpio_do_async(mal_hspec_gpio_s gpio) {
	if (NULL != gpio_array[gpio.port][gpio.pin].callback) {
		// Fetch callback
		mal_hspec_gpio_event_callback_t cb = gpio_array[gpio.port][gpio.pin].callback;
		gpio_array[gpio.port][gpio.pin].callback = NULL;
		// Execute
		cb();
	}
}

mal_error_e mal_hspec_mingw_gpio_event_remove(const mal_hspec_gpio_s *gpio) {
	gpio_array[gpio->port][gpio->pin].event = MAL_HSPEC_GPIO_EVENT_BOTH;
	gpio_array[gpio->port][gpio->pin].callback = NULL;

	return MAL_ERROR_OK;
}
