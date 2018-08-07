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
