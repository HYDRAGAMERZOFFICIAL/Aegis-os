#include <security/microkernel.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMPONENTS 64

static mkern_component_t g_components[MAX_COMPONENTS] = {0};
static uint32_t g_component_count = 0;
static uint8_t g_strict_mode = 1;

int mkern_init(void) {
    g_component_count = 0;
    g_strict_mode = 1;
    return 0;
}

int microkernel_init(void) {
    return mkern_init();
}

int mkern_register_component(mkern_component_t *component) {
    if (!component || g_component_count >= MAX_COMPONENTS) return -1;
    component->component_id = g_component_count + 1;
    g_components[g_component_count++] = *component;
    return (int)component->component_id;
}

int mkern_unregister_component(uint32_t component_id) {
    for (uint32_t i = 0; i < g_component_count; i++) {
        if (g_components[i].component_id == component_id) {
            for (uint32_t j = i; j < g_component_count - 1; j++) {
                g_components[j] = g_components[j + 1];
            }
            g_component_count--;
            return 0;
        }
    }
    return -1;
}

int mkern_set_privilege_level(uint32_t component_id, mkern_privilege_level_t level) {
    for (uint32_t i = 0; i < g_component_count; i++) {
        if (g_components[i].component_id == component_id) {
            g_components[i].privilege = level;
            return 0;
        }
    }
    return -1;
}

int mkern_set_memory_limit(uint32_t component_id, uint64_t limit_bytes) {
    for (uint32_t i = 0; i < g_component_count; i++) {
        if (g_components[i].component_id == component_id) {
            g_components[i].memory_limit = limit_bytes;
            return 0;
        }
    }
    return -1;
}

int mkern_set_syscall_mask(uint32_t component_id, uint64_t syscall_mask) {
    for (uint32_t i = 0; i < g_component_count; i++) {
        if (g_components[i].component_id == component_id) {
            g_components[i].syscall_mask = syscall_mask;
            return 0;
        }
    }
    return -1;
}

int mkern_allow_hw_access(uint32_t component_id) {
    for (uint32_t i = 0; i < g_component_count; i++) {
        if (g_components[i].component_id == component_id) {
            g_components[i].hw_access_allowed = 1;
            return 0;
        }
    }
    return -1;
}

int mkern_revoke_hw_access(uint32_t component_id) {
    for (uint32_t i = 0; i < g_component_count; i++) {
        if (g_components[i].component_id == component_id) {
            g_components[i].hw_access_allowed = 0;
            return 0;
        }
    }
    return -1;
}

int mkern_enforce_isolation(uint32_t component_id1, uint32_t component_id2) {
    (void)component_id1;
    (void)component_id2;
    return 0;
}

int mkern_allow_ipc(uint32_t component_id1, uint32_t component_id2) {
    (void)component_id1;
    (void)component_id2;
    return 0;
}

int mkern_verify_syscall_allowed(uint32_t component_id, uint32_t syscall_num) {
    for (uint32_t i = 0; i < g_component_count; i++) {
        if (g_components[i].component_id == component_id) {
            if (g_components[i].syscall_mask & (1ULL << syscall_num)) return 1;
            return 0;
        }
    }
    return 1;
}

int mkern_enable_strict_mode(void) {
    g_strict_mode = 1;
    return 0;
}

int microkernel_isolate_driver(int driver_id) {
    return mkern_revoke_hw_access((uint32_t)driver_id);
}
