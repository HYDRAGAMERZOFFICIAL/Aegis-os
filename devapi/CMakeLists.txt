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

add_library(aegis_devapi STATIC ${DEVAPI_SOURCES})
target_include_directories(aegis_devapi PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(aegis_devapi PRIVATE -Wall -Wextra -Werror)
