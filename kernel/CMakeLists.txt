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
    adaptive_scheduler.c
    ram_compression.c
    gpu_acceleration.c
    ai_prefetch.c
    universal_cache.c
    tiered_storage.c
)

add_library(aegis_kernel STATIC ${KERNEL_SOURCES})
target_include_directories(aegis_kernel PUBLIC ${INCLUDE_DIR})
target_link_libraries(aegis_kernel PUBLIC arch_common)

if(ARCH STREQUAL "x86_64")
    target_link_libraries(aegis_kernel PRIVATE arch_x86_64)
elseif(ARCH STREQUAL "arm")
    target_link_libraries(aegis_kernel PRIVATE arch_arm)
endif()

add_executable(aegis_kernel_image kernel_image.c)
target_link_libraries(aegis_kernel_image PRIVATE aegis_kernel)
