/*
 * Copyright (c) 2018 Olivier Allaire
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

#include "mal_hspec_stm32f7_can.h"
#include "can/mal_can.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "stm32f7/stm32f7xx_hal_gpio_ex.h"
#include "mal_hspec_stm32f7_gpio.h"
#include "hspec/stm/mal_hspec_stm_bcan.h"

static mal_error_e mal_hspec_stm32f7_can_get_alternate(mal_can_e interface, uint32_t *alternate);
static mal_error_e mal_hspec_stm32f7_can_init_io(mal_can_e interface, mal_gpio_port_e port, uint8_t pin);
static mal_error_e mal_hspec_stm32f7_can_get_irqs(mal_can_e interface, IRQn_Type *f0_irq, IRQn_Type *f1_irq,
                                                  IRQn_Type *tx_irq);
static mal_error_e mal_hspec_stm32f7_can_transmit_msg(mal_can_s *handle, mal_can_msg_s *msg);

mal_error_e mal_can_init(mal_can_init_s *init, mal_can_s *handle) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    // Initialize CAN clock and set handle
    switch (init->interface) {
        case MAL_CAN_1:
            __HAL_RCC_CAN1_CLK_ENABLE();
            handle->hal_can_handle.Instance = CAN1;
            break;
        case MAL_CAN_2:
            __HAL_RCC_CAN2_CLK_ENABLE();
            handle->hal_can_handle.Instance = CAN2;
            break;
        case MAL_CAN_3:
            __HAL_RCC_CAN3_CLK_ENABLE();
            handle->hal_can_handle.Instance = CAN3;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Initialize RX and TX GPIOs
    mal_result = mal_hspec_stm32f7_can_init_io(init->interface, init->tx_port, init->tx_pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    mal_result = mal_hspec_stm32f7_can_init_io(init->interface, init->rx_port, init->rx_pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Reset interface
    hal_result = HAL_CAN_DeInit(&handle->hal_can_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Get clock configurations
    RCC_ClkInitTypeDef clock_config;
    uint32_t flash_latency;
    HAL_RCC_GetClockConfig(&clock_config, &flash_latency);
    uint64_t pclk = (uint64_t)HAL_RCC_GetPCLK1Freq() / (uint64_t)clock_config.APB1CLKDivider;
    // Initialize CAN module
    handle->hal_can_handle.Init.TimeTriggeredMode = DISABLE;
    handle->hal_can_handle.Init.AutoBusOff = ENABLE;
    handle->hal_can_handle.Init.AutoWakeUp = DISABLE;
    handle->hal_can_handle.Init.AutoRetransmission = ENABLE;
    handle->hal_can_handle.Init.ReceiveFifoLocked = ENABLE;
    // This setting means priority is based on ID instead of request time
    handle->hal_can_handle.Init.TransmitFifoPriority = DISABLE;
    handle->hal_can_handle.Init.Mode = CAN_MODE_NORMAL;
    // Find clock settings
    uint32_t prescaler;
    uint32_t tseg1 = 0;
    int32_t tseg2 = 0;
    uint32_t sjw = 0;
    mal_result = mal_hspec_stm_bcan_get_bit_timing(init->bitrate, pclk, &prescaler, &tseg1, &tseg2, &sjw);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    handle->hal_can_handle.Init.TimeSeg1 = (uint8_t)(tseg1 - 1);
    handle->hal_can_handle.Init.TimeSeg2 = (uint8_t)(tseg2 - 1);
    handle->hal_can_handle.Init.SyncJumpWidth = (uint8_t)(sjw - 1);
    handle->hal_can_handle.Init.Prescaler = (uint16_t)prescaler;
    hal_result = HAL_CAN_Init(&handle->hal_can_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    hal_result = HAL_CAN_Start(&handle->hal_can_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Set handle
    handle->hal_can_handle.Parent = handle;
    handle->interface_active = false;
    // Enable interrupts
    mal_result = mal_hspec_stm32f7_can_get_irqs(init->interface, &handle->f0_irq, &handle->f1_irq, &handle->tx_irq);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    NVIC_EnableIRQ(handle->f0_irq);
    NVIC_EnableIRQ(handle->f1_irq);
    NVIC_EnableIRQ(handle->tx_irq);
    NVIC_SetPriority(handle->f0_irq, 1); // Find a way to manage priorities for interrupts
    NVIC_SetPriority(handle->f1_irq, 1);
    NVIC_SetPriority(handle->tx_irq, 1);
    HAL_CAN_ActivateNotification(&handle->hal_can_handle, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING);

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_can_get_alternate(mal_can_e interface, uint32_t *alternate) {
    switch (interface) {
        case MAL_CAN_1:
            *alternate = GPIO_AF9_CAN1;
            return MAL_ERROR_OK;
        case MAL_CAN_2:
            *alternate = GPIO_AF9_CAN2;
            return MAL_ERROR_OK;
        case MAL_CAN_3:
            *alternate = GPIO_AF11_CAN3;
            return MAL_ERROR_OK;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}

static mal_error_e mal_hspec_stm32f7_can_init_io(mal_can_e interface, mal_gpio_port_e port, uint8_t pin) {
    mal_error_e mal_result;
    uint32_t alternate;
    mal_result = mal_hspec_stm32f7_can_get_alternate(interface, &alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    mal_result = mal_hspec_stm32f7_gpio_init_alternate(port, pin, alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_can_get_irqs(mal_can_e interface, IRQn_Type *f0_irq, IRQn_Type *f1_irq,
                                                  IRQn_Type *tx_irq) {
    switch (interface) {
        case MAL_CAN_1:
            *f0_irq = CAN1_RX0_IRQn;
            *f1_irq = CAN1_RX1_IRQn;
            *tx_irq = CAN1_TX_IRQn;
            return MAL_ERROR_OK;
        case MAL_CAN_2:
            *f0_irq = CAN2_RX0_IRQn;
            *f1_irq = CAN2_RX1_IRQn;
            *tx_irq = CAN2_TX_IRQn;
            return MAL_ERROR_OK;
        case MAL_CAN_3:
            *f0_irq = CAN3_RX0_IRQn;
            *f1_irq = CAN3_RX1_IRQn;
            *tx_irq = CAN3_TX_IRQn;
            return MAL_ERROR_OK;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}

mal_error_e mal_can_deinit(mal_can_s *handle) {
    HAL_StatusTypeDef hal_result;
    // Disable interrupt
    mal_can_disable_interrupt(handle, NULL);
    // Reset interface
    hal_result = HAL_CAN_DeInit(&handle->hal_can_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_can_add_filter(mal_can_s *handle, mal_can_filter_s *filter) {
    HAL_StatusTypeDef hal_result = HAL_OK;
    mal_error_e result;
    // Disable interrupts
    bool active = mal_can_disable_interrupt(handle, NULL);
    // Find a free filter
    uint8_t filter_index;
    result = mal_hspec_stm_bcan_add_filter(&handle->filter_banks, filter, &filter_index);
    if (MAL_ERROR_OK == result) {
        // Initialise filter
        CAN_FilterTypeDef filter_init;
        filter_init.FilterActivation = ENABLE;
        if (MAL_HSPEC_STM_BCAN_FIFO_0 == handle->filter_banks.filter_banks[filter_index].fifo) {
            filter_init.FilterFIFOAssignment = CAN_FILTER_FIFO0;
        } else {
            filter_init.FilterFIFOAssignment = CAN_FILTER_FIFO1;
        }
        filter_init.FilterIdHigh = handle->filter_banks.filter_banks[filter_index].filter.std.id[1];
        filter_init.FilterIdLow = handle->filter_banks.filter_banks[filter_index].filter.std.id[0];
        filter_init.FilterMaskIdHigh = handle->filter_banks.filter_banks[filter_index].filter.std.mask[1];
        filter_init.FilterMaskIdLow = handle->filter_banks.filter_banks[filter_index].filter.std.mask[0];
        filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
        filter_init.FilterBank = filter_index;
        if (MAL_CAN_ID_EXTENDED == filter->id_type) {
            filter_init.FilterScale = CAN_FILTERSCALE_32BIT;
        } else {
            filter_init.FilterScale = CAN_FILTERSCALE_16BIT;
        }
        // Filter bank controls are shared for CAN1 and CAN2
        filter_init.SlaveStartFilterBank = MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE;
        if (handle->hal_can_handle.Instance == CAN2) {
            filter_init.FilterBank += MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE;
        }
        hal_result = HAL_CAN_ConfigFilter(&handle->hal_can_handle, &filter_init);
    }
    mal_can_restore_interrupt(handle, active);

    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return result;
}

mal_error_e mal_can_remove_filter(mal_can_s *handle, mal_can_filter_s *filter) {
    HAL_StatusTypeDef hal_result = HAL_OK;
    // Disable interrupts
    bool active = mal_can_disable_interrupt(handle, NULL);
    // Find filter index
    uint8_t filter_index;
    bool found = mal_hspec_stm_bcan_remove_filter(&handle->filter_banks, filter, &filter_index);
    if (found) {
        // Initialise filter
        CAN_FilterTypeDef filter_init;
        if (handle->filter_banks.filter_banks[filter_index].is_active) {
            filter_init.FilterActivation = ENABLE;
        } else {
            filter_init.FilterActivation = DISABLE;
        }
        filter_init.FilterFIFOAssignment = handle->filter_banks.filter_banks[filter_index].fifo;
        filter_init.FilterIdHigh = handle->filter_banks.filter_banks[filter_index].filter.std.id[1];
        filter_init.FilterIdLow = handle->filter_banks.filter_banks[filter_index].filter.std.id[0];
        filter_init.FilterMaskIdHigh = handle->filter_banks.filter_banks[filter_index].filter.std.mask[1];
        filter_init.FilterMaskIdLow = handle->filter_banks.filter_banks[filter_index].filter.std.mask[0];
        filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
        filter_init.FilterBank = filter_index;
        if (MAL_CAN_ID_EXTENDED == filter->id_type) {
            filter_init.FilterScale = CAN_FILTERSCALE_32BIT;
        } else {
            filter_init.FilterScale = CAN_FILTERSCALE_16BIT;
        }
        // Filter bank controls are shared for CAN1 and CAN2
        filter_init.SlaveStartFilterBank = MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE;
        if (handle->hal_can_handle.Instance == CAN2) {
            filter_init.FilterBank += MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE;
        }
        hal_result = HAL_CAN_ConfigFilter(&handle->hal_can_handle, &filter_init);
    }

    mal_can_restore_interrupt(handle, active);

    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_can_transmit(mal_can_s *handle, mal_can_msg_s *msg) {
    mal_error_e result;
    // Disable interrupts to get true status of TX queue
    bool active = mal_can_disable_interrupt(handle, NULL);
    // Check if queue is empty
    if (!handle->interface_active) {
        handle->interface_active = true;
        result = mal_hspec_stm32f7_can_transmit_msg(handle, msg);
        if (MAL_ERROR_OK == result) {
            HAL_CAN_ActivateNotification(&handle->hal_can_handle, CAN_IT_TX_MAILBOX_EMPTY);
        }
    } else {
        result = MAL_ERROR_HARDWARE_UNAVAILABLE;
    }

    mal_can_restore_interrupt(handle, active);

    return result;
}

static mal_error_e mal_hspec_stm32f7_can_transmit_msg(mal_can_s *handle, mal_can_msg_s *msg) {
    HAL_StatusTypeDef hal_result;
    uint32_t mailbox;
    CAN_TxHeaderTypeDef msg_header;
    if (MAL_CAN_ID_STANDARD == msg->id_type) {
        msg_header.StdId = msg->id;
        msg_header.IDE = CAN_ID_STD;
    } else {
        msg_header.ExtId = msg->id;
        msg_header.IDE = CAN_ID_EXT;
    }
    msg_header.DLC = msg->size;
    msg_header.RTR = CAN_RTR_DATA;
    hal_result = HAL_CAN_AddTxMessage(&handle->hal_can_handle, &msg_header, msg->data, &mailbox);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }

    return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_can_disable_interrupt(mal_can_s *handle, mal_can_interrupt_state_s *state) {

}

//TODO 1 and 2
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
}

//TODO 1 and 2
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
}
