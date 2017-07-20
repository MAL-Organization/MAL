/*
 * test_mal_hspec_stm32f072cb_timer.c
 *
 *  Created on: Jul 17, 2017
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
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

#include "timer/test_mal_timer.h"
#include "std/mal_stdlib.h"

static const mal_gpio_s valid_timer1_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 7
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 0
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 1
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 13
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 14
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 15
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 8
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 9
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 10
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 11
    }
};

static const mal_gpio_s valid_timer1_input_capture_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 8
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 9
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 10
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 11
    }
};

static const mal_gpio_s valid_timer2_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 1
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 2
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 3
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 10
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 11
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 3
    }
};

static const mal_gpio_s valid_timer3_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 6
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 7
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 0
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 1
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 4
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 5
    }
};

static const mal_gpio_s valid_timer14_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 4
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 7
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 1
    }
};

static const mal_gpio_s valid_timer15_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 1
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 2
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 3
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 14
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 15
    }
};

static const mal_gpio_s valid_timer15_input_capture_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 2
    },
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 3
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 14
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 15
    }
};

static const mal_gpio_s valid_timer16_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 6
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 6
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 8
    }
};

static const mal_gpio_s valid_timer16_input_capture_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 6
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 8
    }
};

static const mal_gpio_s valid_timer17_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 7
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 7
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 9
    }
};

static const mal_gpio_s valid_timer17_input_capture_gpios[] = {
    {
        .port = MAL_GPIO_PORT_A,
        .pin = 7
    },
    {
        .port = MAL_GPIO_PORT_B,
        .pin = 9
    }
};

mal_error_e test_mal_timer_get_valid_pwm_ios(mal_timer_e timer, const mal_gpio_s **ios, uint8_t *size) {
    switch (timer) {
        case MAL_TIMER_1:
            *ios = valid_timer1_gpios;
            *size = sizeof(valid_timer1_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_2:
            *ios = valid_timer2_gpios;
            *size = sizeof(valid_timer2_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_3:
            *ios = valid_timer3_gpios;
            *size = sizeof(valid_timer3_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_14:
            *ios = valid_timer14_gpios;
            *size = sizeof(valid_timer14_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_15:
            *ios = valid_timer15_gpios;
            *size = sizeof(valid_timer15_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_16:
            *ios = valid_timer16_gpios;
            *size = sizeof(valid_timer16_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_17:
            *ios = valid_timer17_gpios;
            *size = sizeof(valid_timer17_gpios) / sizeof(mal_gpio_s);
            break;
        default:
            *ios = NULL;
            break;
    }
    if (NULL == *ios) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_hspec_get_valid_input_capture_ios(mal_timer_e timer, const mal_gpio_s **ios, uint8_t *size) {
    switch (timer) {
        case MAL_TIMER_1:
            *ios = valid_timer1_input_capture_gpios;
            *size = sizeof(valid_timer1_input_capture_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_2:
            // All available channels are valid input capture IOs
            *ios = valid_timer2_gpios;
            *size = sizeof(valid_timer2_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_3:
            // All available channels are valid input capture IOs
            *ios = valid_timer3_gpios;
            *size = sizeof(valid_timer3_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_14:
            // All available channels are valid input capture IOs
            *ios = valid_timer14_gpios;
            *size = sizeof(valid_timer14_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_15:
            *ios = valid_timer15_input_capture_gpios;
            *size = sizeof(valid_timer15_input_capture_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_16:
            *ios = valid_timer16_input_capture_gpios;
            *size = sizeof(valid_timer16_input_capture_gpios) / sizeof(mal_gpio_s);
            break;
        case MAL_TIMER_17:
            *ios = valid_timer17_input_capture_gpios;
            *size = sizeof(valid_timer17_input_capture_gpios) / sizeof(mal_gpio_s);
            break;
        default:
            *ios = NULL;
            break;
    }
    if (NULL == *ios) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

