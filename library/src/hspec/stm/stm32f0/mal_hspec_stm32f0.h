/**
 * @file mal_hspec_stm32
 * @author Olivier Allaire
 * @date May 2 2015
 * @copyright Copyright (c) 2015 Olivier Allaire
 * @par This file is part of MAL.
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
 * @brief Definitions for STM32F0.
 * @note MAL_FLOAT is not defined for all STM32F0 platforms.
 */

#ifndef HSPEC_STM_MAL_HSPEC_STM32F0_H_
#define HSPEC_STM_MAL_HSPEC_STM32F0_H_

#include "mal_hspec_stm32f0_cmn.h"
#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "gpio/mal_gpio.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx.h"
#include "cmsis/core_cmInstr.h"
#include "mal_hspec_stm32f0_gpio.h"
#include "mal_hspec_stm32f0_timer.h"
#include "mal_hspec_stm32f0_i2c.h"
#include "mal_hspec_stm32f0_can.h"
#include "mal_hspec_stm32f0_uuid.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_adc.h"
#include "mal_hspec_stm32f0_spi.h"
#include "mal_hspec_stm32f0_reset.h"
#include "mal_hspec_stm32f0_flash.h"
#include "mal_hspec_stm32f0_watchdog.h"
#include "mal_hspec_stm32f0_dac.h"

// Device specific function mapping section.
#if defined(MAL_STM32F072RB) || defined(MAL_STM32F072CB)

#include "hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072.h"

#define MAL_HSPEC_STM32F0_MAX_FREQUENCY	48000000

#define MAL_HSPEC_STM32F0_HSI_PLL_DIV	1

#define MAL_HSPEC_STM32F0_HSI_PLL_SRC	RCC_PLLSource_HSI

#define mal_hspec_stm32f0_is_pll_div_available(source) mal_hspec_stm32f072_is_pll_div_available(source)

#define mal_hspec_stm32f0_get_valid_ports(ports, size) mal_hspec_stm32f072_get_valid_ports(ports, size)

#define mal_hspec_stm32f0_get_valid_pins(pins) mal_hspec_stm32f072_get_valid_pins(pins)

#define mal_hspec_stm32f0_get_valid_timers(timers, size) mal_hspec_stm32f072_get_valid_timers(timers, size)

#define mal_hspec_stm32f0_get_timer_update_irq(timer) mal_hspec_stm32f072_get_timer_update_irq(timer)

#define mal_hspec_stm32f0_get_valid_i2c_interfaces(interfaces, size) mal_hspec_stm32f072_get_valid_i2c_interfaces(interfaces, size)

#define mal_hspec_stm32f0_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size) mal_hspec_stm32f072_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size)

#define mal_hspec_stm32f0_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size) mal_hspec_stm32f072_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size)

#define mal_hspec_stm32f0_get_valid_adc_ios(adc, ios, size) mal_hspec_stm32f072_get_valid_adc_ios(adc, ios, size)

#define mal_hspec_stm32f0_get_valid_spi_ios(interface, mosis, mosis_size, misos, misos_size, clks, clks_size, selects, selects_size) mal_hspec_stm32f072_get_valid_spi_ios(interface, mosis, mosis_size, misos, misos_size, clks, clks_size, selects, selects_size)

#define mal_hspec_stm32f0_get_valid_channel_ios(timer, ios, size) mal_hspec_stm32f072_get_valid_channel_ios(timer, ios, size)

#define mal_hspec_stm32f0_get_timer_compare_irq(timer) mal_hspec_stm32f072_get_timer_compare_irq(timer)

#define mal_hspec_stm32f0_get_valid_dac_ios(dac, ios, size) mal_hspec_stm32f072_get_valid_dac_ios(dac, ios, size)

#else
#error No valid hardware specfic device symbol specified...
#endif

void mal_hspec_stm32f0_startup();

mal_error_e mal_hspec_stm32f0_set_system_clk(const mal_hspec_system_clk_s *clk);

mal_error_e mal_hspec_stm32f0_serial_init(mal_hspec_serial_init_s *init);

mal_error_e mal_hspec_stm32f0_serial_transmit(mal_hspec_serial_port_e port, uint16_t data);

#endif /* HSPEC_STM_MAL_HSPEC_STM32F0_H_ */
