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

#include "std/mal_stdint.h"
#include "std/mal_bool.h"
#include "clock/mal_clock.h"
#include "power/mal_power.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "stm32f7/stm32f7xx_hal_conf.h"
#include "stm32f7/stm32f7xx_hal_flash_ex.h"

#define MAL_HSPEC_STM32F7_CLOCK_MAX_FREQUENCY           216000000
#define MAL_HSPEC_STM32F7_CLOCK_MAX_R_DIVIDER           7
#define MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT27_STEP 30000000ULL
#define MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT24_STEP 24000000ULL
#define MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT21_STEP 22000000ULL
#define MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT18_STEP 20000000ULL

static uint32_t mal_hspec_stm32f7_clock_get_flash_latency(uint64_t system_frequency);
static uint32_t mal_hspec_stm32f7_clock_get_apb1_max_freq(uint64_t hclock);
static uint32_t mal_hspec_stm32f7_clock_get_apb2_max_freq(uint64_t hclock);
static uint32_t mal_hspec_stm32f7_clock_get_apb_divider(uint64_t hclock, uint64_t target, const uint8_t *dividers,
                                                        const uint32_t *divider_settings, uint8_t size);

static const uint8_t mal_hspec_stm32f7_clock_apb_dividers[] = {
    1, 2, 4, 8, 16
};

static const uint32_t mal_hspec_stm32f7_clock_apb_settings[] = {
    RCC_HCLK_DIV1,
    RCC_HCLK_DIV2,
    RCC_HCLK_DIV4,
    RCC_HCLK_DIV8,
    RCC_HCLK_DIV16
};

