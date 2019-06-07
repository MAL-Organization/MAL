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

#include "mal_hspec_stm32f0_ram.h"

// Code from newlib...
// defined in linker script
extern unsigned int _sidata;
// Begin address for the .data section; defined in linker script
extern unsigned int _sdata;
// End address for the .data section; defined in linker script
extern unsigned int _edata;

// Begin address for the .bss section; defined in linker script
extern unsigned int __bss_start__;
// End address for the .bss section; defined in linker script
extern unsigned int __bss_end__;

unsigned int mal_ram_get_stack_start_address(void) {
    unsigned int data_end_address = (unsigned int)&_edata;
    unsigned int bss_end_address = (unsigned int)&__bss_end__;
    if (data_end_address > bss_end_address) {
        return data_end_address;
    }
    return bss_end_address;
}

unsigned int mal_ram_get_stack_end_address(void) {
    return 0x20000000 + mal_hspec_stm32f0_ram_get_ram_size() - 1;
}
