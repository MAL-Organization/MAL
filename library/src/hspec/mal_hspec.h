/*
 * mal_hspec.h
 *
 *  Created on: May 2, 2015
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

#ifndef HSPEC_MAL_HSPEC_H_
#define HSPEC_MAL_HSPEC_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"

// Family specific function mapping
#ifdef MAL_STM32F0
#include "stm/stm32f0/mal_hspec_stm32f0.h"

// Startup mappings
#define mal_hspec_startup() mal_hspec_stm32f0_startup()

// GPIO mappings
/**
 * \def mal_hspec_set_gpio_dir(port, pin)
 * \param[in] port The port defined by #mal_hspec_port_e.
 * \param[in] pin  The pin number.
 * \return Returns a #mal_error_e code.
 */
#define mal_hspec_gpio_init(init) mal_hspec_stm32f0_gpio_init(init)

#define mal_hspec_get_valid_ports(ports, size) mal_hspec_stm32f0_get_valid_ports(ports, size)

#define mal_hspec_get_valid_pins(pins) mal_hspec_stm32f0_get_valid_pins(pins)

#define mal_hspec_set_gpio(gpio, value) mal_hspec_stm32f0_set_gpio(gpio, value)

#define mal_hspec_get_gpio(gpio) mal_hspec_stm32f0_get_gpio(gpio)

#define mal_hspec_toggle_gpio(gpio) mal_hspec_stm32f0_toggle_gpio(gpio)

#define mal_hspec_gpio_event_init(init) mal_hspec_stm32f0_gpio_event_init(init)

#define mal_hspec_gpio_event_remove(gpio) mal_hspec_stm32f0_gpio_event_remove(gpio)

#define mal_hspec_gpio_event_disable_interrupt(gpio) mal_hspec_stm32f0_gpio_event_disable_interrupt(gpio)

#define mal_hspec_gpio_event_enable_interrupt(gpio, active) mal_hspec_stm32f0_gpio_event_enable_interrupt(gpio, active)

// Clock mappings
#define mal_hspec_set_system_clk(clk) mal_hspec_stm32f0_set_system_clk(clk)

#define mal_hspec_get_timer_input_clk(timer, clock) mal_hspec_stm32f0_timer_get_input_clk(timer, clock)

// Timer mappings
#define mal_hspec_get_valid_timers(timers, size) mal_hspec_stm32f0_get_valid_timers(timers, size)

#define mal_hspec_timer_init(timer, frequency, precision, callback) mal_hspec_stm32f0_timer_init(timer, frequency, precision, callback)

#define mal_hspec_disable_timer_interrupt(timer) mal_hspec_stm32f0_disable_timer_interrupt(timer)

#define mal_hspec_enable_timer_interrupt(timer, active) mal_hspec_stm32f0_enable_timer_interrupt(timer, active)

#define mal_hspec_timer_free(timer) mal_hspec_stm32f0_timer_free(timer)

// I2C mappings

#define mal_hspec_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size) mal_hspec_stm32f0_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size)

#define mal_hspec_i2c_master_init(init) mal_hspec_stm32f0_i2c_master_init(init)

#define mal_hspec_i2c_transfer(interface, msg) mal_hspec_stm32f0_i2c_transfer(interface, msg)

#define mal_hspec_i2c_disable_interrupt(interface) mal_hspec_stm32f0_i2c_disable_interrupt(interface)

#define mal_hspec_i2c_enable_interrupt(interface, active) mal_hspec_stm32f0_i2c_enable_interrupt(interface, active)

// CAN

#define mal_hspec_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size) mal_hspec_stm32f0_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size)

#define mal_hspec_can_init(init) mal_hspec_stm32f0_can_init(init)

#define mal_hspec_can_transmit(interface, msg) mal_hspec_stm32f0_can_transmit(interface, msg)

#define mal_hspec_can_add_filter(interface, filter) mal_hspec_stm32f0_can_add_filter(interface, filter)

#define mal_hspec_disable_can_interrupt(interface) mal_hspec_stm32f0_disable_can_interrupt(interface)

#define mal_hspec_enable_can_interrupt(interface, active) mal_hspec_stm32f0_enable_can_interrupt(interface, active)

#define mal_hspec_can_remove_filter(interface, filter) mal_hspec_stm32f0_can_remove_filter(interface, filter)

// SERIAL

#define mal_hspec_serial_init(init) mal_hspec_stm32f0_serial_init(init)

#define mal_hspec_serial_transmit(port, data) mal_hspec_stm32f0_serial_transmit(port, data)

// UUID

#define mal_hspec_uuid_read(uuid) mal_hspec_stm32f0_uuid_read(uuid)

// ADC

#define mal_hspec_get_valid_adc_ios(adc, ios, size) mal_hspec_stm32f0_get_valid_adc_ios(adc, ios, size)

#define mal_hspec_adc_init(init) mal_hspec_stm32f0_adc_init(init)

#define mal_hspec_adc_read(adc, value) mal_hspec_stm32f0_adc_read(adc, value)

#define mal_hspec_adc_resolution(adc, resolution) mal_hspec_stm32f0_adc_resolution(adc, resolution)

#define mal_hspec_adc_async_read(adc, callback) mal_hspec_stm32f0_adc_async_read(adc, callback)

#define mal_hspec_disable_adc_interrupt(adc) mal_hspec_stm32f0_disable_adc_interrupt(adc)

