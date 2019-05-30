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

#include "mal_ram.h"

void mal_ram_init_stack_monitor(void) {
    unsigned int current_address;
    unsigned int start_address = mal_ram_get_stack_start_address();
    unsigned int end_address = mal_ram_get_stack_end_address();
    for (current_address = start_address; current_address <= end_address; current_address++) {
        if (current_address >= mal_ram_get_stack_pointer_address()) {
            return;
        }
        mal_ram_write(current_address, MAL_RAM_STACK_MARKER);
    }
}

unsigned int mal_ram_get_stack_size(void) {
    return mal_ram_get_stack_end_address() - mal_ram_get_stack_start_address() + 1;
}

mal_ratio_t mal_ram_get_stack_usage(void) {
    // Find lowest address used
    uint8_t marker_counter = 0;
    unsigned int current_address;
    unsigned int start_address = mal_ram_get_stack_start_address();
    unsigned int end_address = mal_ram_get_stack_end_address();
    for (current_address = end_address; current_address >= start_address; current_address--) {
        uint8_t value = mal_ram_read(current_address);
        if (MAL_RAM_STACK_MARKER == value) {
            if (++marker_counter >= MAL_RAM_STACK_STOP_COUNT) {
                current_address += MAL_RAM_STACK_STOP_COUNT;
                break;
            }
        } else {
            marker_counter = 0;
        }
    }
    if (current_address < start_address) {
        current_address = start_address;
    }
    current_address = current_address - start_address;
    // Reverse ratio since address is down counting
    unsigned int stack_size = mal_ram_get_stack_size();
    unsigned int used_addresses = stack_size - current_address;
    return MAL_TYPES_INT_RATIO_TO_RATIO(used_addresses, stack_size);
}
