/*
 * mal_hspec_mingw_dac.h
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_DAC_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_DAC_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_dac_ios(mal_hspec_adc_e dac,
											  const mal_hspec_gpio_s **ios,
											  uint8_t *size);
mal_error_e mal_hspec_mingw_dac_init(mal_hspec_dac_init_s *init);
mal_error_e mal_hspec_mingw_dac_write(mal_hspec_dac_e dac, uint64_t value);
mal_error_e mal_hspec_mingw_dac_resolution(mal_hspec_dac_e dac, uint8_t *resolution);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_DAC_H_ */