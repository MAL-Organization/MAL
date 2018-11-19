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

#include "dac/mal_dac.h"

typedef struct {
	mal_dac_e dac;
	mal_dac_init_s init;
	uint64_t value;
} dac_info_s;

static dac_info_s dac_array[MAL_DAC_SIZE];

mal_error_e mal_dac_init(mal_dac_init_s *init, mal_dac_s *handle) {
	dac_array[init->dac].dac = init->dac;
	dac_array[init->dac].init = *init;
	dac_array[init->dac].value = 0;
	handle->dac = init->dac;
	return MAL_ERROR_OK;
}

mal_error_e mal_dac_write_bits(mal_dac_s *handle, uint64_t value) {
	dac_array[handle->dac].value = value;
	return MAL_ERROR_OK;
}

mal_error_e mal_dac_resolution(mal_dac_s *handle, uint8_t *resolution) {
	*resolution = dac_array[handle->dac].init.bit_resolution;
	return MAL_ERROR_OK;
}

mal_error_e mal_dac_maximum_resolution(mal_dac_e dac, uint8_t *resolution) {
	MAL_DEFS_UNUSED(dac);
	*resolution = 12;
	return MAL_ERROR_OK;
}