mal_error_e mal_clock_set_system_clock_unmanaged(const mal_system_clk_s *clk) {
    HAL_StatusTypeDef hal_result;
    RCC_OscInitTypeDef osc_config;
    RCC_ClkInitTypeDef clock_config;
    uint32_t flash_latency;
    mal_system_clk_src_e clk_src = clk->src;
    float src_clk_freq;
    uint64_t target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(clk->frequency) / 1000ULL;
    // Choose clk source
    switch (clk->src) {
    case MAL_SYSTEM_CLK_SRC_AUTO:
        // Since external clock is more precise than internal, MAL will favor
        // external in this case.
    case MAL_SYSTEM_CLK_SRC_EXTERNAL:
        // Check if there is an external source
        clk_src = MAL_SYSTEM_CLK_SRC_EXTERNAL;
        src_clk_freq = mal_clock_get_external_clock_frequency();
        if (0 == src_clk_freq) {
            // No external, switch to internal
            src_clk_freq = HSI_VALUE;
            clk_src = MAL_SYSTEM_CLK_SRC_INTERNAL;
        }
        break;
    case MAL_SYSTEM_CLK_SRC_INTERNAL:
    default:
        // Set HSI on
        src_clk_freq = HSI_VALUE;
        break;
    }
    // Start HSE clock if source
    if (MAL_SYSTEM_CLK_SRC_EXTERNAL == clk_src) {
        osc_config.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        osc_config.HSEState = RCC_HSE_ON;
        osc_config.PLL.PLLState = RCC_PLL_NONE;
        hal_result = HAL_RCC_OscConfig(&osc_config);
        if (HAL_OK != hal_result) {
            // HSE startup was unsuccessful, use HSI
            clk_src = MAL_SYSTEM_CLK_SRC_INTERNAL;
            src_clk_freq = HSI_VALUE;
        }
    }
    // Check if clock can be used without PLL
    if (clk->frequency == src_clk_freq) {
        clock_config.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        clock_config.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
        clock_config.AHBCLKDivider = RCC_SYSCLK_DIV1;
        clock_config.APB1CLKDivider = RCC_HCLK_DIV1;
        clock_config.APB2CLKDivider = RCC_HCLK_DIV1;
        flash_latency = mal_hspec_stm32f7_clock_get_flash_latency(target_frequency);
        hal_result = HAL_RCC_ClockConfig(&clock_config, flash_latency);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_CLOCK_ERROR;
        }
    } else {
        // Cap target frequency
        if (target_frequency > MAL_HSPEC_STM32F7_CLOCK_MAX_FREQUENCY) {
            target_frequency = MAL_HSPEC_STM32F7_CLOCK_MAX_FREQUENCY;
        }
        // Find PLL configuration
        bool pll_valid = false;
        uint64_t m_divider = 0;
        uint64_t n_multiplier = 0;
        uint64_t p_divider = 0;
        uint64_t q_divider = 0;
        uint64_t r_divider = 0;
        for (m_divider = 2; m_divider <= 63; m_divider++) {
            for (n_multiplier = 50; n_multiplier <= 432; n_multiplier++) {
                uint64_t vco = ((uint64_t)src_clk_freq / m_divider) * n_multiplier;
                // VCO frequency cannot be lower than 100 MHz and higher than 432 MHz
                if (vco < 100000000ULL || vco > 432000000ULL) {
                    continue;
                }
                for (p_divider = 2; p_divider <= 8; p_divider += 2) {
                    uint64_t pll_clk = vco / p_divider;
                    // Make sure frequency is not bellow desired frequency
                    if (pll_clk < target_frequency) {
                        break;
                    }
                    // Check if frequency is valid
                    if (pll_clk != target_frequency) {
                        continue;
                    }
                    // This setup is valid, find valid values for Q and R
                    q_divider = p_divider;
                    if (p_divider <= MAL_HSPEC_STM32F7_CLOCK_MAX_R_DIVIDER) {
                        r_divider = p_divider;
                        pll_valid = true;
                        break;
                    } else {
                        for (r_divider = 7; r_divider >= 2; r_divider--) {
                            uint64_t pllr = vco / r_divider;
                            if (pllr <= MAL_HSPEC_STM32F7_CLOCK_MAX_FREQUENCY) {
                                pll_valid = true;
                                break;
                            }
                        }
                    }
                    // Check if search is done
                    if (pll_valid) {
                        break;
                    }
                }
                // Check if search is done
                if (pll_valid) {
                    break;
                }
            }
            // Check if search is done
            if (pll_valid) {
                break;
            }
        }
        // Configure PLL
        osc_config.OscillatorType = RCC_OSCILLATORTYPE_NONE;
        osc_config.PLL.PLLState = RCC_PLL_ON;
        if (MAL_SYSTEM_CLK_SRC_EXTERNAL == clk_src) {
            osc_config.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        } else {
            osc_config.PLL.PLLSource = RCC_PLLSOURCE_HSI;
        }
        osc_config.PLL.PLLM = (uint32_t)m_divider;
        osc_config.PLL.PLLN = (uint32_t)n_multiplier;
        osc_config.PLL.PLLP = (uint32_t)p_divider;
        osc_config.PLL.PLLQ = (uint32_t)q_divider;
        osc_config.PLL.PLLR = (uint32_t)r_divider;
        hal_result = HAL_RCC_OscConfig(&osc_config);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_CLOCK_ERROR;
        }
        // Configure clock
        clock_config.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        clock_config.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        clock_config.AHBCLKDivider = RCC_SYSCLK_DIV1;
        // Select proper divider for APB1
        bool auto_select_apb1 = clk->clock_config == NULL;
        if (NULL != clk->clock_config) {
            auto_select_apb1 = !clk->clock_config->set_apb1;
        }
        if (auto_select_apb1) {
            clock_config.APB1CLKDivider = mal_hspec_stm32f7_clock_get_apb1_max_freq(target_frequency);
        } else {
            // Find the highest divider that does give a lower frequency
            uint64_t apb1_target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(clk->clock_config->apb1_frequency);
            apb1_target_frequency /= 1000ULL;
            clock_config.APB1CLKDivider = mal_hspec_stm32f7_clock_get_apb_divider(
                    target_frequency,
                    apb1_target_frequency,
                    mal_hspec_stm32f7_clock_apb_dividers,
                    mal_hspec_stm32f7_clock_apb_settings,
                    sizeof(mal_hspec_stm32f7_clock_apb_dividers));
        }
        // Select proper divider for APB2
        bool auto_select_apb2 = clk->clock_config == NULL;
        if (NULL != clk->clock_config) {
            auto_select_apb2 = !clk->clock_config->set_apb2;
        }
        if (auto_select_apb2) {
            clock_config.APB2CLKDivider = mal_hspec_stm32f7_clock_get_apb2_max_freq(target_frequency);
        } else {
            // Find the highest divider that does give a lower frequency
            uint64_t apb2_target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(clk->clock_config->apb2_frequency);
            apb2_target_frequency /= 1000ULL;
            clock_config.APB2CLKDivider = mal_hspec_stm32f7_clock_get_apb_divider(
                    target_frequency,
                    apb2_target_frequency,
                    mal_hspec_stm32f7_clock_apb_dividers,
                    mal_hspec_stm32f7_clock_apb_settings,
                    sizeof(mal_hspec_stm32f7_clock_apb_dividers));
        }
        flash_latency = mal_hspec_stm32f7_clock_get_flash_latency(target_frequency);
        hal_result = HAL_RCC_ClockConfig(&clock_config, flash_latency);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_CLOCK_ERROR;
        }
    }
    // Return proper result
    if (clk->src != MAL_SYSTEM_CLK_SRC_AUTO && clk->src != clk_src) {
        return MAL_ERROR_CLOCK_ERROR;
    } else if (SystemCoreClock != (uint64_t)clk->frequency) {
        return MAL_ERROR_CLOCK_ERROR;
    }
    return MAL_ERROR_OK;
}

