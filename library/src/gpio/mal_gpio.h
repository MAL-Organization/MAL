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
#include "gpio/mal_gpio_definitions.h"

/**
 * @defgroup GPIO
 * @brief @copybrief mal_gpio.h
 * @{
 */

/**
 * GPIO handle that must be defined by hardware specific implementation. Used
 * to access the gpio functions.
 */
typedef struct MAL_GPIO mal_gpio_s;

/**
 * GPIO event handle that must be defined by hardware specific implementation.
 * Used to access the gpio event functions.
 */
typedef struct MAL_GPIO_EVENT mal_gpio_event_s;

/**
 * Possible GPIO directions.
 */
typedef enum {
	MAL_GPIO_DIRECTION_IN,//!< Set a gpio as an input
	MAL_GPIO_DIRECTION_OUT//!< Set a gpio as an output
} mal_gpio_direction_e;

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
 * Parameters to initialize a GPIO.
 */
typedef struct {
	mal_gpio_port_e port; /**< The port of the GPIO to initialize.*/
	uint8_t pin; /**< The pin of the port of the GPIO to initialize.*/
	mal_gpio_direction_e direction; /**< The direction of the GPIO.*/
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
 * @param handle The handle given during initialization.
 */
typedef void (*mal_gpio_event_callback_t)(void *handle);

/**
 * Parameters to initialize an event.
 */
typedef struct {
    mal_gpio_port_e port; /**< The port of the GPIO for the event.*/
    uint8_t pin; /**< The pin of the port of the GPIO for the event. */
    mal_gpio_pupd_e pupd; /**< The pull-up and pull-down configuration.*/
    uint64_t speed; /**< The refresh speed of the GPIO in hertz.*/
	mal_gpio_event_e event; /**< The event that will trigger.*/
	mal_gpio_event_callback_t callback; /**< The callback to execute upon event.*/
	void *handle; /**< This will be passed to the callback.*/
} mal_gpio_event_init_s;

/**
 * @brief Will set a gpio to the given value.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @param value Boolean value. Set true to set the IO high, false for low.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_set(mal_gpio_s *gpio, bool value);

/**
 * @brief Fetches the value of a GPIO.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @param value Will contain the value of the GPIO.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_get(mal_gpio_s *gpio, bool *value);

/**
 * @brief Toggles and IO. If the IO is high, it will go low and vice-versa.
 * @param gpio A pointer of type ::mal_hspec_gpio_s.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_toggle(mal_gpio_s *gpio);

/**
 * @brief Removes a GPIO event.
 * @param handle The handle of the event to remove.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_event_remove(mal_gpio_event_s *handle);

/**
 * @brief Disable interrupt for a GPIO event.
 * @param handle The event to disable the interrupt from.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_gpio_event_disable_interrupt(mal_gpio_event_s *handle);

/**
 * @brief Enable interrupt for a GPIO event.
 * @param handle The event to disable the interrupt from.
 * @param active A boolean that indicates if the interrupt should be activated.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
MAL_DEFS_INLINE void mal_gpio_event_set_interrupt(mal_gpio_event_s *handle, bool active);

/**
 * @brief Initialize a GPIO.
 * @param init Initialize parameters.
 * @param handle The handle to initialize. Use this handle to access GPIO
 * functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_init(mal_gpio_init_s *init, mal_gpio_s *handle);

/**
 * @brief Initialize a GPIO event. An event will allow you to execute code when
 * a GPIO changes state.
 * @param init Initialize parameters.
 * @param gpio_handle The GPIO handle to initialize. Use this handle to access
 * GPIO functions.
 * @param event_handle The event handle to initialize. Use this handle to
 * access GPIO functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_event_init(mal_gpio_event_init_s *init, mal_gpio_s *gpio_handle, mal_gpio_event_s *event_handle);

/**
 * @brief Return the GPIO to a safe state.
 * @param gpio The GPIO handle to reset.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_gpio_deinit(mal_gpio_s *gpio);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* MAL_GPIO_H_ */
