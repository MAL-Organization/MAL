cmake_minimum_required(VERSION 3.10)
project(pwm C)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)

# Change archiver
set(CMAKE_AR $ENV{ARM_HOME}/../arm-none-eabi/bin/ar.exe)
set(CMAKE_RANLIB $ENV{ARM_HOME}/../arm-none-eabi/bin/ranlib.exe)

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
    set(SET_OPTIMIZATION O0)
    set(OPTIMIZATION_LVL -O0)
endif()
if (OPTIMIZATION_LVL STREQUAL "-O0")
    set(DEBUG_LVL -g3)
else()
    set(DEBUG_LVL )
endif()

# Include directories
include_directories(src;../../../library/src)

# Sources
set(MAIN_SRCS src/main.c mal/mal_config.c)
set(ALL_SRCS ${MAIN_SRCS})

# Common section
set(COMMON_OPTIONS ${OPTIMIZATION_LVL};${DEBUG_LVL})

# ARM section
set(COMMON_ARM_OPTIONS ${COMMON_OPTIONS};-mthumb;-fmessage-length=0;-fsigned-char;-ffunction-sections;-fdata-sections;
        -ffreestanding)

# STM Section
set(COMMON_STM_DEFS -DMAL_STM)

# STM32F7 Section
set(COMMON_STM32F7_OPTIONS -mcpu=cortex-m7;${COMMON_ARM_OPTIONS})
set(COMMON_STM32F7_DEFS ${COMMON_STM_DEFS};-DMAL_STM32F7;-DMAL_FLOAT;-DUSE_HAL_DRIVER)

# STM32F767 Section
set(COMMON_STM32F765_DEFS ${COMMON_STM32F7_DEFS};-DSTM32F765xx)

if(MAL_TOOLCHAIN_NAME STREQUAL "arm")
    add_executable(STM32F765 ${ALL_SRCS} stm/stm32f765/vectors_stm32f765xx.c)
    set_target_properties(STM32F765 PROPERTIES OUTPUT_NAME ${PROJECT_NAME}.elf)
    target_include_directories(STM32F765 PRIVATE
            ../../../library/stm
            ../../../library/cmsis
            ../../../library/cortexm)
    target_link_libraries(STM32F765 ${MAL_LIBRARY_PATH})
    target_link_libraries(STM32F765 $ENV{ARM_HOME}/../arm-none-eabi/lib/thumb/v7e-m/libm.a)
    target_compile_options(STM32F765 BEFORE PRIVATE ${COMMON_STM32F7_OPTIONS})
    target_compile_definitions(STM32F765 PRIVATE ${COMMON_STM32F765_DEFS})
    if (TARGET STM32F765)
        set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_SOURCE_DIR}/stm/stm32f765/ldscripts/mem.ld -T ${CMAKE_SOURCE_DIR}/stm/stm32f765/ldscripts/sections.ld -Xlinker --gc-sections -Wl,-Map,\"${PROJECT_NAME}.map\" -L\"$ENV{GCC_ARM_HOME}/thumb/v7e-m\" -L\"$ENV{ARM_HOME}/../arm-none-eabi/lib/thumb/v7e-m\"")
        set(CMAKE_EXE_LINKER_FLAGS "-v ${CMAKE_EXE_LINKER_FLAGS}")
    endif()

    # Generate HEX file
    set(HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex)
    set(BIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin)
    add_custom_command(TARGET STM32F765 POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex ${PROJECT_NAME}.elf ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${BIN_FILE}
            COMMAND $ENV{ARM_HOME}/arm-none-eabi-size.exe ${PROJECT_NAME}.elf
            COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
endif()
