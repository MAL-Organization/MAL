/*
 * mal_hspec_mingw_dac.c
 *
 *  Created on: Oct 25, 2016
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
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

#include "mal_hspec_mingw_dac.h"
#include "mal_hspec_mingw_cmn.h"

typedef struct {
	mal_hspec_dac_e dac;
	mal_hspec_dac_init_s init;
	uint64_t value;
} dac_info_s;

static dac_info_s dac_array[MAL_HSPEC_DAC_SIZE];

mal_error_e mal_hspec_mingw_get_valid_dac_ios(mal_hspec_adc_e dac,
											  const mal_hspec_gpio_s **ios,
											  uint8_t *size) {
	mal_hspec_mingw_cmn_valid_ios(ios, size);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_dac_init(mal_hspec_dac_init_s *init) {
	dac_array[init->dac].dac = init->dac;
	dac_array[init->dac].init = *init;
	dac_array[init->dac].value = 0;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_dac_write(mal_hspec_dac_e dac, uint64_t value) {
	dac_array[dac].value = value;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_dac_resolution(mal_hspec_dac_e dac, uint8_t *resolution) {
	*resolution = dac_array[dac].init.bit_resolution;
	return MAL_ERROR_OK;
}
