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

add_library(security_lib STATIC ${SECURITY_SOURCES})
target_include_directories(security_lib PUBLIC ${INCLUDE_DIR})
target_link_libraries(security_lib PUBLIC kernel_lib common_lib)
