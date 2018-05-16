/*
 * mal_hspec_mingw_spi.h
 *
 *  Created on: May 19, 2016
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_SPI_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_SPI_H_

#include "spi/mal_spi.h"

mal_error_e mal_hspec_mingw_spi_get_msg(mal_spi_e interface, mal_spi_msg_s **msg);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_SPI_H_ */
