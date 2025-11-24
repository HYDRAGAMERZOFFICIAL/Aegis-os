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

add_library(aegis_services STATIC ${SERVICES_SOURCES})
target_include_directories(aegis_services PUBLIC ${INCLUDE_DIR})
target_link_libraries(aegis_services PUBLIC aegis_kernel)
