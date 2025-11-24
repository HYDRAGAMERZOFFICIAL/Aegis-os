set(ARM_SOURCES
    mm.c
    irq.c
    context_switch.c
    gic.c
)

add_library(arch_arm OBJECT ${ARM_SOURCES})
target_include_directories(arch_arm PUBLIC ${INCLUDE_DIR})
target_compile_options(arch_arm PRIVATE -march=armv8-a -mtune=generic)
