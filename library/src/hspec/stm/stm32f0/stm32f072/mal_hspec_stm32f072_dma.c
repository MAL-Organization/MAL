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

#include "hspec/stm/stm32f0/mal_hspec_stm32f0_dma.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"
#include "stm32f0/stm32f0xx_rcc.h"

#define HSPEC_STM32F0_DMA_CHANNEL_SIZE  7

typedef struct {
    bool used;
    DMA_Channel_TypeDef *handle;
    mal_hspec_stm32f0_dma_irq_callback_t callback;
    void *callback_handle;
} dma_channel_s;

static DMA_Channel_TypeDef* mal_hspec_stm32f0_dma_get_channel(uint8_t *channel_indexes, uint8_t channels_size);

static dma_channel_s dma_channels[HSPEC_STM32F0_DMA_CHANNEL_SIZE] = {
    {
        .used = false,
        .handle = DMA1_Channel1
    },
    {
        .used = false,
        .handle = DMA1_Channel2
    },
    {
        .used = false,
        .handle = DMA1_Channel3
    },
    {
        .used = false,
        .handle = DMA1_Channel4
    },
    {
        .used = false,
        .handle = DMA1_Channel5
    },
    {
        .used = false,
        .handle = DMA1_Channel6
    },
    {
        .used = false,
        .handle = DMA1_Channel7
    }
};

static uint8_t usart_1_tx_channels[] = {
    1, 3
};

static uint8_t usart_1_rx_channels[] = {
    2, 4
};

static uint8_t usart_2_tx_channels[] = {
    3, 6
};

static uint8_t usart_2_rx_channels[] = {
    4, 5
};

static uint8_t usart_3_tx_channels[] = {
    1, 6
};

static uint8_t usart_3_rx_channels[] = {
    2, 5
};

static uint8_t usart_4_tx_channels[] = {
    6
};

static uint8_t usart_4_rx_channels[] = {
    5
};

static DMA_Channel_TypeDef* mal_hspec_stm32f0_dma_get_channel(uint8_t *channel_indexes, uint8_t channels_size) {
    uint8_t index;
    for (index = 0; index < channels_size; index++) {
        uint8_t dma_channel_index = channel_indexes[index];
        if (!dma_channels[dma_channel_index].used) {
            dma_channels[dma_channel_index].used = true;
            return dma_channels[dma_channel_index].handle;
        }
    }
    return NULL;
}

mal_error_e mal_hspec_stm32f0_dma_get_serial_channel(mal_serial_port_e port,
                                                     DMA_Channel_TypeDef **tx_channel,
                                                     DMA_Channel_TypeDef **rx_channel) {
    *tx_channel = NULL;
    *rx_channel = NULL;
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

void mal_hspec_stm32f0_dma_free_channel(DMA_Channel_TypeDef *channel) {
    uint8_t index;
    for (index = 0; index < HSPEC_STM32F0_DMA_CHANNEL_SIZE; index++) {
        if (channel == dma_channels[index].handle) {
            dma_channels[index].callback = NULL;
            dma_channels[index].callback_handle = NULL;
            dma_channels[index].used = false;
            return;
        }
    }
}

void mal_hspec_stm32f0_dma_remap_serial(DMA_Channel_TypeDef *dma_channel, mal_serial_port_e port) {
    // Ensure clock is enabled
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    // Remap
    switch ((uint32_t)dma_channel) {
        case DMA1_Channel2_BASE:
            if (MAL_SERIAL_PORT_1 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Tx, DISABLE);
            } else if (MAL_SERIAL_PORT_3 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART3, ENABLE);
            }
            break;
        case DMA1_Channel3_BASE:
            if (MAL_SERIAL_PORT_1 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Rx, DISABLE);
            } else if (MAL_SERIAL_PORT_3 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART3, ENABLE);
            }
            break;
        case DMA1_Channel4_BASE:
            if (MAL_SERIAL_PORT_1 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Tx, ENABLE);
            } else if (MAL_SERIAL_PORT_2 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART2, DISABLE);
            }
            break;
        case DMA1_Channel5_BASE:
            if (MAL_SERIAL_PORT_1 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Rx, ENABLE);
            } else if (MAL_SERIAL_PORT_2 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART2, DISABLE);
            }
            break;
        case DMA1_Channel6_BASE:
            if (MAL_SERIAL_PORT_2 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART2, ENABLE);
            } else if (MAL_SERIAL_PORT_3 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART3, DISABLE);
            }
            break;
        case DMA1_Channel7_BASE:
            if (MAL_SERIAL_PORT_2 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART2, ENABLE);
            } else if (MAL_SERIAL_PORT_3 == port) {
                SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART3, DISABLE);
            }
            break;
        default:
            break;
    }
}

