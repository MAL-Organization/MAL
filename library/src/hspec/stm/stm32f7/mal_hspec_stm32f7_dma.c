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

#include "mal_hspec_stm32f7_dma.h"

static void mal_hspec_stm32f7_dma_transfer_complete_callback(DMA_HandleTypeDef *hal_dma);

mal_hspec_stm32f7_dma_stream_s *mal_hspec_stm32f7_dma_get_channel(mal_hspec_stm32f7_dma_stream_s *streams,
                                                                  const mal_hspec_stm32f7_dma_location_s *locations,
                                                                  uint8_t locations_size) {
    uint8_t index;
    for (index = 0; index < locations_size; index++) {
        // Get stream index
        uint8_t stream_index = (uint8_t)(MAL_HSPEC_STM32F7_DMA_CHANNEL_STREAM_SIZE * locations[index].dma);
        stream_index += locations[index].stream;
        // Check if stream is free
        if (streams[stream_index].used) {
            continue;
        }
        // Stream found
        streams[stream_index].location = locations + index;
        streams[stream_index].used = true;
        return streams + stream_index;
    }
    return NULL;
}

IRQn_Type mal_hspec_stm32f7_dma_get_irq(mal_hspec_stm32f7_dma_stream_s *stream) {
    switch ((uint32_t)stream->hal_stream) {
        case DMA1_Stream0_BASE:
            return DMA1_Stream0_IRQn;
        case DMA1_Stream1_BASE:
            return DMA1_Stream1_IRQn;
        case DMA1_Stream2_BASE:
            return DMA1_Stream2_IRQn;
        case DMA1_Stream3_BASE:
            return DMA1_Stream3_IRQn;
        case DMA1_Stream4_BASE:
            return DMA1_Stream4_IRQn;
        case DMA1_Stream5_BASE:
            return DMA1_Stream5_IRQn;
        case DMA1_Stream6_BASE:
            return DMA1_Stream6_IRQn;
        case DMA1_Stream7_BASE:
            return DMA1_Stream7_IRQn;
        case DMA2_Stream0_BASE:
            return DMA2_Stream0_IRQn;
        case DMA2_Stream1_BASE:
            return DMA2_Stream1_IRQn;
        case DMA2_Stream2_BASE:
            return DMA2_Stream2_IRQn;
        case DMA2_Stream3_BASE:
            return DMA2_Stream3_IRQn;
        case DMA2_Stream4_BASE:
            return DMA2_Stream4_IRQn;
        case DMA2_Stream5_BASE:
            return DMA2_Stream5_IRQn;
        case DMA2_Stream6_BASE:
            return DMA2_Stream6_IRQn;
        default:
        case DMA2_Stream7_BASE:
            return DMA2_Stream7_IRQn;
    }
}

void mal_hspec_stm32f7_dma_set_callback(mal_hspec_stm32f7_dma_stream_s *stream,
                                        mal_hspec_stm32f7_dma_irq_callback_t callback,
                                        void *handle) {
    stream->callback = callback;
    stream->handle = handle;
}

void mal_hspec_stm32f7_dma_start(mal_hspec_stm32f7_dma_stream_s *stream,
                                 DMA_HandleTypeDef *hal_dma,
                                 uint32_t source,
                                 uint32_t destination,
                                 uint32_t length) {
    stream->hal_dma = hal_dma;
    stream->hal_dma->Parent = stream;
    stream->hal_dma->XferCpltCallback = &mal_hspec_stm32f7_dma_transfer_complete_callback;
    HAL_DMA_Start_IT(hal_dma, source, destination, length);
}

static void mal_hspec_stm32f7_dma_transfer_complete_callback(DMA_HandleTypeDef *hal_dma) {
    mal_hspec_stm32f7_dma_stream_s *stream = hal_dma->Parent;
    stream->callback(stream->handle);
}

void mal_hspec_stm32f7_dma_free_channel(mal_hspec_stm32f7_dma_stream_s *stream) {
    if (NULL != stream) {
        stream->used = false;
        stream->callback = NULL;
        stream->handle = NULL;
    }
}
