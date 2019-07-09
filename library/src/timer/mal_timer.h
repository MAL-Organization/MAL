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

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"
#include "std/mal_types.h"
#include "gpio/mal_gpio_definitions.h"
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
 * Timer PWM handle that must be defined by hardware specific implementation.
 * Used to access the timer pwm functions.
 */
typedef struct MAL_TIMER_PWM mal_timer_pwm_s;

/**
 * Timer input capture handle that must be defined by hardware specific
 * implementation. Used to access the timer input capture functions.
 */
typedef struct MAL_TIMER_INPUT_CAPTURE mal_timer_input_capture_s;

/**
 * @brief Function pointer typdef for timer in task mode.
 * @param handle The given handle during initialization.
 */
typedef void (*mal_timer_callback_t)(void *handle);

/**
 * This structure is used to retain interrupt status between disable and
 * restore. Must be defined by the hardware specific implementation.
 */
typedef struct MAL_TIMER_INTERRUPT_STATE mal_timer_interrupt_state_s;

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
 * Initialization parameters of a PWM output.
 */
typedef struct {
    mal_timer_e timer; /**< The timer to use for the PWM output.*/
    mal_hertz_t frequency; /**< The frequency of the PWM.*/
    mal_hertz_t delta; /**< The acceptable frequency delta.*/
    mal_gpio_port_e port; /**< The port of the PWM GPIO to initialize.*/
    uint8_t pin; /**< The pin of the port of the PWM GPIO to initialize. */
} mal_timer_init_pwm_s;

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
 * @param handle The callback handle given during initialization.
 * @param count The captured count.
 */
typedef void (*mal_timer_input_capture_callback_t)(void *handle, uint64_t count);

/**
 * Initialization parameters of a capture input.
 */
typedef struct {
    mal_timer_e timer; /**< The timer to use for the input capture.*/
    mal_hertz_t frequency; /**< The frequency to count to.*/
    mal_gpio_port_e port; /**< The port of the input capture GPIO to initialize.*/
    uint8_t pin; /**< The pin of the port of the input capture GPIO to initialize. */
    mal_timer_input_e input_event; /**< The input event to capture.*/
    uint8_t input_divider; /**< Specifies after how many events the capture happens.*/
    mal_timer_input_capture_callback_t callback; /**< The callback to be executed when capture occurs.*/
    void *callback_handle; /**< This will be passed back at callback execution.*/
} mal_timer_init_intput_capture_s;

typedef struct {
    mal_timer_e timer; /**< The timer to use for the count.*/
    mal_gpio_port_e port; /**< The port of the count GPIO to initialize.*/
    uint8_t pin; /**< The pin of the port of the count GPIO to initialize. */
    mal_hertz_t maximum_frequency; /**< Maximum expected frequency. This is used to configure input filters if available.*/
    mal_ratio_t duty_cycle; /**< The duty cycle of the input frequency. This is used to configure input filters if available.*/
} mal_timer_init_count_input_s;

/**
 * Initialization parameters of a timer in count mode.
 */
typedef struct {
    mal_timer_e timer; /**< The timer to use for the count.*/
    mal_hertz_t frequency; /**< The frequency to count at.*/
} mal_timer_init_count_s;

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
 * @param timer The timer to disable the interrupt.
 * @param state The state to use to restore interrupts.
 */
MAL_DEFS_INLINE void mal_timer_disable_interrupt(mal_timer_s *handle, mal_timer_interrupt_state_s *state);

/**
 * @brief Enable interrupts for a timer.
 * @param timer The timer to restore the interrupt to.
 * @param state The state given by the disable function.
 */
MAL_DEFS_INLINE void mal_timer_restore_interrupt(mal_timer_s *handle, mal_timer_interrupt_state_s *state);

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
 * subsequent timer functions.
 * @param pwm_handle The PWM handle to initialize. This handle is used to
 * access subsequent PWM functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_pwm(mal_timer_init_pwm_s *init, mal_timer_s *handle, mal_timer_pwm_s *pwm_handle);

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
 * @param handle The handle of the PWM IO.
 * @param duty_cyle A mal_ratio_t value representing the duty cycle on/off
 * ratio.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_set_pwm_duty_cycle(mal_timer_pwm_s *handle, mal_ratio_t duty_cycle);

/**
 * @brief Set the frequency of an initialized timer..
 * @param handle The handle of the timer.
 * @param frequency The frequency to set.
 * @param delta The error tolerated in the frequency to set.
 * @return #MAL_ERROR_OK on success.
 * @note If changing the frequency for a timer with PWM IOs, the duty cycle should be reset for these IOs. after this
 * change.
 */
mal_error_e mal_timer_set_frequency(mal_timer_s *handle, mal_hertz_t frequency, mal_hertz_t delta);

/**
 * @brief Set the frequency of an initialized timer. Note that implementations of timer init methods strive to find the
 * most precise timer divider for the given frequency. This method is method is meant for speed, hence it will not
 * search for the most optimal settings and instead used the settings found during the init to simplify the computation.
 * This means the found frequency will be less precise and the delta should be chosen accordingly.
 * @param handle The handle of the timer.
 * @param frequency The frequency to set.
 * @param delta The error tolerated in the frequency to set.
 * @return #MAL_ERROR_OK on success.
 * @note If changing the frequency for a timer with PWM IOs, the duty cycle should be reset for these IOs. after this
 * change.
 */
mal_error_e mal_timer_fast_set_frequency(mal_timer_s *handle, mal_hertz_t frequency, mal_hertz_t delta);

/**
 * @brief Get the actual counting frequency of a timer.
 * @param handle The timer handle to get the frequency from.
 * @param frequency A pointer to a mal_hertz_t that will contain the counting
 * frequency.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_count_frequency(mal_timer_s *handle, mal_hertz_t *frequency);

/**
 * @brief Get the count register of a timer.
 * @param handle The timer handle to get the count from.
 * @param count A pointer to a uint64_t. It will contain the count.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_get_count(mal_timer_s *handle, uint64_t *count);

/**
 * This is a simple timer initialization. You have to handle the overflow of the timer based on timer resolution.
 * @param init Timer count initialization parameters.
 * @param handle The handle to initialize. This handle is used to access subsequent timer functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_count(mal_timer_init_count_s *init, mal_timer_s *handle);

/**
 * Initialise a timer which will count pulses from an input.
 * @param init Timer input count initialization parameters.
 * @param handle The handle to initialize. This handle is used to access subsequent timer functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_input_count(mal_timer_init_count_input_s *init, mal_timer_s *handle);

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
 * @param handle The handle to initialize. This handle is used to access
 * subsequent timer functions.
 * @param input_capture_handle The input capture handle to initialize. This
 * handle is used to access subsequent PWM functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_init_input_capture(mal_timer_init_intput_capture_s *init, mal_timer_s *handle,
                                         mal_timer_input_capture_s *input_capture_handle);

/**
 * @brief Frees a used timer.
 * @param timer The timer handle to free.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_timer_free(mal_timer_s *handle);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

#endif /* TIMER_MAL_TIMER_H_ */
