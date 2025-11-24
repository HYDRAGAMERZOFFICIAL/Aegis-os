project(aegis_drivers)

set(DRIVERS_SOURCES
    input_driver.c
    gpu_driver.c
    storage_driver.c
    network_driver.c
    sensor_driver.c
)

add_library(aegis_drivers STATIC ${DRIVERS_SOURCES})
target_include_directories(aegis_drivers PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(aegis_drivers PRIVATE -Wall -Wextra -Werror)
