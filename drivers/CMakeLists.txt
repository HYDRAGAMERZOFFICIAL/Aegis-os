project(aegis_drivers)

set(DRIVERS_SOURCES
    input_driver.c
    gpu_driver.c
    storage_driver.c
    network_driver.c
    sensor_driver.c
    driver_manager.c
)

add_library(drivers_lib STATIC ${DRIVERS_SOURCES})
target_include_directories(drivers_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(drivers_lib PRIVATE -Wall -Wextra -Werror)
