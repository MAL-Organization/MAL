/*
 * mal_hspec_stm32f7_startup.c
 *
 *  Created on: May 3, 2018
 *      Author: olivi
 */
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

#include "startup/mal_startup.h"
#include "stm32f7/system_stm32f7xx.h"

static void initialise_memory(void);

// Code from newlib.
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

void mal_startup_hardware(void) {
    SystemInit();
    initialise_memory();
    mal_clock_initialise_system_clock();
}

// This code is copied from newlib.
static void initialise_memory(void) {
    // Copy the DATA segment from Flash to RAM (inlined).
    unsigned int *data_start = &_sdata;
    unsigned int *source_data = &_sidata;
    while (data_start < &_edata) {
        *(data_start++) = *(source_data++);
    }
    // Zero fill the BSS section (inlined).
    data_start = &__bss_start__;
    while (data_start < &__bss_end__) {
        *(data_start++) = 0;
    }
}
