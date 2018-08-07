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

#ifndef MAL_MAL_HSPEC_STM32F7_DMA_H
#define MAL_MAL_HSPEC_STM32F7_DMA_H

#include "std/mal_error.h"
#include "serial/mal_serial.h"
#include "stm32f7/stm32f7xx_hal_dma.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"

#define MAL_HSPEC_STM32F7_DMA_CHANNEL_STREAM_SIZE   8

typedef void (*mal_hspec_stm32f7_dma_irq_callback_t)(void *handle);

typedef struct {
    uint8_t dma;
    uint8_t stream;
    uint32_t channel;
} mal_hspec_stm32f7_dma_location_s;

typedef struct {
    bool used;
    DMA_Stream_TypeDef *hal_stream;
    DMA_HandleTypeDef *hal_dma;
    mal_hspec_stm32f7_dma_irq_callback_t callback;
    void *handle;
    const mal_hspec_stm32f7_dma_location_s *location;
} mal_hspec_stm32f7_dma_stream_s;

mal_error_e mal_hspec_stm32f7_dma_get_serial_stream(mal_serial_port_e port,
                                                    mal_hspec_stm32f7_dma_stream_s **tx_channel_stream,
                                                    mal_hspec_stm32f7_dma_stream_s **rx_channel_stream);

mal_hspec_stm32f7_dma_stream_s *mal_hspec_stm32f7_dma_get_channel(mal_hspec_stm32f7_dma_stream_s *streams,
                                                                  const mal_hspec_stm32f7_dma_location_s *locations,
                                                                  uint8_t locations_size);

void mal_hspec_stm32f7_dma_enable_clock(mal_hspec_stm32f7_dma_stream_s *stream);

IRQn_Type mal_hspec_stm32f7_dma_get_irq(mal_hspec_stm32f7_dma_stream_s *stream);

void mal_hspec_stm32f7_dma_set_callback(mal_hspec_stm32f7_dma_stream_s *stream,
                                        mal_hspec_stm32f7_dma_irq_callback_t callback,
                                        void *handle);

void mal_hspec_stm32f7_dma_start(mal_hspec_stm32f7_dma_stream_s *stream,
                                 DMA_HandleTypeDef *hal_dma,
                                 uint32_t source,
                                 uint32_t destination,
                                 uint32_t length);

void mal_hspec_stm32f7_dma_free_channel(mal_hspec_stm32f7_dma_stream_s *stream);

#endif //MAL_MAL_HSPEC_STM32F7_DMA_H
