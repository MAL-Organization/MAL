cmake_minimum_required(VERSION 3.10)
project(mal C)

function(ConsoleProcessMessage)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${ARGN}")
endfunction()

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)

# Set debug and optimaization
set(CMAKE_C_FLAGS  "")
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE  "")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE  "")
if(SET_OPTIMIZATION)
    set(OPTIMIZATION_LVL -${SET_OPTIMIZATION})
else()
    set(OPTIMIZATION_LVL -O0)
endif()
if (OPTIMIZATION_LVL STREQUAL "-O0")
    set(DEBUG_LVL -g3)
else()
    set(DEBUG_LVL )
endif()

include_directories(src)

# Top level source files
set(ADC_SRCS src/adc/mal_adc.c src/adc/mal_adc.h)
set(CAN_SRCS src/can/mal_can.h src/can/mal_can_buffer.c src/can/mal_can_buffer.h src/can/mal_can_definitions.h)
set(CLOCK_SRCS src/clock/mal_clock.c src/clock/mal_clock.h)
set(DAC_SRCS src/dac/mal_dac.c src/dac/mal_dac.h)
set(FLASH_SRCS src/flash/mal_flash.c src/flash/mal_flash.h)
set(GPIO_SRCS src/gpio/mal_gpio.h src/gpio/mal_gpio_definitions.h)
set(HSPEC_SRCS src/hspec/mal_hspec.h)
set(I2C_SRCS src/i2c/mal_i2c.h src/i2c/mal_i2c_buffer.c src/i2c/mal_i2c_buffer.h)
set(POWER_SRCS src/power/mal_power.h)
set(RESET_SRCS src/reset/mal_reset.c src/reset/mal_reset.h)
set(SERIAL_SRCS src/serial/mal_serial.h src/serial/mal_serial_buffer.c src/serial/mal_serial_buffer.h)
set(SPI_SRCS src/spi/mal_spi.h)
set(STARTUP_SRCS src/startup/mal_startup.c src/startup/mal_startup.h)
set(STD_SRCS src/std/mal_error.h src/std/mal_stdint.h src/std/mal_bool.h src/std/mal_defs.h src/std/mal_math.c
        src/std/mal_math.h src/std/mal_stdlib.h src/std/mal_types.h src/std/mal_string.h src/std/mal_stdarg.h)
set(TIMER_SRCS src/timer/mal_timer.c src/timer/mal_timer.h)
set(UTILS_SRCS src/utils/mal_circular_buffer.c src/utils/mal_circular_buffer.h src/utils/mal_e3prom.c
        src/utils/mal_e3prom.h src/utils/mal_pool.c src/utils/mal_pool.h)
set(UUID_SRCS src/uuid/mal_uuid.h)
set(WATCHDOG_SRCS src/watchdog/mal_watchdog.c src/watchdog/mal_watchdog.h)
#Common source files to all configurations
set(COMMON_SRCS ${UTILS_SRCS};${ADC_SRCS};${CAN_SRCS};${CLOCK_SRCS};${DAC_SRCS};${FLASH_SRCS};${GPIO_SRCS};
        ${HSPEC_SRCS};${I2C_SRCS};${POWER_SRCS};${RESET_SRCS};${SERIAL_SRCS};${SPI_SRCS};${STARTUP_SRCS};${STD_SRCS};
        ${TIMER_SRCS};${UUID_SRCS};${WATCHDOG_SRCS})

set(COMMON_OPTIONS ${OPTIMIZATION_LVL};${DEBUG_LVL};-Wall)

# GNU section
if(MAL_TOOLCHAIN_NAME STREQUAL "mingw")
    # Setting AR in toolchain file does not seem to work
    set(CMAKE_AR mingw32-gcc-ar)
endif()
if(MAL_TOOLCHAIN_NAME STREQUAL "cygwin")
    include_directories($ENV{CYGWIN_PATH}/../usr/include)
    # Setting AR in toolchain file does not seem to work
    set(CMAKE_AR $ENV{CYGWIN_PATH}/gcc-ar.exe)
endif()
if(MAL_TOOLCHAIN_NAME STREQUAL "gcc")
    include_directories("/usr/include")
    # Setting AR in toolchain file does not seem to work
    set(gcc-ar)
