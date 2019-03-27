/*
 * Copyright (c) 2019 Olivier Allaire
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

#include "gpio/mal_gpio.h"
#include "can/mal_can.h"
#include "std/mal_stdlib.h"
#include "std/mal_defs.h"

static mal_error_e tx_callback(void *handle, mal_can_msg_s *next_msg);
static void rx_callback(void *handle, mal_can_msg_s *msg);

static mal_gpio_s standby_gpio;
static mal_can_s can_handle;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(void) {
    mal_error_e result;

    mal_gpio_init_s gpio_init_params;
    gpio_init_params.port = MAL_GPIO_PORT_C;
    gpio_init_params.pin = 10;
    gpio_init_params.direction = MAL_GPIO_DIRECTION_OUT;
    gpio_init_params.output_config = MAL_GPIO_OUT_PP;
    gpio_init_params.pupd = MAL_GPIO_PUPD_NONE;
    gpio_init_params.speed = UINT64_MAX;
    result = mal_gpio_init(&gpio_init_params, &standby_gpio);
    if (MAL_ERROR_OK != result) {
        while (true);
    }

    mal_can_init_s can_init_params;
    can_init_params.interface = MAL_CAN_1;
    can_init_params.tx_port = MAL_GPIO_PORT_A;
    can_init_params.tx_pin = 12;
    can_init_params.rx_port = MAL_GPIO_PORT_A;
    can_init_params.rx_pin = 11;
    can_init_params.bitrate = 125000;
    can_init_params.tx_callback = &tx_callback;
    can_init_params.tx_callback_handle = NULL;
    can_init_params.rx_callback = &rx_callback;
    can_init_params.rx_callback_handle = NULL;
    result = mal_can_init(&can_init_params, &can_handle);
    if (MAL_ERROR_OK != result) {
        while (true);
    }

    mal_can_msg_s msg;
    msg.id = 0x3FF;
    msg.id_type = MAL_CAN_ID_STANDARD;
    msg.size = 0;
    result = mal_can_transmit(&can_handle, &msg);
    if (MAL_ERROR_OK != result) {
        while (true);
    }

    while (true);

    return 0;
}
#pragma clang diagnostic pop

static mal_error_e tx_callback(void *handle, mal_can_msg_s *next_msg) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(next_msg);
    return MAL_ERROR_EMPTY;
}

static void rx_callback(void *handle, mal_can_msg_s *msg) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(msg);
}