IRQn_Type mal_hspec_stm32f0_dma_get_irq(DMA_Channel_TypeDef *channel) {
    switch ((uint32_t)channel) {
        case DMA1_Channel1_BASE:
            return DMA1_Channel1_IRQn;
        case DMA1_Channel2_BASE:
        case DMA1_Channel3_BASE:
            return DMA1_Channel2_3_IRQn;
        case DMA1_Channel4_BASE:
        case DMA1_Channel5_BASE:
        case DMA1_Channel6_BASE:
        case DMA1_Channel7_BASE:
        default:
            return DMA1_Channel4_5_6_7_IRQn;
    }
}

uint32_t mal_hspec_stm32f0_dma_get_flag(DMA_Channel_TypeDef *channel) {
    switch ((uint32_t)channel) {
        case DMA1_Channel1_BASE:
            return DMA1_IT_GL1;
        case DMA1_Channel2_BASE:
            return DMA1_IT_GL2;
        case DMA1_Channel3_BASE:
            return DMA1_IT_GL3;
        case DMA1_Channel4_BASE:
            return DMA1_IT_GL4;
        case DMA1_Channel5_BASE:
            return DMA1_IT_GL5;
        case DMA1_Channel6_BASE:
            return DMA1_IT_GL5;
        case DMA1_Channel7_BASE:
        default:
            return DMA1_IT_GL6;
    }
}

void mal_hspec_stm32f0_dma_set_callback(DMA_Channel_TypeDef *channel,
                                        mal_hspec_stm32f0_dma_irq_callback_t callback,
                                        void *handle) {
    switch ((uint32_t)channel) {
        case DMA1_Channel1_BASE:
            dma_channels[0].callback = callback;
            dma_channels[0].callback_handle = handle;
            break;
        case DMA1_Channel2_BASE:
            dma_channels[1].callback = callback;
            dma_channels[1].callback_handle = handle;
            break;
        case DMA1_Channel3_BASE:
            dma_channels[2].callback = callback;
            dma_channels[2].callback_handle = handle;
            break;
        case DMA1_Channel4_BASE:
            dma_channels[3].callback = callback;
            dma_channels[3].callback_handle = handle;
            break;
        case DMA1_Channel5_BASE:
            dma_channels[4].callback = callback;
            dma_channels[4].callback_handle = handle;
            break;
        case DMA1_Channel6_BASE:
            dma_channels[5].callback = callback;
            dma_channels[5].callback_handle = handle;
            break;
        case DMA1_Channel7_BASE:
        default:
            dma_channels[6].callback = callback;
            dma_channels[6].callback_handle = handle;
            break;
    }
}

void mal_hspec_stm32f0_dma_enable_clock(DMA_Channel_TypeDef *channel) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void DMA1_Channel1_IRQHandler(void) {
    if (NULL != dma_channels[0].callback) {
        dma_channels[0].callback(dma_channels[0].callback_handle);
    }
}

void DMA1_Channel2_3_IRQHandler(void) {
    if (NULL != dma_channels[1].callback) {
        dma_channels[1].callback(dma_channels[1].callback_handle);
    }
    if (NULL != dma_channels[2].callback) {
        dma_channels[2].callback(dma_channels[2].callback_handle);
    }
}

void DMA1_Channel4_5_6_7_IRQHandler(void) {
    if (NULL != dma_channels[3].callback) {
        dma_channels[3].callback(dma_channels[3].callback_handle);
    }
    if (NULL != dma_channels[4].callback) {
        dma_channels[4].callback(dma_channels[4].callback_handle);
    }
    if (NULL != dma_channels[5].callback) {
        dma_channels[5].callback(dma_channels[5].callback_handle);
    }
    if (NULL != dma_channels[6].callback) {
        dma_channels[6].callback(dma_channels[6].callback_handle);
    }
}
