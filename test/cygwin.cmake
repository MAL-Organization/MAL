set(MAL_TOOLCHAIN_NAME "cygwin")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_COMPILER $ENV{CYGWIN_PATH}/gcc.exe)
set(CMAKE_CXX_COMPILER $ENV{CYGWIN_PATH}/g++.exe)
# Change archiver
set(CMAKE_AR $ENV{CYGWIN_PATH}/gcc-ar.exe)
set(CMAKE_RANLIB $ENV{CYGWIN_PATH}/gcc-ranlib.exe)
