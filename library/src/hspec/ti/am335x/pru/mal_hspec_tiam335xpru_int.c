/*
 * mal_hspec_tiam335xpru_int.c
 *
 *  Created on: Feb 8, 2018
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

#include "mal_hspec_tiam335xpru_int.h"

#include "std/mal_stdint.h"

#include "soc_AM335x.h"

#include "dcan.h"

void __attribute__((weak)) mal_hspec_tiam335xpru_dcan_isr0(void) {

}

void mal_hspec_tiam335xpru_handle_interrupts(void) {
    // Check DCAN1 interrupts
    if (DCANIntRegStatusGet(SOC_DCAN_1_REGS, DCAN_INT_LINE0_STAT)) {
        mal_hspec_tiam335xpru_dcan_isr0();
    }
}

