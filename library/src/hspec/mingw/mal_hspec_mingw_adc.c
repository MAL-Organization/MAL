/*
 * mal_hspec_mingw_adc.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#include "mal_hspec_mingw_adc.h"

typedef struct {
	mal_hspec_adc_e adc;
	mal_hspec_adc_init_s init;
	mal_hspec_adc_read_callback_t callback;
	float value;
} adc_info_s;

extern float mal_external_vdda;

static adc_info_s adc_array[MAL_HSPEC_ADC_SIZE];
static mal_hspec_gpio_s valid_adc_ios[MAL_HSPEC_PORT_SIZE][PORT_SIZE];

mal_error_e mal_hspec_mingw_get_valid_adc_ios(mal_hspec_adc_e adc,const mal_hspec_gpio_s **ios, uint8_t *size) {
	// Initialise IOs
	for (int i = 0; i < MAL_HSPEC_PORT_SIZE; i++) {
		for (int j = 0; j < PORT_SIZE; j++) {
			valid_adc_ios[i][j].port = i;
			valid_adc_ios[i][j].pin = j;
		}
	}
	// Set ios
	*ios = (mal_hspec_gpio_s*)valid_adc_ios;
	// Set size
	*size = MAL_HSPEC_PORT_SIZE * PORT_SIZE;

	return MAL_ERROR_OK;
}


mal_error_e mal_hspec_mingw_adc_init(mal_hspec_adc_init_s *init) {
	// Save info
	adc_array[init->adc].init = *init;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_adc_read(mal_error_e adc, uint64_t *value) {
	// Compute ratio
	float ratio = adc_array[adc].value / mal_external_vdda;
	// Get resolution
	uint8_t resolution;
	mal_hspec_mingw_adc_resolution(adc, &resolution);
	// Compute max int
	uint64_t max_adc_value = (((uint64_t)1) << resolution) - 1;
	// Convert to int
	*value = (uint64_t)(ratio * (float)max_adc_value);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_adc_resolution(mal_hspec_adc_e adc, uint8_t *resolution) {
	*resolution = adc_array[adc].init.bit_resolution;
	return MAL_ERROR_OK;
}

void mal_hspec_mingw_adc_set_value(mal_hspec_adc_e adc, float value) {
	adc_array[adc].value = value;
}

mal_error_e mal_hspec_mingw_adc_async_read(mal_hspec_adc_e adc, mal_hspec_adc_read_callback_t callback) {
	if (NULL == adc_array[adc].callback) {
		adc_array[adc].callback = callback;
		return MAL_ERROR_OK;
	}
	return MAL_ERROR_HARDWARE_UNAVAILABLE;
}

void mal_hspec_mingw_adc_do_async(mal_hspec_adc_e adc) {
	if (NULL != adc_array[adc].callback) {
		// Fetch callback
		mal_hspec_adc_read_callback_t cb = adc_array[adc].callback;
		adc_array[adc].callback = NULL;
		// Fetch value
		uint64_t value;
		mal_hspec_mingw_adc_read(adc, &value);
		// Execute
		cb(adc, value);
	}
}
