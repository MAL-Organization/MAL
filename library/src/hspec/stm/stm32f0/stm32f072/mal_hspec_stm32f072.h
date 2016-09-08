/*
 * mal_hspec_stm32f072.h
 *
 *  Created on: Sep 7, 2016
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
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

#ifndef HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_H_
#define HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_H_

#include "hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_flash.h"
#include "hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_timer.h"

#include "hspec/mal_hspec_cmn.h"
#include "std/mal_bool.h"

// Device specific function mapping section.
#if defined(MAL_STM32F072RB)

#include "hspec/stm/stm32f0/stm32f072/stm32f072rb/mal_hspec_stm32f072rb.h"

#define mal_hspec_stm32f072_get_port_afs(port, afs) mal_hspec_stm32f072rb_get_port_afs(port, afs)

#define mal_hspec_stm32f072_get_valid_ports(ports, size) mal_hspec_stm32f072rb_get_valid_ports(ports, size)

#define mal_hspec_stm32f072_get_valid_pins(pins) mal_hspec_stm32f072rb_get_valid_pins(pins)

#define mal_hspec_stm32f072_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size) mal_hspec_stm32f072rb_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size)

#define mal_hspec_stm32f072_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size) mal_hspec_stm32f072rb_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size)

#define mal_hspec_stm32f072_get_valid_adc_ios(adc, ios, size) mal_hspec_stm32f072rb_get_valid_adc_ios(adc, ios, size)

#define mal_hspec_stm32f072_get_valid_spi_ios(interface, mosis, mosis_size, misos, misos_size, clks, clks_size, selects, selects_size) mal_hspec_stm32f072rb_get_valid_spi_ios(interface, mosis, mosis_size, misos, misos_size, clks, clks_size, selects, selects_size)

#define mal_hspec_stm32f072_get_valid_channel_ios(timer, ios, size) mal_hspec_stm32f072rb_get_valid_channel_ios(timer, ios, size)

#define mal_hspec_stm32f072_get_timer_afs(afs) mal_hspec_stm32f072rb_get_timer_afs(afs)

#define mal_hspec_stm32f072_flash_get_page_count() mal_hspec_stm32f072rb_flash_get_page_count()

#elif defined(MAL_STM32F072CB)

#include "hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb.h"

#define mal_hspec_stm32f072_get_port_afs(port, afs) mal_hspec_stm32f072cb_get_port_afs(port, afs)

#define mal_hspec_stm32f072_get_valid_ports(ports, size) mal_hspec_stm32f072cb_get_valid_ports(ports, size)

#define mal_hspec_stm32f072_get_valid_pins(pins) mal_hspec_stm32f072cb_get_valid_pins(pins)

#define mal_hspec_stm32f072_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size) mal_hspec_stm32f072cb_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size)

#define mal_hspec_stm32f072_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size) mal_hspec_stm32f072cb_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size)

#define mal_hspec_stm32f072_get_valid_adc_ios(adc, ios, size) mal_hspec_stm32f072cb_get_valid_adc_ios(adc, ios, size)

#define mal_hspec_stm32f072_get_valid_spi_ios(interface, mosis, mosis_size, misos, misos_size, clks, clks_size, selects, selects_size) mal_hspec_stm32f072cb_get_valid_spi_ios(interface, mosis, mosis_size, misos, misos_size, clks, clks_size, selects, selects_size)

#define mal_hspec_stm32f072_get_valid_channel_ios(timer, ios, size) mal_hspec_stm32f072cb_get_valid_channel_ios(timer, ios, size)

#define mal_hspec_stm32f072_get_timer_afs(afs) mal_hspec_stm32f072cb_get_timer_afs(afs)

#define mal_hspec_stm32f072_flash_get_page_count() mal_hspec_stm32f072cb_flash_get_page_count()

#else
#error No valid hardware specfic device symbol specified...
#endif

bool mal_hspec_stm32f072_is_pll_div_available(mal_hspec_system_clk_src_e source);

#endif /* HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_H_ */
