#ifndef AEGIS_SECURITY_MICROKERNEL_H
#define AEGIS_SECURITY_MICROKERNEL_H

#include <stdint.h>
#include <kernel/types.h>

typedef enum {
    MKERN_PRIV_USER = 0,
    MKERN_PRIV_DRIVER = 1,
    MKERN_PRIV_SYSTEM = 2,
    MKERN_PRIV_KERNEL = 3,
} mkern_privilege_level_t;

typedef struct {
    uint32_t component_id;
    char name[64];
    mkern_privilege_level_t privilege;
    uint64_t memory_limit;
    uint64_t syscall_mask;
    uint8_t hw_access_allowed;
} mkern_component_t;

int mkern_init(void);
int mkern_register_component(mkern_component_t *component);
int mkern_unregister_component(uint32_t component_id);
int mkern_set_privilege_level(uint32_t component_id, mkern_privilege_level_t level);
int mkern_set_memory_limit(uint32_t component_id, uint64_t limit_bytes);
int mkern_set_syscall_mask(uint32_t component_id, uint64_t syscall_mask);
int mkern_allow_hw_access(uint32_t component_id);
int mkern_revoke_hw_access(uint32_t component_id);
int mkern_enforce_isolation(uint32_t component_id1, uint32_t component_id2);
int mkern_allow_ipc(uint32_t component_id1, uint32_t component_id2);
int mkern_verify_syscall_allowed(uint32_t component_id, uint32_t syscall_num);
int mkern_enable_strict_mode(void);

/* Legacy / Unit Test Aliases */
int microkernel_init(void);
int microkernel_isolate_driver(int driver_id);

#endif
