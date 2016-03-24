/*
 * mal_hspec_mingw_cmn.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#include "hspec/mal_hspec_cmn.h"

static mal_hspec_gpio_s valid_ios[MAL_HSPEC_PORT_SIZE][PORT_SIZE];

void mal_hspec_mingw_cmn_valid_ios(mal_hspec_gpio_s **ios, uint8_t *size) {
	// Initialise IOs
	for (int i = 0; i < MAL_HSPEC_PORT_SIZE; i++) {
		for (int j = 0; j < PORT_SIZE; j++) {
			valid_ios[i][j].port = i;
			valid_ios[i][j].pin = j;
		}
	}
	// Set IOs
	*ios = (mal_hspec_gpio_s*)valid_ios;
	*size = MAL_HSPEC_PORT_SIZE * PORT_SIZE;
}

