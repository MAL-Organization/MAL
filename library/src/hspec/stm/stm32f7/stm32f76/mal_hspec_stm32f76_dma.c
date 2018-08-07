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
#define MAL_HSPEC_STM32F76_DMA_STREAMS_SIZE     (MAL_HSPEC_STM32F76_DMA_CONTROLLER_SIZE * MAL_HSPEC_STM32F7_DMA_CHANNEL_STREAM_SIZE)

static mal_hspec_stm32f7_dma_stream_s dma_streams[MAL_HSPEC_STM32F76_DMA_STREAMS_SIZE] = {
    { // DMA1
        .used = false,
        .hal_stream = DMA1_Stream0
    },
    {
        .used = false,
        .hal_stream = DMA1_Stream1
    },
    {
        .used = false,
        .hal_stream = DMA1_Stream2
    },
    {
        .used = false,
        .hal_stream = DMA1_Stream3
    },
    {
        .used = false,
        .hal_stream = DMA1_Stream4
    },
    {
        .used = false,
        .hal_stream = DMA1_Stream5
    },
    {
        .used = false,
        .hal_stream = DMA1_Stream6
    },
    {
        .used = false,
        .hal_stream = DMA1_Stream7
    },
    { // DMA2
        .used = false,
        .hal_stream = DMA2_Stream0
    },
    {
        .used = false,
        .hal_stream = DMA2_Stream1
    },
    {
        .used = false,
        .hal_stream = DMA2_Stream2
    },
    {
        .used = false,
        .hal_stream = DMA2_Stream3
    },
    {
        .used = false,
        .hal_stream = DMA2_Stream4
    },
    {
        .used = false,
        .hal_stream = DMA2_Stream5
    },
    {
        .used = false,
        .hal_stream = DMA2_Stream6
    },
    {
        .used = false,
        .hal_stream = DMA2_Stream7
    }
};

