project(aegis_devapi)

set(DEVAPI_SOURCES
    core_api.c
    ui_api.c
    fs_api.c
    network_api.c
    crypto_api.c
    sensor_api.c
    container_api.c
)

add_library(devapi_lib STATIC ${DEVAPI_SOURCES})
target_include_directories(devapi_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(devapi_lib PRIVATE -Wall -Wextra -Werror)
