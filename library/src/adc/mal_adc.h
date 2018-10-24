/**
 * @file mal_adc.h
 * @author Olivier Allaire
 * @date October 24 2018
 * @copyright Copyright (c) 2018 Olivier Allaire
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
 * @brief Functions to access and configure the ADCs of the MCU.
 */


#ifndef ADC_MAL_ADC_H_
#define ADC_MAL_ADC_H_

#include "gpio/mal_gpio_definitions.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "std/mal_bool.h"
#include "std/mal_types.h"
#include "std/mal_defs.h"

/**
 * @defgroup ADC
 * @brief @copybrief mal_adc.h
 * @{
 */

/**
* ADC handle that must be defined by hardware specific implementation. Used
* to access the ADC functions.
*/
typedef struct MAL_ADC mal_adc_s;

/**
 * This structure is used to retain interrupt status between disable and
 * restore. Must be defined by the hardware specific implementation.
 */
typedef struct MAL_ADC_INTERRUPT_STATE mal_adc_interrupt_state_s;

/**
 * The possible ADCs.
 */
typedef enum {
	MAL_ADC_0 = 0,   //!< MAL_ADC_0
	MAL_ADC_1 = 1,   //!< MAL_ADC_1
	MAL_ADC_2 = 2,   //!< MAL_ADC_2
	MAL_ADC_3 = 3,   //!< MAL_ADC_3
	MAL_ADC_4 = 4,   //!< MAL_ADC_4
	MAL_ADC_5 = 5,   //!< MAL_ADC_5
	MAL_ADC_6 = 6,   //!< MAL_ADC_6
	MAL_ADC_7 = 7,   //!< MAL_ADC_7
	MAL_ADC_8 = 8,   //!< MAL_ADC_8
	MAL_ADC_9 = 9,   //!< MAL_ADC_9
	MAL_ADC_10 = 10, //!< MAL_ADC_10
	MAL_ADC_11 = 11, //!< MAL_ADC_11
	MAL_ADC_12 = 12, //!< MAL_ADC_12
	MAL_ADC_13 = 13, //!< MAL_ADC_13
	MAL_ADC_14 = 14, //!< MAL_ADC_14
	MAL_ADC_15 = 15, //!< MAL_ADC_15
    MAL_ADC_16 = 16, //!< MAL_ADC_16
    MAL_ADC_17 = 17, //!< MAL_ADC_17
    MAL_ADC_18 = 18, //!< MAL_ADC_18
    MAL_ADC_19 = 19, //!< MAL_ADC_19
    MAL_ADC_20 = 20, //!< MAL_ADC_20
    MAL_ADC_21 = 21, //!< MAL_ADC_21
    MAL_ADC_22 = 22, //!< MAL_ADC_22
    MAL_ADC_23 = 23, //!< MAL_ADC_23
    MAL_ADC_24 = 24, //!< MAL_ADC_24
    MAL_ADC_25 = 25, //!< MAL_ADC_25
    MAL_ADC_26 = 26, //!< MAL_ADC_26
    MAL_ADC_27 = 27, //!< MAL_ADC_27
    MAL_ADC_28 = 28, //!< MAL_ADC_28
    MAL_ADC_29 = 29, //!< MAL_ADC_29
    MAL_ADC_30 = 30, //!< MAL_ADC_30
    MAL_ADC_31 = 31, //!< MAL_ADC_31
    MAL_ADC_32 = 32, //!< MAL_ADC_32
    MAL_ADC_33 = 33, //!< MAL_ADC_33
    MAL_ADC_34 = 34, //!< MAL_ADC_34
    MAL_ADC_35 = 35, //!< MAL_ADC_35
    MAL_ADC_36 = 36, //!< MAL_ADC_36
    MAL_ADC_37 = 37, //!< MAL_ADC_37
    MAL_ADC_38 = 38, //!< MAL_ADC_38
    MAL_ADC_39 = 39, //!< MAL_ADC_39
    MAL_ADC_40 = 40, //!< MAL_ADC_40
    MAL_ADC_41 = 41, //!< MAL_ADC_41
    MAL_ADC_42 = 42, //!< MAL_ADC_42
    MAL_ADC_43 = 43, //!< MAL_ADC_43
    MAL_ADC_44 = 44, //!< MAL_ADC_44
    MAL_ADC_45 = 45, //!< MAL_ADC_45
    MAL_ADC_46 = 46, //!< MAL_ADC_46
    MAL_ADC_47 = 47, //!< MAL_ADC_47
    MAL_ADC_48 = 48, //!< MAL_ADC_48
    MAL_ADC_49 = 49, //!< MAL_ADC_49
    MAL_ADC_50 = 50, //!< MAL_ADC_50
    MAL_ADC_51 = 51, //!< MAL_ADC_51
    MAL_ADC_52 = 52, //!< MAL_ADC_52
    MAL_ADC_53 = 53, //!< MAL_ADC_53
    MAL_ADC_54 = 54, //!< MAL_ADC_54
    MAL_ADC_55 = 55, //!< MAL_ADC_55
    MAL_ADC_56 = 56, //!< MAL_ADC_56
    MAL_ADC_57 = 57, //!< MAL_ADC_57
    MAL_ADC_58 = 58, //!< MAL_ADC_58
    MAL_ADC_59 = 59, //!< MAL_ADC_59
    MAL_ADC_60 = 60, //!< MAL_ADC_60
    MAL_ADC_61 = 61, //!< MAL_ADC_61
    MAL_ADC_62 = 62, //!< MAL_ADC_62
    MAL_ADC_63 = 63, //!< MAL_ADC_63
    MAL_ADC_64 = 64, //!< MAL_ADC_64
    MAL_ADC_65 = 65, //!< MAL_ADC_65
    MAL_ADC_66 = 66, //!< MAL_ADC_66
    MAL_ADC_67 = 67, //!< MAL_ADC_67
    MAL_ADC_68 = 68, //!< MAL_ADC_68
    MAL_ADC_69 = 69, //!< MAL_ADC_69
    MAL_ADC_70 = 70, //!< MAL_ADC_70
    MAL_ADC_71 = 71, //!< MAL_ADC_71
    MAL_ADC_SIZE = 72
} mal_adc_e;

