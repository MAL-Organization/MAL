/*
 * mal_hspec_mingw_gpio.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Kevin
 */

#include "mal_hspec_mingw_gpio.h"
#include "mal_hspec_mingw_cmn.h"
#include "stddef.h"

typedef struct {
	mal_hspec_gpio_s gpio;
	bool value;
} gpio_info_s;

static bool gpio_array[MAL_HSPEC_PORT_SIZE][PORT_SIZE];
static mal_hspec_gpio_event_callback_t gpio_event_callbacks[MAL_HSPEC_PORT_SIZE][PORT_SIZE];


mal_error_e mal_hspec_mingw_gpio_init(mal_hspec_gpio_init_s *gpio_init){
	gpio_array[gpio_init->gpio.port][gpio_init->gpio.pin] = false;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_get_valid_ports(const mal_hspec_port_e **ports, uint8_t *size){
	mal_hspec_mingw_cmn_valid_ports(ports, size);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_get_valid_pins(const uint64_t **pins){
	mal_hspec_mingw_cmn_valid_pins(pins);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_set_gpio(const mal_hspec_gpio_s *gpio, bool value){
	gpio_array[gpio->port][gpio->pin] = value;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_toggle_gpio(const mal_hspec_gpio_s *gpio){
	gpio_array[gpio->port][gpio->pin] = !gpio_array[gpio->port][gpio->pin];

	return MAL_ERROR_OK;
}

bool mal_hspec_mingw_get_gpio(const mal_hspec_gpio_s *gpio){
	return gpio_array[gpio->port][gpio->pin];
}

mal_error_e mal_hspec_mingw_gpio_event_init(mal_hspec_gpio_event_init_s *init){
	gpio_event_callbacks[init->gpio->port][init->gpio->pin] = init->callback;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_gpio_event_remove(const mal_hspec_gpio_s *gpio){
	gpio_event_callbacks[gpio->port][gpio->pin] = NULL;

	return MAL_ERROR_OK;
}
