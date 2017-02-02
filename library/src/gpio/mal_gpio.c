/*
 * mal_gpio.c
 *
 *  Created on: Jun 24, 2015
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

#include "mal_gpio.h"

mal_error_e mal_gpio_init(mal_hspec_gpio_init_s *gpio_init) {
	mal_error_e result;
	// Check gpio
	result = mal_hspec_is_gpio_valid(&gpio_init->gpio);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	return mal_hspec_gpio_init(gpio_init);
}

mal_error_e mal_gpio_event_init(mal_hspec_gpio_init_s *gpio_init, mal_hspec_gpio_event_init_s *event_init) {
	mal_error_e result = mal_gpio_init(gpio_init);
	if (MAL_ERROR_OK != result) {
		return result;
	}

	return mal_hspec_gpio_event_init(event_init);
}

mal_error_e mal_gpio_deinit(const mal_hspec_gpio_s *gpio) {
	// Disable IRQ interrupt
	mal_gpio_event_disable_interrupt(gpio);
	// Set IO to input
	mal_hspec_gpio_init_s gpio_init;
	gpio_init.gpio = *gpio;
	gpio_init.direction = MAL_HSPEC_GPIO_DIR_IN;
	gpio_init.pupd = MAL_HSPEC_GPIO_PUPD_NONE;
	gpio_init.speed = UINT64_MAX;
	return mal_gpio_init(&gpio_init);
}
