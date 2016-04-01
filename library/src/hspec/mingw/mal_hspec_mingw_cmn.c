/*
 * mal_hspec_mingw_cmn.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#include "hspec/mal_hspec_cmn.h"

static mal_hspec_gpio_s valid_ios[MAL_HSPEC_GPIO_PORT_SIZE][PORT_SIZE];
static mal_hspec_gpio_port_e valid_ports[MAL_HSPEC_GPIO_PORT_SIZE];
static uint64_t valid_pins[MAL_HSPEC_GPIO_PORT_SIZE];

void mal_hspec_mingw_cmn_valid_ios(const mal_hspec_gpio_s **ios, uint8_t *size) {
	// Initialise IOs
	for (int i = 0; i < MAL_HSPEC_GPIO_PORT_SIZE; i++) {
		for (int j = 0; j < PORT_SIZE; j++) {
			valid_ios[i][j].port = i;
			valid_ios[i][j].pin = j;
		}
	}
	// Set IOs
	*ios = (mal_hspec_gpio_s*)valid_ios;
	*size = MAL_HSPEC_GPIO_PORT_SIZE * PORT_SIZE;
}

void mal_hspec_mingw_cmn_valid_ports(const mal_hspec_gpio_port_e **ports, uint8_t *size) {
	// Initialise Ports
	for(int i = 0; i < MAL_HSPEC_GPIO_PORT_SIZE; i++) {
		valid_ports[i] = i;
	}

	*ports = valid_ports;
	*size = MAL_HSPEC_GPIO_PORT_SIZE;
}

void mal_hspec_mingw_cmn_valid_pins(const uint64_t **pins) {
	// Initialise Pins
	for(int i = 0; i < MAL_HSPEC_GPIO_PORT_SIZE; i++) {
		valid_pins[i] = ((uint64_t)1 << (uint64_t)PORT_SIZE) - (uint64_t)1;
	}
	*pins = valid_pins;
}
