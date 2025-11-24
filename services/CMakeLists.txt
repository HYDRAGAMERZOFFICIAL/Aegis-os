set(SERVICES_SOURCES
    service_manager.c
    session_manager.c
    update_manager.c
    network_manager.c
    storage_manager.c
    package_manager.c
    logging_service.c
    audit_service.c
    sandbox_manager.c
    command_palette.c
    system_troubleshoot.c
    task_optimizer.c
    neural_activity_monitor.c
    voice_commands.c
    checkpoint_system.c
)

add_library(services_lib STATIC ${SERVICES_SOURCES})
target_include_directories(services_lib PUBLIC ${INCLUDE_DIR})
target_link_libraries(services_lib PUBLIC kernel_lib)