endif()
if(MAL_TOOLCHAIN_NAME STREQUAL "mingw" OR MAL_TOOLCHAIN_NAME STREQUAL "cygwin" OR MAL_TOOLCHAIN_NAME STREQUAL "gcc")
    file(GLOB_RECURSE GNU_HAL_SOURCES "src/hspec/gnu/*.c" "src/hspec/gnu/*.h")
    add_library(GNU ${COMMON_SRCS};${GNU_HAL_SOURCES})
    set_target_properties(GNU PROPERTIES OUTPUT_NAME "mal")
    target_compile_options(GNU BEFORE PRIVATE ${COMMON_OPTIONS})
    target_compile_definitions(GNU PRIVATE -DMAL_GNU -DMAL_FLOAT)
    get_target_property(GNU_PROJECT_SOURCES GNU SOURCES)
    ConsoleProcessMessage("~~~~~GNU")
    ConsoleProcessMessage(${GNU_PROJECT_SOURCES})
endif()

# ARM section
set(COMMON_ARM_INCS cmsis;cortexm)
set(COMMON_ARM_SRCS ${COMMON_SRCS};cmsis/cmsis/cmsis_gcc.h;cmsis/cmsis/core_cmFunc.h;cmsis/cmsis/core_cmInstr.h;
        cmsis/cmsis/core_cmSimd.h;cortexm/cortexm/exception_handlers.c;cortexm/cortexm/exception_handlers.h)
set(COMMON_ARM_OPTIONS ${COMMON_OPTIONS};-mthumb;-fmessage-length=0;-fsigned-char;-ffunction-sections;-fdata-sections;
        -ffreestanding)
# STM sections
set(COMMON_STM_INCS ${COMMON_ARM_INCS};stm)
set(COMMON_STM_SRCS ${COMMON_ARM_SRCS};src/hspec/stm/mal_hspec_stm.h;src/hspec/stm/mal_hspec_stm_bcan.h;
        src/hspec/stm/mal_hspec_stm_bcan.c)
set(COMMON_STM_DEFS -DMAL_STM)
# STM32F0 sections
file(GLOB_RECURSE STM32F0_SP_SOURCES "stm/stm32f0/*.c" "stm/stm32f0/*.h")
set(COMMON_STM32F0_SRCS ${COMMON_STM_SRCS};${STM32F0_SP_SOURCES};src/hspec/stm/stm32f0/mal_hspec_stm32f0.h;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0.c;src/hspec/stm/stm32f0/mal_hspec_stm32f0_adc.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_can.h;src/hspec/stm/stm32f0/mal_hspec_stm32f0_can.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_cmn.h;src/hspec/stm/stm32f0/mal_hspec_stm32f0_cmn.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_dac.c;src/hspec/stm/stm32f0/mal_hspec_stm32f0_dma.h;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_flash.c;src/hspec/stm/stm32f0/mal_hspec_stm32f0_gpio.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_i2c.h;src/hspec/stm/stm32f0/mal_hspec_stm32f0_i2c.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_nvic.h;src/hspec/stm/stm32f0/mal_hspec_stm32f0_nvic.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_power.c;src/hspec/stm/stm32f0/mal_hspec_stm32f0_reset.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_serial.h;src/hspec/stm/stm32f0/mal_hspec_stm32f0_serial.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_spi.c;src/hspec/stm/stm32f0/mal_hspec_stm32f0_timer.h;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_timer.c;src/hspec/stm/stm32f0/mal_hspec_stm32f0_uuid.c;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_watchdog.c;src/hspec/stm/stm32f0/mal_hspec_stm32f0_adc.h;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_dac.h;src/hspec/stm/stm32f0/mal_hspec_stm32f0_gpio.h;
        src/hspec/stm/stm32f0/mal_hspec_stm32f0_spi.h;cmsis/cm0/core_cm0.h;cmsis/cm0/core_cm0.c)
set(COMMON_STM32F0_OPTIONS -mcpu=cortex-m0;-mfloat-abi=soft;${COMMON_ARM_OPTIONS})
set(COMMON_STM32F0_DEFS ${COMMON_STM_DEFS};-DMAL_STM32F0;-DUSE_STDPERIPH_DRIVER)
# STM32F072 sections
set(COMMON_STM32F072_SRCS ${COMMON_STM32F0_SRCS};src/hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072.c;
        src/hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_dma.c;
        src/hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_flash.c;
        src/hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_timer.c)
