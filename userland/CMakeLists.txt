set(USERLAND_SOURCES
    aegis_ui.c
    app_runtime.c
    settings_center.c
    aegis_shell.c
    app_store.c
    aegis_ui_theme.c
)

add_library(userland_lib STATIC ${USERLAND_SOURCES})
target_include_directories(userland_lib PUBLIC ${INCLUDE_DIR})
target_link_libraries(userland_lib PUBLIC kernel_lib common_lib)