static uint32_t mal_hspec_stm32f7_clock_get_apb1_max_freq(uint64_t hclock) {
    if (hclock <= 54000000ULL) {
        return RCC_HCLK_DIV1;
    } else if (hclock <= 108000000ULL) {
        return RCC_HCLK_DIV2;
    } else {
        return RCC_HCLK_DIV4;
    }
}

static uint32_t mal_hspec_stm32f7_clock_get_apb2_max_freq(uint64_t hclock) {
    if (hclock <= 108000000ULL) {
        return RCC_HCLK_DIV1;
    } else {
        return RCC_HCLK_DIV2;
    }
}

static uint32_t mal_hspec_stm32f7_clock_get_apb_divider(uint64_t hclock, uint64_t target, const uint8_t *dividers,
                                                        const uint32_t *divider_settings, uint8_t size) {
    uint8_t i;
    for (i = 0; i < size; i++) {
        uint64_t apb_frequency = hclock / dividers[i];
        if (apb_frequency == target) {
            return divider_settings[i];
        } else if (apb_frequency < target) {
            if (0 == i) {
                return divider_settings[i];
            }
            return divider_settings[i - 1];
        }
    }
    return divider_settings[size - 1];
}

static uint32_t mal_hspec_stm32f7_clock_get_flash_latency(uint64_t system_frequency) {
    // Fetch power
    mal_volts_t vdd_value = 3.3f;
    mal_power_get_rail_voltage(MAL_POWER_RAIL_VDD, &vdd_value);
    float vdd_volts = MAL_TYPES_MAL_VOLTS_TO_VOLTS(vdd_value);
    // Select frequency steps
    uint64_t freq_step;
    if (vdd_volts > 2.7f) {
        freq_step = MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT27_STEP;
    } else if (vdd_volts > 2.4f) {
        freq_step = MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT24_STEP;
    } else if (vdd_volts > 2.1f) {
        freq_step = MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT21_STEP;
    } else {
        freq_step = MAL_HSPEC_STM32F7_CLOCK_FLASH_LATENCY_GT18_STEP;
    }
    // Select latency
    uint64_t frequency_limit = freq_step;
    uint32_t latency;
    for (latency = 0; latency <= FLASH_ACR_LATENCY_9WS; latency++) {
        // Check if latency is valid
        if (system_frequency <= frequency_limit) {
            return latency;
        }
        // Move to next step
        frequency_limit += freq_step;
    }
    // Proper value not found, set maximum latency
    return FLASH_ACR_LATENCY_9WS;
}
