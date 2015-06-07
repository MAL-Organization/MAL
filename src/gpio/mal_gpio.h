/*
 * mal_gpio.h
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
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAL_GPIO_H_
#define MAL_GPIO_H_

#include "hspec/mal_hspec.h"

#define mal_gpio_init(init) mal_hspec_gpio_init(init)

#define mal_gpio_set(gpio, value) mal_hspec_set_gpio(gpio, value)

#define mal_gpio_get(gpio) mal_hspec_get_gpio(gpio)

#define mal_gpio_toggle(gpio) mal_hspec_toggle_gpio(gpio)

#endif /* MAL_GPIO_H_ */
