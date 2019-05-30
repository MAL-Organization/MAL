/*
 * Copyright (c) 2019 Olivier Allaire
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

#include "ram/mal_ram.h"
#include "std/mal_bool.h"

static void more_ram_usage(void) {
    uint32_t waste_size = 10000;
    uint8_t use_some_stack[waste_size];
    for (int i = 0; i < waste_size; i++) {
        use_some_stack[i] = 0;
    }
    // Read stack usage
    mal_ratio_t value = mal_ram_get_stack_usage();
    unsigned int percent = MAL_TYPES_RATIO_OF_INT_VALUE(value, uint8_t, 100);
    if (percent >= 95) {
        return;
    }
    more_ram_usage();
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "OCDFAInspection"
int main(void) {
    mal_ram_init_stack_monitor();

    // Read stack usage
    more_ram_usage();
    while (true);

    return 0;
}
#pragma clang diagnostic pop
