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
#include "clock/mal_clock.h"
#include "stm32f7/stm32f7xx_hal.h"
#include "stm32f7/stm32f7xx.h"

static void initialise_memory(void);
void mal_stm32f7_relocate_vector_table(void);

// Code from newlib.
// defined in linker script
extern unsigned int __data_regions_array_start;
extern unsigned int __data_regions_array_end;
extern unsigned int __bss_regions_array_start;
extern unsigned int __bss_regions_array_end;

void mal_startup_hardware(void) {
    SystemInit();
    initialise_memory();
    SCB_EnableICache();
    SCB_EnableDCache();
    HAL_Init();
    mal_clock_initialise_system_clock();
    // Relocate vector table
    mal_stm32f7_relocate_vector_table();
}

// This code is copied from newlib.
inline void
__attribute__((always_inline))
__initialize_data (unsigned int* from, unsigned int* region_begin,
           unsigned int* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
  while (p < region_end)
    *p++ = *from++;
}

// This code is copied from newlib.
inline void
__attribute__((always_inline))
__initialize_bss (unsigned int* region_begin, unsigned int* region_end)
{
  // Iterate and clear word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
  while (p < region_end)
    *p++ = 0;
}

// This code is copied from newlib.
static void initialise_memory(void) {
    // Copy the data sections from flash to SRAM.
    for (unsigned int* p = &__data_regions_array_start; p < &__data_regions_array_end;) {
        unsigned int* from = (unsigned int *) (*p++);
        unsigned int* region_begin = (unsigned int *) (*p++);
        unsigned int* region_end = (unsigned int *) (*p++);

        __initialize_data (from, region_begin, region_end);
    }
    // Zero fill all bss segments
    for (unsigned int *p = &__bss_regions_array_start; p < &__bss_regions_array_end;) {
        unsigned int* region_begin = (unsigned int*) (*p++);
        unsigned int* region_end = (unsigned int*) (*p++);
        __initialize_bss (region_begin, region_end);
    }
}
