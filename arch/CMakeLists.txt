add_subdirectory(x86_64)
add_subdirectory(arm)

set(ARCH_COMMON_SOURCES
    cpu.c
    boot.c
)

add_library(arch_common OBJECT ${ARCH_COMMON_SOURCES})
target_include_directories(arch_common PUBLIC ${INCLUDE_DIR})
