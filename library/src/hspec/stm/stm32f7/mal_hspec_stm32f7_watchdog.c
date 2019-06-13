/*
 * Copyright (c) 2019 Olivier Allaire
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

#include "watchdog/mal_watchdog.h"
#include "stm32f7/stm32f7xx_hal_iwdg.h"
#include "std/mal_bool.h"

mal_error_e mal_watchdog_init(uint32_t timeout_ms) {
    // Find prescaler and reload values
    // Prescaler values can only be power of 2 from 4 to 256
    bool found = false;
    uint8_t prescaler;
    uint16_t reload = 0;
    for (prescaler = IWDG_PRESCALER_4; prescaler <= IWDG_PRESCALER_256; prescaler++) {
        // Select divider value
        uint32_t divider;
        switch (prescaler) {
            case IWDG_PRESCALER_4:
                divider = 4;
                break;
            case IWDG_PRESCALER_8:
                divider = 8;
                break;
            case IWDG_PRESCALER_16:
                divider = 16;
                break;
            case IWDG_PRESCALER_32:
                divider = 32;
                break;
            case IWDG_PRESCALER_64:
                divider = 64;
                break;
            case IWDG_PRESCALER_128:
                divider = 128;
                break;
            case IWDG_PRESCALER_256:
            default:
                divider = 256;
                break;
        }
        // See if a reload value matches the requested reset period
        for (reload = 1; reload <= 0x0FFF; reload++) {
            uint32_t current_timeout_ms = (uint32_t)reload * divider;
            current_timeout_ms *= 1000;
            current_timeout_ms /= LSI_VALUE;
            // Check if this is a match
            if (current_timeout_ms == timeout_ms) {
                found = true;
                break;
            }
        }
        // Check if values are correct
        if (found) {
            break;
        }
    }
    // Check if timeout is possible
    if (!found) {
        return MAL_ERROR_CLOCK_ERROR;
    }
    // Initialise
    IWDG->KR = IWDG_KEY_ENABLE;
    IWDG->KR = IWDG_KEY_WRITE_ACCESS_ENABLE;
    IWDG->PR = prescaler;
    IWDG->RLR = reload;
    // Fixme find non blocking way
    while(RESET != IWDG->SR);
    if(IWDG_WINDOW_DISABLE != IWDG->WINR) {
        IWDG->WINR = IWDG_WINDOW_DISABLE;
    } else {
        IWDG->KR = IWDG_KEY_RELOAD;
    }
    return MAL_ERROR_OK;
}

void mal_watchdog_feed(void) {
    IWDG->KR = IWDG_KEY_RELOAD;
}
