/*
 * mal_hspec.h
 *
 *  Created on: May 2, 2015
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

#ifndef HSPEC_MAL_HSPEC_H_
#define HSPEC_MAL_HSPEC_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"

mal_error_e mal_hspec_initialise_system_clk(void);

uint64_t mal_hspec_get_external_clk_freq(void);

mal_error_e mal_hspec_is_timer_valid(mal_timer_e timer);

mal_error_e mal_hspec_is_i2c_interface_valid(mal_hspec_i2c_e interface, const mal_gpio_s *scl, const mal_gpio_s *sda);

mal_error_e mal_hspec_is_can_interface_valid(mal_hspec_can_e interface, const mal_gpio_s *tx, const mal_gpio_s *rx);

mal_error_e mal_hspec_is_spi_interface_valid(mal_hspec_spi_e interface, const mal_gpio_s *mosi, const mal_gpio_s *miso, const mal_gpio_s *clk, const mal_gpio_s *select);

mal_error_e mal_hspec_is_input_capture_valid(mal_timer_e timer, const mal_gpio_s *gpio);

mal_error_e mal_hspec_flash_is_page_valid(uint32_t page);

mal_error_e mal_hspec_is_dac_valid(mal_hspec_dac_e dac, const mal_gpio_s *gpio);

#endif /* HSPEC_MAL_HSPEC_H_ */