/**
 * The initialization parameters of an ADC.
 */
typedef struct {
	mal_adc_e adc; /**< To ADC to initialize.*/
	mal_gpio_port_e port; /**< The port of the ADC GPIO to initialize.*/
	uint8_t pin; /**< The pin of the port of the ADC GPIO to initialize. */
	uint8_t bit_resolution; /**< The resolution of the ADC.*/
} mal_adc_init_s;

/**
 * @brief This callback will be executed when an ADC read is complete.
 * @param handle The handle given for the callback.
 * @param value The value read.
 */
typedef void (*mal_adc_read_callback_t)(void *handle, uint64_t value);

/**
 * @brief Read an ADC.
 * @param handle The ADC handle to read from.
 * @param value A pointer of type uint64_t that that will contain the data
 * read.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_read_bits(mal_adc_s *handle, uint64_t *value);

/**
 * @brief Get the resolution of the ADC.
 * @param handle The ADC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_resolution(mal_adc_s *handle, uint8_t *resolution);

/**
 * @brief Start an async read. A callback will be executed once the read is
 * complete.
 * @param handle The ADC to read from.
 * @param callback The callback to execute once the read is complete.
 * @param callback_handle This handle will be passed to the callback funtion.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_async_read(mal_adc_s *handle, mal_adc_read_callback_t callback, void *callback_handle);

/**
 * @brief Disable interrupts for an ADC.
 * @param handle The ADC to disable the interrupt.
 * @param state The state to use to restore interrupts.
 */
MAL_DEFS_INLINE void mal_adc_disable_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state);

/**
 * @brief Enable interrupts for an ADC.
 * @param handle The ADC to enable the interrupt.
 * @param state A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
MAL_DEFS_INLINE void mal_adc_restore_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state);

/**
 * @brief Get the maximum resolution of the ADC.
 * @param adc The ADC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_maximum_resolution(mal_adc_e adc, uint8_t *resolution);

/**
 * @brief Initialize an ADC.
 * @param init The initialization parameters.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_init(mal_adc_init_s *init, mal_adc_s *handle);

/**
 * @brief Read an ADC values, but as volts instead of LSBs.
 * @param handle The ADC to read from.
 * @param value The value read.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_read_volts(mal_adc_s *handle, mal_volts_t *value);

/**
 * @brief Convert a bit value of an ADC to volts.
 * @param handle The ADC from which the bit value was read from.
 * @param bit_value The value read rom the ADC.
 * @param value The converted value.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_bits_to_volts(mal_adc_s *handle, uint64_t bit_value, mal_volts_t *value);

/**
 * Since the ADC driver treats adc channels as separate ADCs, this function allows to get the ADC enum equivalent given
 * the concrete ADC and the channel.
 * @param adc The concrete ADC. Say the MCU has 3 independent ADCs. valid values would be 0, 1 and 2.
 * @param channel The channel within the concrete ADC. Say the MCU has 32 channels on the desired concrete ADC. The
 * valid values are from 0 to 31.
 * @param mal_adc The adc enum value will be returned here.
 * @return Returns #MAL_ERROR_OK if a valid enum value can be generated.
 */
mal_error_e mal_adc_from_channel(uint32_t adc, uint32_t channel, mal_adc_e *mal_adc);

/**
 * Given a concrete ADC, must return the number of channels for that ADC.
 * @param adc The concrete ADC. Say the MCU has 3 independent ADCs. valid values would be 0, 1 and 2.
 * @param channel_count The channel count will be returned here.
 * @return Returns #MAL_ERROR_OK if a valid count is available.
 */
mal_error_e mal_adc_get_channel_count(uint32_t adc, uint32_t *channel_count);

/**
 * Since the ADC driver treats adc channels as separate ADCs, this function allows to get the concrete ADC from the enum
 * value.
 * @param mal_adc The ADC enum value to get the concrete ADC from.
 * @param adc The concrete ADC value is returned here. Say the MCU has 3 independent ADCs. valid values would be 0, 1
 * and 2.
 * @return Returns #MAL_ERROR_OK if a valid ADC is available.
 */
mal_error_e mal_adc_get_adc_from_enum(mal_adc_e mal_adc, uint32_t *adc);

/**
 * Since the ADC driver treats adc channels as separate ADCs, this function allows to get the channel from the enum
 * value.
 * @param mal_adc The ADC enum value to get the channel from.
 * @param channel The channel value within the concrete ADC is returned. Say the MCU has 32 channels on the desired
 * concrete ADC. The valid values are from 0 to 31.
 * @return Returns #MAL_ERROR_OK if a valid channel is available.
 */
mal_error_e mal_adc_get_channel_from_enum(mal_adc_e mal_adc, uint32_t *channel);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* ADC_MAL_ADC_H_ */
