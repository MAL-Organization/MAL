#ifndef MAL_MAL_HSPEC_STM32F7_FLASH_H
#define MAL_MAL_HSPEC_STM32F7_FLASH_H

#include "std/mal_stdint.h"
#include "std/mal_bool.h"

uint32_t mal_hspec_stm32f7_flash_get_sector(uint32_t page);
bool mal_hspec_stm32f7_flash_is_dual_boot(void);

#endif //MAL_MAL_HSPEC_STM32F7_FLASH_H
