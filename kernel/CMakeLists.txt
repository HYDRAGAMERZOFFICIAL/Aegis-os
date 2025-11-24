set(KERNEL_SOURCES
    process.c
    memory.c
    scheduler.c
    interrupt.c
    filesystem.c
    ipc.c
    network.c
    driver.c
    security.c
    main.c
    system_init.c
    adaptive_scheduler.c
    ram_compression.c
    gpu_acceleration.c
    ai_prefetch.c
    universal_cache.c
    tiered_storage.c
)

add_library(kernel_lib STATIC ${KERNEL_SOURCES})
target_include_directories(kernel_lib PUBLIC ${INCLUDE_DIR})
target_link_libraries(kernel_lib PUBLIC arch_common)

if(ARCH STREQUAL "x86_64")
    target_link_libraries(kernel_lib PRIVATE arch_x86_64)
elseif(ARCH STREQUAL "arm")
    target_link_libraries(kernel_lib PRIVATE arch_arm)
endif()

add_executable(aegis_kernel_image kernel_image.c)
target_link_libraries(aegis_kernel_image PRIVATE kernel_lib)
