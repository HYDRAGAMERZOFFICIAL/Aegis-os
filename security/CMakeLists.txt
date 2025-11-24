set(SECURITY_SOURCES
    crypto_engine.c
    zero_trust_ac.c
    aegis_shield.c
    secure_boot.c
    app_permissions.c
    syscall_gatekeeper.c
    microkernel.c
    immutable_partitions.c
    aegis_guard.c
    hypervisor_protection.c
)

add_library(aegis_security STATIC ${SECURITY_SOURCES})
target_include_directories(aegis_security PUBLIC ${INCLUDE_DIR})
target_link_libraries(aegis_security PUBLIC aegis_kernel aegis_common)
