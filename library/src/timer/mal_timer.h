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

#include "hspec/mal_hspec_cmn.h"
#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"

/**
 * @defgroup Timer
 * @brief @copybrief mal_timer.h
 * @{
 */

/**
 * Structure that contains basic info about a timer.
 */
typedef struct {
	mal_hspec_timer_mode_e mode; //!< The current mode of the timer.
	bool is_available; //!< true if the timer is available, false otherwise.
	float frequency; //!< Frequency of the timer.
	float delta; //!< Frequency error tolerance.
	// Tick timer variables
	volatile uint64_t tick_counter; //!< Contains the count of the timer in tick mode.
} mal_timer_state_s;

/**
 * @brief Disable interrupts for a timer.
 * @param timer The timer to disable the interrupt. Should be of type
 * ::mal_hspec_timer_e.
 * @return Returns true if interrupt was active before disabling it.
 */
#define mal_timer_disable_interrupt(timer) mal_hspec_disable_timer_interrupt(timer)

/**
 * @brief Enable interrupts for a timer.
 * @param timer The timer to enable the interrupt. Should be of type
 * ::mal_hspec_timer_e.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
#define mal_timer_enable_interrupt(timer, active) mal_hspec_enable_timer_interrupt(timer, active)

/**
 * @brief Set the duty cycle of an initialized PWM IO.
 * @param timer The timer of the PWM IO.
 * @param gpio The GPIO the PWM is on.
 * @param duty_cyle A float that should go from 0 to 1. 0 is 0% duty cyle and 1
 * is 100% duty cycle.
 * @return #MAL_ERROR_OK on success.
 */
#define mal_timer_set_pwm_duty_cycle(timer, gpio, duty_cycle) mal_hspec_timer_set_pwm_duty_cycle(timer, gpio, duty_cycle)

/**
 * @brief Get the resolution of timer. The resolution is number of bits for the
 * count register.
 * @param timer The timer to get the resolution from.
 * @param resolution A pointer to a uint8_t. It will contain the number of bits
 * the count register uses. Usually 8, 16, 32 or 64 bits.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_timer_get_resolution(timer, resolution) mal_hspec_timer_get_resolution(timer, resolution)

/**
 * @brief Get the actual counting frequency of a timer.
 * @param timer The timer to get the frequency from.
 * @param frequency A pointer to a float that will contain the counting
 * frequency.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_timer_get_count_frequency(timer, frequency) mal_hspec_timer_get_count_frequency(timer, frequency)

/**
 * @brief Initialize a timer as a simple tick counter. Use ::mal_timer_get_tick
 * to read ticks.
 * @param timer The desired timer to initialize.
 * @param frequency The frequency to count at in hertz.
 * @param delta The allowed delta of frequency in hertz. This means the actual
 * frequency of the timer can be frequency +/- delta.
 * @param handle This handle will return the used timer. Useful when using
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_tick(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_e *handle);

/**
 * This is a simple timer initialization. The main difference between this and
 * the tick timer is you have to handle yourself the overflow of the timer
 * based on timer resolution. This method is much faster however because it
 * doesn't rely on interrupts to work. Use this to time precise events.
 * @param timer The desired timer to initialize.
 * @param frequency The frequency to count at in hertz.
 * @param handle This handle will return the used timer. Useful when using
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_count(mal_hspec_timer_e timer, float frequency, mal_hspec_timer_e *handle);

/**
 * @brief Initialize a timer that periodically calls a function (task).
 * @param timer The desired timer to initialize.
 * @param frequency The frequency to count at in hertz.
 * @param delta delta The allowed delta of frequency in hertz. This means the actual
 * frequency of the timer can be frequency +/- delta.
 * @param callback The callback to execute at every count.
 * @param handle This handle will return the used timer. Useful when using
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_task(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback, mal_hspec_timer_e *handle);

/**
 * @brief Return the tick of a timer when initialized in tick mode.
 * @param handle The timer to check for tick.
 * @return The tick count.
 */
uint64_t mal_timer_get_tick(mal_hspec_timer_e handle);

/**
 * @brief Frees a used timer.
 * @param timer The timer to free.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_free(mal_hspec_timer_e timer);

/**
 * @brief Initializes a timer and the IO as a PWM generator.
 * @param init The initialize structure of the pwm.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_pwm(mal_hspec_timer_pwm_init_s *init);

/**
 * @brief Returns the state of the timer.
 * @param timer The desired timer.
 * @param state A pointer to a mal_timer_state_s structure. The timer state
 * will be copied there.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_state(mal_hspec_timer_e timer, mal_timer_state_s *state);

/**
 * @}
 */

#endif /* TIMER_MAL_TIMER_H_ */
