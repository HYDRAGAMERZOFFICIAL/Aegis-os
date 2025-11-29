cmake_minimum_required(VERSION 3.20)
project(AegisOS VERSION 1.0.0 LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wextra -Werror")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Werror -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Werror -fPIC")

set(KERNEL_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/kernel")
set(ARCH_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/arch")
set(COMMON_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories(${INCLUDE_DIR})

set(ARCH "x86_64" CACHE STRING "Target architecture: x86_64 or arm")
set_property(CACHE ARCH PROPERTY STRINGS x86_64 arm)

if(ARCH STREQUAL "x86_64")
    set(ARCH_NAME "x86_64")
    enable_language(ASM)
    set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
elseif(ARCH STREQUAL "arm")
    set(ARCH_NAME "arm")
    enable_language(ASM)
    set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
endif()

add_subdirectory(arch)
add_subdirectory(kernel)
add_subdirectory(common)
add_subdirectory(services)
add_subdirectory(security)
add_subdirectory(userland)
add_subdirectory(filesystem)
add_subdirectory(drivers)
add_subdirectory(devapi)
add_subdirectory(hal)

enable_testing()
add_subdirectory(tests)
