/*
 * mal_hspec_stm32f0_spi.h
 *
 *  Created on: Mar 30, 2016
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_SPI_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_SPI_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "stm32f0/stm32f0xx_misc.h"
#include "std/mal_bool.h"

#define mal_hspec_stm32f0_spi_enable_interrupt(interface, active) do {\
	if (active) { \
		NVIC_EnableIRQ(mal_hspec_stm32f0_spi_get_irq(interface)); \
	} \
} while(0)

mal_error_e mal_hspec_stm32f0_spi_master_init(mal_hspec_spi_master_init_s *init);

mal_error_e mal_hspec_stm32f0_spi_send_msg(mal_hspec_spi_e interface, mal_hspec_spi_msg_s *msg);

IRQn_Type mal_hspec_stm32f0_spi_get_irq(mal_hspec_spi_e interface);

bool mal_hspec_stm32f0_spi_disable_interrupt(mal_hspec_spi_e interface);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_SPI_H_ */
