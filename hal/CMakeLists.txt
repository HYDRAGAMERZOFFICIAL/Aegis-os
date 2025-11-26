add_library(hal
    hal.c
    hal_cpu.c
    hal_firmware.c
    hal_acpi_uefi.c
    hal_gpu.c
    hal_gpu_display.c
    hal_audio.c
    hal_storage.c
    hal_usb.c
    hal_power.c
)

target_include_directories(hal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set_target_properties(hal PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)