#define mal_hspec_enable_adc_interrupt(adc, active) mal_hspec_stm32f0_enable_adc_interrupt(adc, active)

#elif defined(MAL_MINGW)
#include "mingw/mal_hspec_mingw.h"

// Startup mappings
#define mal_hspec_startup()

// GPIO mappings

#define mal_hspec_gpio_init(init) mal_hspec_mingw_gpio_init(init)

#define mal_hspec_get_valid_ports(ports, size) mal_hspec_mingw_get_valid_ports(ports, size)

#define mal_hspec_get_valid_pins(pins) mal_hspec_mingw_get_valid_pins(pins)

#define mal_hspec_set_gpio(gpio, value) mal_hspec_mingw_set_gpio(gpio, value)

#define mal_hspec_get_gpio(gpio) mal_hspec_mingw_get_gpio(gpio)

#define mal_hspec_toggle_gpio(gpio) mal_hspec_mingw_toggle_gpio(gpio)

#define mal_hspec_gpio_event_init(init) mal_hspec_mingw_gpio_event_init(init)

#define mal_hspec_gpio_event_remove(gpio) mal_hspec_mingw_gpio_event_remove(gpio)

#define mal_hspec_gpio_event_disable_interrupt(gpio) 1

#define mal_hspec_gpio_event_enable_interrupt(gpio, active) (void)(gpio);(void)(active);

// Clock mappings
#define mal_hspec_set_system_clk(clk) mal_hspec_mingw_set_system_clk(clk)

#define mal_hspec_get_timer_input_clk(timer, clock)

// Timer mappings
#define mal_hspec_get_valid_timers(timers, size) mal_hspec_mingw_get_valid_timers(timers, size)

#define mal_hspec_timer_init(timer, frequency, precision, callback) mal_hspec_mingw_timer_init(timer, frequency, precision, callback)

#define mal_hspec_disable_timer_interrupt(timer) 1

#define mal_hspec_enable_timer_interrupt(timer, active) (void)(timer);(void)(active);

#define mal_hspec_timer_free(timer) mal_hspec_mingw_timer_free(timer)

// I2C mappings

#define mal_hspec_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size) mal_hspec_mingw_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size)

#define mal_hspec_i2c_master_init(init) mal_hspec_mingw_i2c_master_init(init)

#define mal_hspec_i2c_transfer(interface, msg) mal_hspec_mingw_i2c_transfer(interface, msg)

#define mal_hspec_i2c_disable_interrupt(interface) 1

#define mal_hspec_i2c_enable_interrupt(interface, active) (void)(interface);(void)(active);

// CAN

#define mal_hspec_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size) mal_hspec_mingw_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size)

#define mal_hspec_can_init(init) mal_hspec_mingw_can_init(init)

#define mal_hspec_can_transmit(interface, msg) mal_hspec_mingw_can_transmit(interface, msg)

#define mal_hspec_can_add_filter(interface, filter) mal_hspec_mingw_can_add_filter(interface, filter)

#define mal_hspec_disable_can_interrupt(interface) 1

#define mal_hspec_enable_can_interrupt(interface, active) (void)(interface);(void)(active);

#define mal_hspec_can_remove_filter(interface, filter) mal_hspec_mingw_can_remove_filter(interface, filter)

//// SERIAL
//
//#define mal_hspec_serial_init(init) mal_hspec_stm32f0_serial_init(init)
//
//#define mal_hspec_serial_transmit(port, data) mal_hspec_stm32f0_serial_transmit(port, data)

// UUID

#define mal_hspec_uuid_read(uuid) mal_hspec_mingw_uuid_read(uuid)

// ADC

#define mal_hspec_get_valid_adc_ios(adc, ios, size) mal_hspec_mingw_get_valid_adc_ios(adc, ios, size)

#define mal_hspec_adc_init(init) mal_hspec_mingw_adc_init(init)

#define mal_hspec_adc_read(adc, value) mal_hspec_mingw_adc_read(adc, value)

#define mal_hspec_adc_resolution(adc, resolution) mal_hspec_mingw_adc_resolution(adc, resolution)

#define mal_hspec_adc_async_read(adc, callback) mal_hspec_mingw_adc_async_read(adc, callback)

#define mal_hspec_disable_adc_interrupt(adc) 1

#define mal_hspec_enable_adc_interrupt(adc, active) (void)(adc);(void)(active);

#else
#error No hardware specfic family symbol specified...
#endif

mal_error_e mal_hspec_is_port_valid(mal_hspec_port_e port);

mal_error_e mal_hspec_is_gpio_valid(mal_hspec_gpio_s *gpio);

mal_error_e mal_hspec_initialise_system_clk(void);

uint64_t mal_hspec_get_external_clk_freq(void);

mal_error_e mal_hspec_is_timer_valid(mal_hspec_timer_e timer);

mal_error_e mal_hspec_is_i2c_interface_valid(mal_hspec_i2c_e interface, const mal_hspec_gpio_s *scl, const mal_hspec_gpio_s *sda);

mal_error_e mal_hspec_is_can_interface_valid(mal_hspec_can_e interface, mal_hspec_gpio_s *tx, mal_hspec_gpio_s *rx);

mal_error_e mal_hspec_is_adc_valid(mal_hspec_adc_e adc, const mal_hspec_gpio_s *gpio);

#endif /* HSPEC_MAL_HSPEC_H_ */
