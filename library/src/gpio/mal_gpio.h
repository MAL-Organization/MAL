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

#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "std/mal_bool.h"
#include "std/mal_defs.h"

/**
 * @defgroup GPIO
 * @brief @copybrief mal_gpio.h
 * @{
 */

/**
 * Possible GPIO ports.
 */
typedef enum {
	MAL_GPIO_PORT_A = 0,  //!< MAL_PORT_A
	MAL_GPIO_PORT_B = 1,  //!< MAL_PORT_B
	MAL_GPIO_PORT_C = 2,  //!< MAL_PORT_C
	MAL_GPIO_PORT_D = 3,  //!< MAL_PORT_D
	MAL_GPIO_PORT_E = 4,  //!< MAL_PORT_E
	MAL_GPIO_PORT_F = 5,  //!< MAL_PORT_F
	MAL_GPIO_PORT_SIZE = 6//!< MAL_PORT_SIZE
} mal_gpio_port_e;

/**
 * Defines a GPIO.
 */
typedef struct {
	mal_gpio_port_e port; /**< The port of the GPIO.*/
	uint8_t pin; /**< The pin of the GPIO.*/
} mal_gpio_s;

/**
 * Possible GPIO directions.
 */
typedef enum {
	MAL_GPIO_DIR_IN,//!< MAL_GPIO_DIR_IN
	MAL_GPIO_DIR_OUT//!< MAL_GPIO_DIR_OUT
} mal_gpio_dir_e;

/**
 * Possible GPIO types.
 */
typedef enum {
	MAL_GPIO_OUT_PP,//!< Push-Pull
	MAL_GPIO_OUT_OD //!< Open Drain
} mal_gpio_out_e;

/**
 * Possible pull-up and pull-down values.
 */
typedef enum {
	MAL_GPIO_PUPD_NONE,//!< No pull-up or pull-down.
	MAL_GPIO_PUPD_PU,  //!< Pull-up.
	MAL_GPIO_PUPD_PD   //!< Pull-down.
} mal_gpio_pupd_e;

/**
 * Parameters to initialise a GPIO.
 */
typedef struct {
	mal_gpio_s gpio; /**< The gpio to initialize.*/
	mal_gpio_dir_e direction; /**< The direction of the GPIO.*/
	mal_gpio_out_e output_config; /**< The output configuration (type) of the GPIO.*/
	mal_gpio_pupd_e pupd; /**< The pull-up and pull-down configuration.*/
	uint64_t speed; /**< The refresh speed of the GPIO in hertz.*/
} mal_gpio_init_s;

/**
 * Possible GPIO event triggers.
 */
typedef enum {
	MAL_GPIO_EVENT_RISING, //!< MAL_GPIO_EVENT_RISING
	MAL_GPIO_EVENT_FALLING,//!< MAL_GPIO_EVENT_FALLING
	MAL_GPIO_EVENT_BOTH    //!< MAL_GPIO_EVENT_BOTH
} mal_gpio_event_e;

/**
 * @brief GPIO event callback.
 * @return Return a status of the callback.
 */
typedef mal_error_e (*mal_gpio_event_callback_t)(mal_gpio_s *gpio);

/**
 * Parameters to initialize an event.
 */
typedef struct {
	const mal_gpio_s *gpio; /**< The gpio for the event.*/
	mal_gpio_event_e event; /**< The event that will trigger.*/
	mal_gpio_event_callback_t callback; /**< The callback to execute upon event.*/
} mal_gpio_event_init_s;

/**
 * @brief Will set a gpio to the given value.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @param value Boolean value. Set true to set the IO high, false for low.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_set(const mal_gpio_s *gpio, bool value);

/**
 * @brief Returns the value of a GPIO.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return Boolean value. Returns true if IO is high, false if low.
 */
bool mal_gpio_get(const mal_gpio_s *gpio);

/**
 * @brief Toggles and IO. If the IO is high, it will go low and vice-versa.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_toggle(const mal_gpio_s *gpio);

/**
 * @brief Removes a GPIO event.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_event_remove(const mal_gpio_s *gpio);

/**
 * @brief Disable interrupt for a GPIO event.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_gpio_event_disable_interrupt(const mal_gpio_s *gpio);

/**
 * @brief Enable interrupt for a GPIO event.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
MAL_DEFS_INLINE void mal_gpio_event_enable_interrupt(const mal_gpio_s *gpio, bool active);

/**
 * @brief Initialize a GPIO.
 * @param gpio_init Initialize parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_init(mal_gpio_init_s *gpio_init);

/**
 * @brief Initialize a GPIO event. An event will allow you to execute code when
 * a GPIO changes state.
 * @param gpio_init GPIO initialize parameters.
 * @param event_init Event initialize parameters.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_event_init(mal_gpio_init_s *gpio_init, mal_gpio_event_init_s *event_init);

/**
 * @brief Return the GPIO to a safe state.
 * @param gpio The GPIO to reset.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_deinit(const mal_gpio_s *gpio);

/**
 * @}
 */

#endif /* MAL_GPIO_H_ */