set(COMMON_STM32F072_DEFS ${COMMON_STM32F0_DEFS};-DMAL_STM32F072;-DSTM32F072)
# STM32F7 sections
file(GLOB_RECURSE STM32F7_HAL_SOURCES "stm/stm32f7/*.c" "stm/stm32f7/*.h")
set(COMMON_STM32F7_SRCS ${COMMON_STM_SRCS};src/hspec/stm/stm32f7/mal_hspec_stm32f7.h;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_clock.c;src/hspec/stm/stm32f7/mal_hspec_stm32f7_gpio.c;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_power.c;src/hspec/stm/stm32f7/mal_hspec_stm32f7_reset.c;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_serial.h;src/hspec/stm/stm32f7/mal_hspec_stm32f7_startup.c;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_timer.c;src/hspec/stm/stm32f7/mal_hspec_stm32f7_timer.h;
        cmsis/cm7/core_cm7.h;${STM32F7_HAL_SOURCES};src/hspec/stm/stm32f7/mal_hspec_stm32f7_gpio.h;
        src/hspec/stm/stm32f7/mal_hspec_stm32f7_can.c src/hspec/stm/stm32f7/mal_hspec_stm32f7_can.h src/hspec/stm/stm32f7/mal_hspec_stm32f7_i2c.c src/hspec/stm/stm32f7/mal_hspec_stm32f7_i2c.h src/hspec/stm/stm32f7/mal_hspec_stm32f7_flash.c src/hspec/stm/stm32f7/mal_hspec_stm32f7_flash.h src/hspec/stm/stm32f7/mal_hspec_stm32f7_serial.c src/hspec/stm/stm32f7/mal_hspec_stm32f7_dma.h)
set(COMMON_STM32F7_OPTIONS -mcpu=cortex-m7;${COMMON_ARM_OPTIONS})
set(COMMON_STM32F7_DEFS ${COMMON_STM_DEFS};-DMAL_FLOAT;-DMAL_STM32F7;-DUSE_HAL_DRIVER)
# STM32F76 sections
set(COMMON_STM32F76_SRCS ${COMMON_STM32F7_SRCS};src/hspec/stm/stm32f7/stm32f76/mal_hspec_stm32f76_flash.c src/hspec/stm/stm32f7/stm32f76/mal_hspec_stm32f76_serial.c src/hspec/stm/stm32f7/stm32f76/mal_hspec_stm32f76_dma.c)
if(MAL_TOOLCHAIN_NAME STREQUAL "arm")
    # STM32F765VI section
    add_library(STM32F765VI ${COMMON_STM32F76_SRCS} src/hspec/stm/stm32f7/stm32f76/stm32f765vi/mal_hspec_stm32f765vi_flash.c)
    set_target_properties(STM32F765VI PROPERTIES OUTPUT_NAME "mal")
    target_include_directories(STM32F765VI PRIVATE ${COMMON_STM_INCS})
    target_compile_options(STM32F765VI BEFORE PRIVATE ${COMMON_STM32F7_OPTIONS})
    target_compile_definitions(STM32F765VI PRIVATE ${COMMON_STM32F7_DEFS};-DSTM32F765xx)
    get_target_property(STM32F765VI_PROJECT_SOURCES STM32F765VI SOURCES)
    ConsoleProcessMessage("~~~~~STM32F765VI")
    ConsoleProcessMessage(${STM32F765VI_PROJECT_SOURCES})
    # STM32F767ZI section
    add_library(STM32F767ZI ${COMMON_STM32F76_SRCS})
    set_target_properties(STM32F767ZI PROPERTIES OUTPUT_NAME "mal")
    target_include_directories(STM32F767ZI PRIVATE ${COMMON_STM_INCS})
    target_compile_options(STM32F767ZI BEFORE PRIVATE ${COMMON_STM32F7_OPTIONS})
    target_compile_definitions(STM32F767ZI PRIVATE ${COMMON_STM32F7_DEFS};-DSTM32F767xx)
    get_target_property(STM32F767ZI_PROJECT_SOURCES STM32F767ZI SOURCES)
    ConsoleProcessMessage("~~~~~STM32F767ZI")
    ConsoleProcessMessage(${STM32F767ZI_PROJECT_SOURCES})
    # STM32F072CB section
    add_library(STM32F072CB ${COMMON_STM32F072_SRCS};
            src/hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb.c;
            src/hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_flash.c;
            src/hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_timer.c)
    set_target_properties(STM32F072CB PROPERTIES OUTPUT_NAME "mal")
    target_include_directories(STM32F072CB PRIVATE ${COMMON_STM_INCS})
    target_compile_options(STM32F072CB BEFORE PRIVATE ${COMMON_STM32F0_OPTIONS})
    target_compile_definitions(STM32F072CB PRIVATE ${COMMON_STM32F072_DEFS};-DMAL_STM32F072CB)
    get_target_property(STM32F072CB_PROJECT_SOURCES STM32F072CB SOURCES)
    ConsoleProcessMessage("~~~~~STM32F072CB")
    ConsoleProcessMessage(${STM32F072CB_PROJECT_SOURCES})
endif()
