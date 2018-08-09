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

#ifndef MAL_MAL_HSPEC_STM32F7_CLOCK_H
#define MAL_MAL_HSPEC_STM32F7_CLOCK_H

#include "std/mal_bool.h"
#include "std/mal_types.h"

typedef struct MAL_CLOCK_CONFIG {
    bool set_apb1;
    mal_hertz_t apb1_frequency;
    bool set_apb2;
    mal_hertz_t apb2_frequency;
} mal_clock_config_s;

#endif //MAL_MAL_HSPEC_STM32F7_CLOCK_H