static const mal_hspec_stm32f7_dma_location_s usart1_tx_locations[] = {
    {
        .dma = 1,
        .stream = 7,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart1_rx_locations[] = {
    {
        .dma = 1,
        .stream = 2,
        .channel = DMA_CHANNEL_4
    },
    {
        .dma = 1,
        .stream = 5,
        .channel = DMA_CHANNEL_4
    }
};

static const mal_hspec_stm32f7_dma_location_s usart2_tx_locations[] = {
    {
        .dma = 0,
        .stream = 6,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart2_rx_locations[] = {
    {
        .dma = 0,
        .stream = 5,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart3_tx_locations[] = {
    {
        .dma = 0,
        .stream = 3,
        .channel = DMA_CHANNEL_4
    },
    {
        .dma = 0,
        .stream = 4,
        .channel = DMA_CHANNEL_7
    }
};

static const mal_hspec_stm32f7_dma_location_s usart3_rx_locations[] = {
    {
        .dma = 0,
        .stream = 1,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart4_tx_locations[] = {
    {
        .dma = 0,
        .stream = 4,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart4_rx_locations[] = {
    {
        .dma = 0,
        .stream = 2,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart5_tx_locations[] = {
    {
        .dma = 0,
        .stream = 7,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart5_rx_locations[] = {
    {
        .dma = 0,
        .stream = 0,
        .channel = DMA_CHANNEL_4
    },
};

static const mal_hspec_stm32f7_dma_location_s usart6_tx_locations[] = {
    {
        .dma = 1,
        .stream = 6,
        .channel = DMA_CHANNEL_5
    },
    {
        .dma = 1,
        .stream = 7,
        .channel = DMA_CHANNEL_5
    }
};

static const mal_hspec_stm32f7_dma_location_s usart6_rx_locations[] = {
    {
        .dma = 1,
        .stream = 1,
        .channel = DMA_CHANNEL_5
    },
    {
        .dma = 1,
        .stream = 2,
        .channel = DMA_CHANNEL_5
    }
};

static const mal_hspec_stm32f7_dma_location_s usart7_tx_locations[] = {
    {
        .dma = 0,
        .stream = 1,
        .channel = DMA_CHANNEL_5
    },
};

static const mal_hspec_stm32f7_dma_location_s usart7_rx_locations[] = {
    {
        .dma = 0,
        .stream = 3,
        .channel = DMA_CHANNEL_5
    },
};

static const mal_hspec_stm32f7_dma_location_s usart8_tx_locations[] = {
    {
        .dma = 0,
        .stream = 0,
        .channel = DMA_CHANNEL_5
    },
};

static const mal_hspec_stm32f7_dma_location_s usart8_rx_locations[] = {
    {
        .dma = 0,
        .stream = 6,
        .channel = DMA_CHANNEL_5
    },
};

mal_error_e mal_hspec_stm32f7_dma_get_serial_stream(mal_serial_port_e port,
                                                    mal_hspec_stm32f7_dma_stream_s **tx_channel_stream,
                                                    mal_hspec_stm32f7_dma_stream_s **rx_channel_stream) {
    mal_hspec_stm32f7_dma_location_s *tx_locations;
    mal_hspec_stm32f7_dma_location_s *rx_locations;
    uint8_t tx_locations_size;
    uint8_t rx_locations_size;
    *tx_channel_stream = NULL;
    *rx_channel_stream = NULL;
    switch (port) {
        case MAL_SERIAL_PORT_1:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart1_tx_locations;
            tx_locations_size = sizeof(usart1_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart1_rx_locations;
            rx_locations_size = sizeof(usart1_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        case MAL_SERIAL_PORT_2:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart2_tx_locations;
            tx_locations_size = sizeof(usart2_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart2_rx_locations;
            rx_locations_size = sizeof(usart2_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        case MAL_SERIAL_PORT_3:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart3_tx_locations;
            tx_locations_size = sizeof(usart3_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart3_rx_locations;
            rx_locations_size = sizeof(usart3_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        case MAL_SERIAL_PORT_4:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart4_tx_locations;
            tx_locations_size = sizeof(usart4_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart4_rx_locations;
            rx_locations_size = sizeof(usart4_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        case MAL_SERIAL_PORT_5:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart5_tx_locations;
            tx_locations_size = sizeof(usart5_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart5_rx_locations;
            rx_locations_size = sizeof(usart5_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        case MAL_SERIAL_PORT_6:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart6_tx_locations;
            tx_locations_size = sizeof(usart6_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart6_rx_locations;
            rx_locations_size = sizeof(usart6_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        case MAL_SERIAL_PORT_7:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart7_tx_locations;
            tx_locations_size = sizeof(usart7_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart7_rx_locations;
            rx_locations_size = sizeof(usart7_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        case MAL_SERIAL_PORT_8:
            tx_locations = (mal_hspec_stm32f7_dma_location_s *)usart8_tx_locations;
            tx_locations_size = sizeof(usart8_tx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            rx_locations = (mal_hspec_stm32f7_dma_location_s *)usart8_rx_locations;
            rx_locations_size = sizeof(usart8_rx_locations) / sizeof(mal_hspec_stm32f7_dma_location_s);
            break;
        default:
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    *tx_channel_stream = mal_hspec_stm32f7_dma_get_channel(
            dma_streams,
            tx_locations,
            tx_locations_size);
    *rx_channel_stream = mal_hspec_stm32f7_dma_get_channel(
            dma_streams,
            rx_locations,
            rx_locations_size);
    if (NULL == *tx_channel_stream || NULL == *rx_channel_stream) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    return MAL_ERROR_OK;
}

void mal_hspec_stm32f7_dma_enable_clock(mal_hspec_stm32f7_dma_stream_s *stream) {
    if (0 == stream->location->dma) {
        __HAL_RCC_DMA1_CLK_ENABLE();
    } else {
        __HAL_RCC_DMA2_CLK_ENABLE();
    }
}

void DMA1_Stream0_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[0].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA1_Stream1_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[1].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA1_Stream2_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[2].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA1_Stream3_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[3].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA1_Stream4_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[4].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA1_Stream5_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[5].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA1_Stream6_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[6].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA1_Stream7_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[7].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream0_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[8].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream1_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[9].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream2_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[10].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream3_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[11].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream4_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[12].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream5_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[13].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream6_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[14].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}

void DMA2_Stream7_IRQHandler(void) {
    DMA_HandleTypeDef *hal_dma = dma_streams[15].hal_dma;
    if (NULL != hal_dma) {
        HAL_DMA_IRQHandler(hal_dma);
    }
}
