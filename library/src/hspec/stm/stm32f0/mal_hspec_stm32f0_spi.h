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

#ifndef MAL_MAL_HSPEC_STM32F0_SPI_H
#define MAL_MAL_HSPEC_STM32F0_SPI_H

#include "stm32f0/stm32f0xx_spi.h"
#include "spi/mal_spi.h"
#include "mal_hspec_stm32f0_gpio.h"

typedef struct MAL_SPI {
    SPI_TypeDef *spi_typedef;
    IRQn_Type irq;
    // Runtime settings
    mal_gpio_s select;
    mal_spi_select_mode_e select_mode;
    mal_spi_select_polarity_e select_polarity;
    // Runtime variables
    volatile bool is_active;
    mal_spi_msg_s *active_msg;
    uint8_t out_data_ptr;
    uint8_t in_data_ptr;
} mal_spi_s;

typedef struct MAL_SPI_INTERRUPT_STATE {
    bool active;
} mal_spi_interrupt_state_s;

#endif //MAL_MAL_HSPEC_STM32F0_SPI_H
