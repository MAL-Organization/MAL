/*
 * mal_hspec_mingw_gpio.h
 *
 *  Created on: Mar 24, 2016
 *      Author: Kevin
 */

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_

#include "std/mal_error.h"
#include "std/mal_bool.h"
#include "hspec/mal_hspec_cmn.h"

mal_error_e mal_hspec_mingw_gpio_init(mal_hspec_gpio_init_s *gpio_init);

mal_error_e mal_hspec_mingw_get_valid_ports(const mal_hspec_port_e **ports, uint8_t *size);

mal_error_e mal_hspec_mingw_get_valid_pins(const uint64_t **pins);

mal_error_e mal_hspec_mingw_set_gpio(const mal_hspec_gpio_s *gpio, bool value);

mal_error_e mal_hspec_mingw_toggle_gpio(const mal_hspec_gpio_s *gpio);

bool mal_hspec_mingw_get_gpio(const mal_hspec_gpio_s *gpio);

mal_error_e mal_hspec_mingw_gpio_event_init(mal_hspec_gpio_event_init_s *init);

mal_error_e mal_hspec_mingw_gpio_async_event(mal_hspec_gpio_s *gpio, mal_hspec_gpio_event_callback_t callback);

void mal_hspec_mingw_gpio_do_async(mal_hspec_gpio_s gpio);

mal_error_e mal_hspec_mingw_gpio_event_remove(const mal_hspec_gpio_s *gpio);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_ */
