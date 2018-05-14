cmake_minimum_required(VERSION 3.10)
project(mal)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)

set(OPTIMIZATION_LVL -O0)
set(DEBUG_LVL -g3)

include_directories(src)

# Top level source files
set(ADC_SRCS src/adc/mal_adc.c src/adc/mal_adc.h)
set(CAN_SRCS src/can/mal_can.h src/can/mal_can_buffer.c src/can/mal_can_buffer.h)
set(CLOCK_SRCS src/clock/mal_clock.c src/clock/mal_clock.h)
set(DAC_SRCS src/dac/mal_dac.c src/dac/mal_dac.h)
set(FLASH_SRCS src/flash/mal_flash.c src/flash/mal_flash.h)
set(GPIO_SRCS src/gpio/mal_gpio.h)
set(HSPEC_SRCS src/hspec/mal_hspec.h)
set(I2C_SRCS src/i2c/mal_i2c.h src/i2c/mal_i2c_buffer.c src/i2c/mal_i2c_buffer.h)
set(POWER_SRCS src/power/mal_power.h)
set(RESET_SRCS src/reset/mal_reset.c src/reset/mal_reset.h)
set(SERIAL_SRCS src/serial/mal_serial.h src/serial/mal_serial_buffer.c src/serial/mal_serial_buffer.h)
set(SPI_SRCS src/spi/mal_spi.h)
set(STARTUP_SRCS src/startup/mal_startup.c src/startup/mal_startup.h)
set(STD_SRCS src/std/mal_error.h src/std/mal_stdint.h src/std/mal_bool.h src/std/mal_defs.h src/std/mal_math.c
        src/std/mal_math.h src/std/mal_stdlib.h src/std/mal_types.h)
set(TIMER_SRCS src/timer/mal_timer2.c src/timer/mal_timer2.h)
set(UTILS_SRCS src/utils/mal_circular_buffer.c src/utils/mal_circular_buffer.h src/utils/mal_e3prom.c
        src/utils/mal_e3prom.h src/utils/mal_pool.c src/utils/mal_pool.h)
set(UUID_SRCS src/uuid/mal_uuid.h)
set(WATCHDOG_SRCS src/watchdog/mal_watchdog.c src/watchdog/mal_watchdog.h)
#Common source files to all configurations
set(COMMON_SRCS ${UTILS_SRCS};${ADC_SRCS};${CAN_SRCS};${CLOCK_SRCS};${DAC_SRCS};${FLASH_SRCS};${GPIO_SRCS};
        ${HSPEC_SRCS};${I2C_SRCS};${POWER_SRCS};${RESET_SRCS};${SERIAL_SRCS};${SPI_SRCS};${STARTUP_SRCS};${STD_SRCS};
        ${TIMER_SRCS};${UUID_SRCS};${WATCHDOG_SRCS})

set(COMMON_OPTIONS ${OPTIMIZATION_LVL};${DEBUG_LVL})
# Change compiler for libraries
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
# Change archiver
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_RANLIB arm-none-eabi-ranlib)
# ARM section
set(COMMON_ARM_INCS cmsis;cortexm)
set(COMMON_ARM_SRCS ${COMMON_SRCS};cmsis/cmsis/cmsis_gcc.h;cmsis/cmsis/core_cmFunc.h;cmsis/cmsis/core_cmInstr.h;
        cmsis/cmsis/core_cmSimd.h;cortexm/cortexm/exception_handlers.c;cortexm/cortexm/exception_handlers.h)
set(COMMON_ARM_OPTIONS ${COMMON_OPTIONS};-mthumb;-fmessage-length=0;-fsigned-char;-ffunction-sections;-fdata-sections;
        -ffreestanding)
# STM sections
set(COMMON_STM_INCS ${COMMON_ARM_INCS};stm)
set(COMMON_STM_SRCS ${COMMON_ARM_SRCS};src/hspec/stm/mal_hspec_stm.h)
set(COMMON_STM_DEFS -DMAL_STM)
# STM32F7 sections
file(GLOB_RECURSE STM32F7_HAL_SOURCES "stm/stm32f7/*.c" "stm/stm32f7/*.h")
set(COMMON_STM32F7_SRCS ${COMMON_STM_SRCS};src/hspec/stm/stm32f7/mal_hspec_stm32f7.h;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_clock.c;src/hspec/stm/stm32f7/mal_hspec_stm32f7_gpio.c;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_power.c;src/hspec/stm/stm32f7/mal_hspec_stm32f7_reset.c;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_serial.h;src/hspec/stm/stm32f7/mal_hspec_stm32f7_startup.c;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_timer.c;src/hspec/stm/stm32f7/mal_hspec_stm32f7_timer.h;
        ${STM32F7_HAL_SOURCES})
set(COMMON_STM32F7_OPTIONS -mcpu=cortex-m7;${COMMON_ARM_OPTIONS})
set(COMMON_STM32F7_DEFS ${COMMON_STM_DEFS};-DMAL_FLOAT;-DMAL_STM32F7;-DUSE_HAL_DRIVER)
# STM32F767ZI section
add_library(STM32F767ZI STATIC ${COMMON_STM32F7_SRCS})
set_target_properties(STM32F767ZI PROPERTIES OUTPUT_NAME "mal")
set_target_properties(STM32F767ZI PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/STM32F767ZI)
target_include_directories(STM32F767ZI PRIVATE ${COMMON_STM_INCS})
target_compile_options(STM32F767ZI BEFORE PRIVATE ${COMMON_STM32F7_OPTIONS})
target_compile_definitions(STM32F767ZI PRIVATE ${COMMON_STM32F7_DEFS};-DSTM32F767xx)
# STM32F072CB section
add_library(STM32F072CB STATIC ${COMMON_STM_SRCS})
target_compile_options(STM32F072CB BEFORE PRIVATE -mcpu=cortex-m0;${COMMON_ARM_OPTIONS})
target_compile_definitions(STM32F072CB PRIVATE -DMAL_STM)