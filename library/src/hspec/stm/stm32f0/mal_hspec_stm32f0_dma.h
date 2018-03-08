/*
 * mal_hspec_stm32f0_dma.h
 *
 *  Created on: Mar 1, 2018
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_DMA_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_DMA_H_

#include "stm32f0/stm32f0xx_dma.h"
#include "serial/mal_serial.h"
#include "std/mal_error.h"

typedef void (*mal_hspec_stm32f0_dma_irq_callback_t)(void *handle);

mal_error_e mal_hspec_stm32f0_dma_get_serial_channel(mal_serial_port_e port,
                                                     DMA_Channel_TypeDef **tx_channel,
                                                     DMA_Channel_TypeDef **rx_channel);

void mal_hspec_stm32f0_dma_free_channel(DMA_Channel_TypeDef *channel);

void mal_hspec_stm32f0_dma_remap_serial(DMA_Channel_TypeDef *dma_channel, mal_serial_port_e port);

IRQn_Type mal_hspec_stm32f0_dma_get_irq(DMA_Channel_TypeDef *channel);

void mal_hspec_stm32f0_dma_set_callback(DMA_Channel_TypeDef *channel,
                                        mal_hspec_stm32f0_dma_irq_callback_t callback,
                                        void *handle);

uint32_t mal_hspec_stm32f0_dma_get_flag(DMA_Channel_TypeDef *channel);

void mal_hspec_stm32f0_dma_enable_clock(DMA_Channel_TypeDef *channel);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_DMA_H_ */
