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

#include "mal_hspec_stm32f7_ram.h"
#include "stm32f7/stm32f7xx.h"

// Code from newlib.
// defined in linker script
extern unsigned int __data_regions_array_start;
extern unsigned int __data_regions_array_end;
extern unsigned int __bss_regions_array_start;
extern unsigned int __bss_regions_array_end;

static unsigned int *mal_hspec_stm32f7_ram_start_address = 0;

unsigned int mal_ram_get_stack_start_address(void) {
    if (0 == mal_hspec_stm32f7_ram_start_address) {
        for (unsigned int *p = &__data_regions_array_start; p < &__data_regions_array_end;) {
            // From
            p++;
            // Region begin
            p++;
            unsigned int *region_end = (unsigned int *) (*p++);
            if (region_end > mal_hspec_stm32f7_ram_start_address) {
                mal_hspec_stm32f7_ram_start_address = region_end;
            }
        }
        // Zero fill all bss segments
        for (unsigned int *p = &__bss_regions_array_start; p < &__bss_regions_array_end;) {
            // Region begin
            p++;
            unsigned int *region_end = (unsigned int *) (*p++);
            if (region_end > mal_hspec_stm32f7_ram_start_address) {
                mal_hspec_stm32f7_ram_start_address = region_end;
            }
        }
    }
    return (unsigned int)mal_hspec_stm32f7_ram_start_address;
}

unsigned int mal_ram_get_stack_end_address(void) {
    return 0x20000000 + mal_hspec_stm32f7_ram_get_ram_size() - 1;
}

unsigned int mal_ram_get_stack_pointer_address(void) {
    return __get_MSP();
}
