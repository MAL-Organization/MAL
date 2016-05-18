/*
 * mal_hspec_mingw_gpio.h
 *
 *  Created on: Mar 24, 2016
 *      Author: Kevin
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_

#include "std/mal_error.h"
#include "std/mal_bool.h"
#include "hspec/mal_hspec_cmn.h"

mal_error_e mal_hspec_mingw_gpio_init(mal_hspec_gpio_init_s *gpio_init);

mal_error_e mal_hspec_mingw_get_valid_ports(const mal_hspec_gpio_port_e **ports, uint8_t *size);

mal_error_e mal_hspec_mingw_get_valid_pins(const uint64_t **pins);

mal_error_e mal_hspec_mingw_set_gpio(const mal_hspec_gpio_s *gpio, bool value);

mal_error_e mal_hspec_mingw_toggle_gpio(const mal_hspec_gpio_s *gpio);

bool mal_hspec_mingw_get_gpio(const mal_hspec_gpio_s *gpio);

mal_error_e mal_hspec_mingw_gpio_event_init(mal_hspec_gpio_event_init_s *init);

void mal_hspec_mingw_gpio_execute_callback(mal_hspec_gpio_s gpio);

mal_error_e mal_hspec_mingw_gpio_event_remove(const mal_hspec_gpio_s *gpio);

mal_error_e mal_hspec_mingw_set_mocked_gpio(const mal_hspec_gpio_s *gpio, bool value);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_ */
