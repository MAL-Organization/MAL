/*
 * mal_hspec_tiam335xpru_int.h
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

#ifndef SRC_HSPEC_TI_AM335X_PRU_MAL_HSPEC_TIAM335XPRU_INT_H_
#define SRC_HSPEC_TI_AM335X_PRU_MAL_HSPEC_TIAM335XPRU_INT_H_

void __attribute__((weak)) mal_hspec_tiam335xpru_dcan_isr0(void);

void mal_hspec_tiam335xpru_handle_interrupts(void);

#endif /* SRC_HSPEC_TI_AM335X_PRU_MAL_HSPEC_TIAM335XPRU_INT_H_ */
