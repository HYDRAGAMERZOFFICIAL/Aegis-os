set(X86_64_SOURCES
    mm.c
    irq.c
    context_switch.c
    gdt.c
    idt.c
)

add_library(arch_x86_64 OBJECT ${X86_64_SOURCES})
target_include_directories(arch_x86_64 PUBLIC ${INCLUDE_DIR})
target_compile_options(arch_x86_64 PRIVATE -march=x86-64 -mtune=generic)
