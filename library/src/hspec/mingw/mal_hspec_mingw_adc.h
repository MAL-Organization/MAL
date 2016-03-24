/*
 * mal_hspec_mingw_adc.h
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_ADC_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_ADC_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_adc_ios(mal_hspec_adc_e adc,const mal_hspec_gpio_s **ios, uint8_t size);

void mal_hspec_mingw_add_adc(mal_hspec_adc_e adc);

mal_error_e mal_hspec_mingw_adc_init(mal_hspec_adc_init_s *init);

mal_error_e mal_hspec_mingw_adc_read(mal_error_e adc, uint64_t *value);

mal_error_e mal_hspec_mingw_adc_resolution(mal_hspec_adc_init_s adc, uint8_t *resolution);

void mal_hspec_mingw_adc_set_value(mal_hspec_adc_e adc, float value);

mal_error_e mal_hspec_mingw_adc_async_read(mal_hspec_adc_e adc, mal_hspec_adc_read_callback_t callback);

void mal_hspec_mingw_adc_do_async(mal_hspec_adc_e adc);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_ADC_H_ */