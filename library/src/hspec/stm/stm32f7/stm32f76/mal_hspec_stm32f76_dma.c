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

#include "hspec/stm/stm32f7/mal_hspec_stm32f7_dma.h"

#define MAL_HSPEC_STM32F76_DMA_CONTROLLER_SIZE  2
#define MAL_HSPEC_STM32F76_DMA1_CHANNEL_SIZE    10
#define MAL_HSPEC_STM32F76_DMA2_CHANNEL_SIZE    12

static mal_hspec_stm32f7_dma_stream_s dma1_streams[MAL_HSPEC_STM32F7_DMA_CHANNEL_STREAM_SIZE];
static mal_hspec_stm32f7_dma_stream_s dma2_streams[MAL_HSPEC_STM32F7_DMA_CHANNEL_STREAM_SIZE];

static const mal_hspec_stm32f7_dma_location_s usart1_tx_locations[] = {
    {
        .dma = 1,
        .stream = 7,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart1_rx_locations[] = {
    {
        .dma = 1,
        .stream = 2,
        .channel = 4
    },
    {
        .dma = 1,
        .stream = 5,
        .channel = 4
    }
};

static const mal_hspec_stm32f7_dma_location_s usart2_tx_locations[] = {
    {
        .dma = 0,
        .stream = 6,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart2_rx_locations[] = {
    {
        .dma = 0,
        .stream = 5,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart3_tx_locations[] = {
    {
        .dma = 0,
        .stream = 3,
        .channel = 4
    },
    {
        .dma = 0,
        .stream = 4,
        .channel = 7
    }
};

static const mal_hspec_stm32f7_dma_location_s usart3_rx_locations[] = {
    {
        .dma = 0,
        .stream = 1,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart4_tx_locations[] = {
    {
        .dma = 0,
        .stream = 4,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart4_rx_locations[] = {
    {
        .dma = 0,
        .stream = 2,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart5_tx_locations[] = {
    {
        .dma = 0,
        .stream = 7,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart5_rx_locations[] = {
    {
        .dma = 0,
        .stream = 0,
        .channel = 4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart6_tx_locations[] = {
    {
        .dma = 1,
        .stream = 6,
        .channel = 5
    },
    {
        .dma = 1,
        .stream = 7,
        .channel = 5
    }
};

static const mal_hspec_stm32f7_dma_location_s usart6_rx_locations[] = {
    {
        .dma = 1,
        .stream = 1,
        .channel = 5
    },
    {
        .dma = 1,
        .stream = 2,
        .channel = 5
    }
};

static const mal_hspec_stm32f7_dma_location_s usart7_tx_locations[] = {
    {
        .dma = 0,
        .stream = 1,
        .channel = 5
    },
};

static const mal_hspec_stm32f7_dma_location_s usart7_rx_locations[] = {
    {
        .dma = 0,
        .stream = 3,
        .channel = 5
    },
};

static const mal_hspec_stm32f7_dma_location_s usart8_tx_locations[] = {
    {
        .dma = 0,
        .stream = 0,
        .channel = 5
    },
};

static const mal_hspec_stm32f7_dma_location_s usart8_rx_locations[] = {
    {
        .dma = 0,
        .stream = 6,
        .channel = 5
    },
};

mal_error_e mal_hspec_stm32f0_dma_get_serial_channel(mal_serial_port_e port,
                                                     uint32_t *tx_channel,
                                                     DMA_Stream_TypeDef **tx_channel_stream,
                                                     uint32_t *rx_channel,
                                                     DMA_Stream_TypeDef **rx_channel_stream) {
    switch (port) {
        case MAL_SERIAL_PORT_1:
            *tx_channel = mal_hspec_stm32f0_dma_get_channel(usart_1_tx_channels, sizeof(usart_1_tx_channels));
            *rx_channel = mal_hspec_stm32f0_dma_get_channel(usart_1_rx_channels, sizeof(usart_1_rx_channels));
            break;
            // Port 2 and 3 have dependencies between channels. See DMA channels section of user manual.
        case MAL_SERIAL_PORT_2:
            *tx_channel = mal_hspec_stm32f0_dma_get_channel(usart_2_tx_channels, sizeof(usart_2_tx_channels));
            *rx_channel = mal_hspec_stm32f0_dma_get_channel(usart_2_rx_channels, sizeof(usart_2_rx_channels));
            if ((DMA1_Channel4 == *tx_channel && DMA1_Channel5 != *rx_channel) ||
                (DMA1_Channel7 == *tx_channel && DMA1_Channel6 != *rx_channel)) {
                return MAL_ERROR_HARDWARE_UNAVAILABLE;
            }
            break;
        case MAL_SERIAL_PORT_3:
            *tx_channel = mal_hspec_stm32f0_dma_get_channel(usart_3_tx_channels, sizeof(usart_3_tx_channels));
            *rx_channel = mal_hspec_stm32f0_dma_get_channel(usart_3_rx_channels, sizeof(usart_3_rx_channels));
            if ((DMA1_Channel2 == *tx_channel && DMA1_Channel3 != *rx_channel) ||
                (DMA1_Channel7 == *tx_channel && DMA1_Channel6 != *rx_channel)) {
                return MAL_ERROR_HARDWARE_UNAVAILABLE;
            }
            break;
        case MAL_SERIAL_PORT_4:
            *tx_channel = mal_hspec_stm32f0_dma_get_channel(usart_4_tx_channels, sizeof(usart_4_tx_channels));
            *rx_channel = mal_hspec_stm32f0_dma_get_channel(usart_4_rx_channels, sizeof(usart_4_rx_channels));
            break;
        default:
            break;
    }
    if (NULL == *tx_channel || NULL == *rx_channel) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    return MAL_ERROR_OK;
}
