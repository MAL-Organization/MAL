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

#ifndef TIMER_MAL_TIMER2_H_
#define TIMER_MAL_TIMER2_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"
#include "std/mal_types.h"
#include "gpio/mal_gpio.h"
#include "std/mal_defs.h"

/**
 * @defgroup Timer
 * @brief @copybrief mal_timer.h
 * @{
 */

/**
 * Possible timers to use. Note that not all timers are supported on all
 * platforms.
 */
typedef enum {
    MAL_TIMER_1 = 0,    //!< Timer 1
    MAL_TIMER_2 = 1,    //!< Timer 2
    MAL_TIMER_3 = 2,    //!< Timer 3
    MAL_TIMER_4 = 3,    //!< Timer 4
    MAL_TIMER_5 = 4,    //!< Timer 5
    MAL_TIMER_6 = 5,    //!< Timer 6
    MAL_TIMER_7 = 6,    //!< Timer 7
    MAL_TIMER_8 = 7,    //!< Timer 8
    MAL_TIMER_9 = 8,    //!< Timer 9
    MAL_TIMER_10 = 9,   //!< Timer 10
    MAL_TIMER_11 = 10,  //!< Timer 11
    MAL_TIMER_12 = 11,  //!< Timer 12
    MAL_TIMER_13 = 12,  //!< Timer 13
    MAL_TIMER_14= 13,   //!< Timer 14
    MAL_TIMER_15 = 14,  //!< Timer 15
    MAL_TIMER_16 = 15,  //!< Timer 16
    MAL_TIMER_17 = 16,  //!< Timer 17
    MAL_TIMER_SIZE = 17,//!< Do not use.
    MAL_TIMER_ANY       //!< Use to pick first available timer for initialization.
} mal_timer_e;

/**
 * Timer handle that must be defined by hardware specific implementation. Used
 * to access the timer functions.
 */
typedef struct MAL_TIMER mal_timer_s;

/**
 * @brief Function pointer typdef for timer in task mode.
 * @param handle The given handle during initialization.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef void (*mal_timer_callback_t)(void *handle);

/**
 * Information for reserved timers.
 */
typedef struct {
    bool is_available; //!< true if the timer is available, false otherwise.
} mal_timer_state_s;

/**
 * Initialization parameters of a timer in task mode.
 */
typedef struct {
    mal_timer_e timer; //!< The timer to use for the input capture.
    mal_hertz_t frequency; //!< The frequency to count to.
    mal_hertz_t delta; //!< Frequency error tolerance.
    mal_timer_callback_t callback; //!< The callback to be executed on overflow.
    void *callback_handle;
} mal_timer_init_task_s;

/**
 * Initialization parameters of a PWM input.
 */
typedef struct {
    mal_timer_e timer; /**< The timer to use for the PWM output.*/
    mal_hertz_t frequency; /**< The frequency of the PWM.*/
    mal_hertz_t delta; /**< The acceptable frequency delta.*/
    const mal_gpio_s *pwm_io; /**< The gpio of the PWM output.*/
} mal_timer_pwm_init_s;

/**
 * @brief Macro to help subtract 2 timer values regardless of the resolution.
 * The result will be count2 - count1 respecting the two's complement wrap
 * around. This means that for an 8 bit resolution timer, 1 - 255 = 2.
 * @param count2 The count 2 value. Should be the most recent count.
 * @param count1 The count 1 value. Should be the oldest count.
 * @param mask The value obtained by #mal_timer_get_count_mask.
 * @return The result of the subtraction.
 */
#define MAL_TIMER_SUB_COUNTS(count2, count1, mask)	(((count2) + ((-(count1)) & (mask))) & (mask))

/**
 * Reserve a timer. This function is useful if you want to use
 * #MAL_HSPEC_TIMER_ANY to reserve a random timer. This allows the use of a
 * random timer when the specific timer is not important.
 * @param timer The timer to reserve. Note that if a random timer is fine,
 * #MAL_HSPEC_TIMER_ANY can be used.
 * @param reserved_timer The actual timer reserved. Useful when timer is
 * #MAL_HSPEC_TIMER_ANY.
 * @return #MAL_ERROR_OK if timer is reserved, #MAL_ERROR_HARDWARE_UNAVAILABLE
 * otherwise.
 */
mal_error_e mal_timer_reserve(mal_timer_e timer, mal_timer_e *reserved_timer);

/**
 * @brief Disable interrupts for a timer.
 * @param timer The timer to disable the interrupt. Should be of type
 * ::mal_hspec_timer_e.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_timer_disable_interrupt(mal_timer_s *handle);

/**
 * @brief Enable interrupts for a timer.
 * @param timer The timer to enable the interrupt. Should be of type
 * ::mal_hspec_timer_e.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
MAL_DEFS_INLINE void mal_timer_enable_interrupt(mal_timer_s *handle, bool active);

/**
 * @brief Initialize a timer that periodically calls a function (task).
 * @param init Task timer initialize parameters.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_task(mal_timer_init_task_s *init, mal_timer_s *handle);

/**
 * @brief Initialize directly a timer that periodically calls a function
 * (task).
 * @param init Timer initialization parameters.
 * @param direct_init A pointer to direct initialization parameters. See the
 * hardware specific implementation to know what type this should be.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_direct_init_task(mal_timer_init_task_s *init, const void *direct_init, mal_timer_s *handle);

/**
 * @brief Initializes a timer and the IO as a PWM generator.
 * @param init The initialize structure of the pwm.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_pwm(mal_timer_pwm_init_s *init, mal_timer_s *handle);

/**
 * @brief Get the resolution of timer. The resolution is number of bits for the
 * count register.
 * @param timer The timer to get the resolution from.
 * @param resolution A pointer to a uint8_t. It will contain the number of bits
 * the count register uses. Usually 8, 16, 32 or 64 bits.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_resolution(mal_timer_e timer, uint8_t *resolution);

/**
 * @brief Will return the mask to use to filter the count register values.
 * @param timer
 * @param mask A pointer to a uint64_t that will contain the mask;
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_count_mask(mal_timer_e timer, uint64_t *mask);

/**
 * @brief Check if a timer is available for this MCU.
 * @param timer The timer to check.
 * @return Returns #MAL_ERROR_OK if available.
 */
mal_error_e mal_timer_is_valid(mal_timer_e timer);

/**
 * @brief Get a list of valid timers for this MCU.
 * @param timers The returned list.
 * @param size The size of the returned list.
 * @return Returns #MAL_ERROR_OK if available.
 */
mal_error_e mal_timer_get_valid_timers(const mal_timer_e **timers, uint8_t *size);

/**
 * @brief Set the duty cycle of an initialized PWM IO.
 * @param handle The timer of the PWM IO.
 * @param gpio The GPIO the PWM is on.
 * @param duty_cyle A mal_ratio_t value representing the duty cycle on/off
 * ratio.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_set_pwm_duty_cycle(mal_timer_s *handle, const mal_gpio_s *gpio, mal_ratio_t duty_cycle);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

#endif /* TIMER_MAL_TIMER2_H_ */
