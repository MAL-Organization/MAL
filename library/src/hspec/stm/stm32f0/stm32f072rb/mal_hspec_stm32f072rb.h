/*
 * mal_hspec_stm32f072rb.h
 *
 *  Created on: May 3, 2015
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

#ifndef HSPEC_STM_MAL_HSPEC_STM32F072RB_H_
#define HSPEC_STM_MAL_HSPEC_STM32F072RB_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "stm32f0/stm32f0xx.h"
#include "i2c/mal_i2c.h"

mal_error_e mal_hspec_stm32f072rb_get_valid_ports(const mal_hspec_port_e **ports, uint8_t *size);
mal_error_e mal_hspec_stm32f072rb_get_valid_pins(const uint64_t **pins);
mal_error_e mal_hspec_stm32f072rb_get_valid_timers(const mal_hspec_timer_e **timers, uint8_t *size);
IRQn_Type mal_hspec_stm32f072rb_get_timer_update_irq(mal_hspec_timer_e timer);
mal_error_e mal_hspec_stm32f072rb_get_valid_i2c_ios(mal_hspec_i2c_e interface, const mal_hspec_gpio_s **scls, uint8_t *scls_size, const mal_hspec_gpio_s **sdas, uint8_t *sdas_size);
mal_error_e mal_hspec_stm32f072rb_get_port_afs(mal_hspec_port_e port, const mal_hspec_stm32f0_af_e (**afs)[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE]);
mal_error_e mal_hspec_stm32f072rb_get_valid_can_ios(mal_hspec_can_e interface, const mal_hspec_gpio_s **txs, uint8_t *txs_size, const mal_hspec_gpio_s **rxs, uint8_t *rxs_size);

#endif /* HSPEC_STM_MAL_HSPEC_STM32F072RB_H_ */
