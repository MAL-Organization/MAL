/**
 * @file mal_gpio.h
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
 * @brief Functions to access and configure the GPIOs of the MCU.
 */

#ifndef MAL_GPIO_H_
#define MAL_GPIO_H_

#include "hspec/mal_hspec.h"

/**
 * @defgroup GPIO
 * @brief @copybrief mal_gpio.h
 * @{
 */

/**
 * @brief Will set a gpio to the given value.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @param value Boolean value. Set true to set the IO high, false for low.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_gpio_set(gpio, value) mal_hspec_set_gpio(gpio, value)

/**
 * @brief Returns the value of a GPIO.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return Boolean value. Returns true if IO is high, false if low.
 */
#define mal_gpio_get(gpio) mal_hspec_get_gpio(gpio)

/**
 * @brief Toggles and IO. If the IO is high, it will go low and vice-versa.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_gpio_toggle(gpio) mal_hspec_toggle_gpio(gpio)

/**
 * @brief Removes a GPIO event.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_gpio_event_remove(gpio) mal_hspec_gpio_event_remove(gpio)

/**
 * @brief Disable interrupt for a GPIO event.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return Returns true if interrupt was active before disabling it.
 */
#define mal_gpio_event_disable_interrupt(gpio) mal_hspec_gpio_event_disable_interrupt(gpio)

/**
 * @brief Enable interrupt for a GPIO event.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
#define mal_gpio_event_enable_interrupt(gpio, active) mal_hspec_gpio_event_enable_interrupt(gpio, active)

/**
 * @brief Initialize a GPIO.
 * @param gpio_init Initialize parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_init(mal_hspec_gpio_init_s *gpio_init);

/**
 * @brief Initialize a GPIO event. An event will allow you to execute code when
 * a GPIO changes state.
 * @param gpio_init GPIO initialize parameters.
 * @param event_init Event initialize parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_event_init(mal_hspec_gpio_init_s *gpio_init, mal_hspec_gpio_event_init_s *event_init);

/**
 * @}
 */

#endif /* MAL_GPIO_H_ */
