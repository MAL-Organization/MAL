set(MAL_TOOLCHAIN_NAME "gcc")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS};-pthread)

set(MAL_LIB_PATH ${PROJECT_SOURCE_DIR}/../library/build-gcc/libmal.a)
