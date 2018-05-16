/*
 * mal_hspec.h
 *
 *  Created on: Mar 7, 2018
 *      Author: Olivier
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

#ifndef HSPEC_MAL_HSPEC_H_
#define HSPEC_MAL_HSPEC_H_

#if defined(MAL_STM)
#include "hspec/stm/mal_hspec_stm.h"
#elif defined(MAL_GNU)
#include "hspec/gnu/mal_hspec_gnu.h"
#endif

#endif /* HSPEC_MAL_HSPEC_H_ */
