/**
 * @file mal_timer.h
 * @author Olivier Allaire
 * @date May 25 2015
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
 * @brief Functions to access and configure the timers of the MCU.
 */

#ifndef TIMER_MAL_TIMER_H_
#define TIMER_MAL_TIMER_H_

/**
 * Possible timer input triggers.
 */
typedef enum {
	MAL_TIMER_INPUT_RISING, //!< Rising event.
	MAL_TIMER_INPUT_FALLING,//!< Falling event.
	MAL_TIMER_INPUT_BOTH    //!< Rising and falling event.
} mal_timer_input_e;

/**
 * @brief Function pointer typdef for timer in input capture mode.
 * @param timer Will provide the timer executing the callback.
 * @param count The captured count.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_timer_input_capture_callback_t)(mal_timer_e timer, uint64_t count);

/**
 * Initialization parameters of a capture input.
 */
typedef struct {
	mal_timer_e timer; /**< The timer to use for the input capture.*/
	mal_hertz_t frequency; /**< The frequency to count to.*/
	const mal_gpio_s *input_io; /**< The gpio of the input capture.*/
	mal_timer_input_e input_event; /**< The input event to capture.*/
	uint8_t input_divider; /**< Specifies after how many events the capture happens.*/
	mal_timer_input_capture_callback_t callback; /**< The callback to be executed when capture occurs.*/
} mal_timer_intput_capture_init_s;

/**
 * @brief Get the actual counting frequency of a timer.
 * @param timer The timer to get the frequency from.
 * @param frequency A pointer to a mal_hertz_t that will contain the counting
 * frequency.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_count_frequency(mal_timer_e timer, mal_hertz_t *frequency);

/**
 * @brief Get the count register of a timer.
 * @param timer The timer to get the count from.
 * @param count A pointer to a uint64_t. It will contain the count.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_count(mal_timer_e timer, uint64_t *count);

/**
 * This is a simple timer initialization. The main difference between this and
 * the tick timer is you have to handle yourself the overflow of the timer
 * based on timer resolution. This method is much faster however because it
 * doesn't rely on interrupts to work. Use this to time precise events.
 * @param timer The desired timer to initialize.
 * @param frequency The frequency to count at.
 * @param handle This handle will return the used timer. Useful when using
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_count(mal_timer_e timer, mal_hertz_t frequency, mal_timer_e *handle);

/**
 * @brief Similar to mal_timer_free, but will be unmanaged. This means this
 * timer will not be flagged as busy and the use of timer ANY will not work
 * properly.
 * @param timer The desired timer to initialize.
 * @param frequency The frequency to count at.
 * @param handle This handle will return the used timer. Useful when using
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_count_unmanaged(mal_timer_e timer, mal_hertz_t frequency);

/**
 * @brief Return the tick of a timer when initialized in tick mode.
 * @param handle The timer to check for tick.
 * @return The tick count.
 */
uint64_t mal_timer_get_tick(mal_timer_e handle);

/**
 * @brief Frees a used timer.
 * @param timer The timer to free.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_free(mal_timer_e timer);

/**
 * @brief Similar to mal_timer_free, but will be unmanaged. This means this
 * timer will not be flagged as busy and the use of timer ANY will not work
 * properly.
 * @param timer
 * @return
 */
mal_error_e mal_timer_free_unmanaged(mal_timer_e timer);

/**
 * @brief Returns the state of the timer.
 * @param timer The desired timer.
 * @param state A pointer to a mal_timer_state_s structure. The timer state
 * will be copied there.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_state(mal_timer_e timer, mal_timer_state_s *state);

/**
 * @brief Initialize a timer as input capture timer.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_input_capture(mal_timer_intput_capture_init_s *init);

/**
 * @brief Similar to mal_timer_init_input_capture, but will be unmanaged. This
 * means this timer will not be flagged as busy and the use of timer ANY will
 * not work properly.
 * @param The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_input_capture_unmanaged(mal_timer_intput_capture_init_s *init);

/**
 * @}
 */

#endif /* TIMER_MAL_TIMER_H_ */
