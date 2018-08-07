#ifndef MAL_MAL_HSPEC_STM32F7_DMA_DEFINITIONS_H
#define MAL_MAL_HSPEC_STM32F7_DMA_DEFINITIONS_H

#include "std/mal_bool.h"
#include "std/mal_stdint.h"
#include "stm32f7/stm32f7xx_hal_dma.h"

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

#endif //MAL_MAL_HSPEC_STM32F7_DMA_DEFINITIONS_H
