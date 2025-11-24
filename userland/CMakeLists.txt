set(USERLAND_SOURCES
    aegis_ui.c
    app_runtime.c
    settings_center.c
    aegis_shell.c
    app_store.c
)

add_library(aegis_userland STATIC ${USERLAND_SOURCES})
target_include_directories(aegis_userland PUBLIC ${INCLUDE_DIR})
target_link_libraries(aegis_userland PUBLIC aegis_kernel aegis_common)
