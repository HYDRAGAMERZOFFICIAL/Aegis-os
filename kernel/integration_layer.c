#include "integration_layer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_CALLBACKS 16
#define MAX_DEPENDENCIES_PER_SUBSYSTEM 5

static SubsystemStatus g_statuses[9] = {0};
static SystemHealth g_system_health = {0};
static IntegrationCallback g_callbacks[MAX_CALLBACKS] = {0};
static uint32_t g_callback_count = 0;

typedef struct {
    SubsystemType subsystem;
    SubsystemType dependencies[MAX_DEPENDENCIES_PER_SUBSYSTEM];
    uint32_t dependency_count;
} DependencyMap;

static DependencyMap g_dependencies[] = {
    {SUBSYSTEM_KERNEL, {0}, 0},
    {SUBSYSTEM_SERVICES, {SUBSYSTEM_KERNEL}, 1},
    {SUBSYSTEM_SECURITY, {SUBSYSTEM_KERNEL}, 1},
    {SUBSYSTEM_UI, {SUBSYSTEM_KERNEL, SUBSYSTEM_SERVICES}, 2},
    {SUBSYSTEM_FILESYSTEM, {SUBSYSTEM_KERNEL}, 1},
    {SUBSYSTEM_NETWORK, {SUBSYSTEM_KERNEL, SUBSYSTEM_SERVICES}, 2},
    {SUBSYSTEM_STORAGE, {SUBSYSTEM_KERNEL, SUBSYSTEM_FILESYSTEM}, 2},
    {SUBSYSTEM_CONTAINER, {SUBSYSTEM_KERNEL, SUBSYSTEM_FILESYSTEM, SUBSYSTEM_NETWORK}, 3},
    {SUBSYSTEM_WASM, {SUBSYSTEM_KERNEL}, 1},
};

static void notify_callbacks(SubsystemType subsystem, int event) {
    for (uint32_t i = 0; i < g_callback_count; i++) {
        if (g_callbacks[i]) {
            g_callbacks[i](subsystem, event);
        }
    }
}

int integration_init_all(void) {
    memset(g_statuses, 0, sizeof(g_statuses));
    memset(&g_system_health, 0, sizeof(g_system_health));
    
    for (int i = 0; i < 9; i++) {
        g_statuses[i].subsystem = i;
        g_statuses[i].state = INTEGRATION_STATE_INITIALIZING;
        g_statuses[i].health_score = 100;
        g_statuses[i].initialized_at = time(NULL);
    }
    
    g_system_health.total_subsystems = 9;
    g_system_health.overall_state = INTEGRATION_STATE_INITIALIZING;
    g_system_health.last_check_time = time(NULL);
    
    for (int i = 0; i < 9; i++) {
        g_system_health.statuses[i] = g_statuses[i];
        g_statuses[i].state = INTEGRATION_STATE_READY;
        g_statuses[i].health_score = 100;
    }
    
    g_system_health.overall_state = INTEGRATION_STATE_READY;
    
    notify_callbacks(SUBSYSTEM_KERNEL, 1);
    
    return 0;
}

int integration_init_subsystem(SubsystemType subsystem) {
    if (subsystem < 0 || subsystem >= 9) {
        return -1;
    }
    
    g_statuses[subsystem].state = INTEGRATION_STATE_INITIALIZING;
    g_statuses[subsystem].initialized_at = time(NULL);
    g_statuses[subsystem].health_score = 100;
    
    g_statuses[subsystem].state = INTEGRATION_STATE_READY;
    
    notify_callbacks(subsystem, 1);
    
    return 0;
}

int integration_get_status(SubsystemType subsystem, SubsystemStatus* status) {
    if (subsystem < 0 || subsystem >= 9 || !status) {
        return -1;
    }
    
    *status = g_statuses[subsystem];
    return 0;
}

int integration_get_system_health(SystemHealth* health) {
    if (!health) {
        return -1;
    }
    
    *health = g_system_health;
    return 0;
}

int integration_check_health(void) {
    int all_healthy = 1;
    
    for (int i = 0; i < 9; i++) {
        if (g_statuses[i].state != INTEGRATION_STATE_READY) {
            all_healthy = 0;
            g_system_health.overall_state = INTEGRATION_STATE_DEGRADED;
        }
        
        g_system_health.statuses[i] = g_statuses[i];
    }
    
    if (all_healthy) {
        g_system_health.overall_state = INTEGRATION_STATE_READY;
    }
    
    g_system_health.last_check_time = time(NULL);
    
    return all_healthy ? 0 : -1;
}

int integration_handle_fault(SubsystemType subsystem, const char* error) {
    if (subsystem < 0 || subsystem >= 9 || !error) {
        return -1;
    }
    
    g_statuses[subsystem].state = INTEGRATION_STATE_FAILED;
    strncpy(g_statuses[subsystem].error_msg, error, sizeof(g_statuses[subsystem].error_msg) - 1);
    g_statuses[subsystem].health_score = 0;
    
    g_system_health.overall_state = INTEGRATION_STATE_DEGRADED;
    
    notify_callbacks(subsystem, 2);
    
    return 0;
}

int integration_recover(SubsystemType subsystem) {
    if (subsystem < 0 || subsystem >= 9) {
        return -1;
    }
    
    if (g_statuses[subsystem].state == INTEGRATION_STATE_FAILED) {
        g_statuses[subsystem].state = INTEGRATION_STATE_READY;
        g_statuses[subsystem].health_score = 100;
        memset(g_statuses[subsystem].error_msg, 0, sizeof(g_statuses[subsystem].error_msg));
        
        notify_callbacks(subsystem, 3);
        
        return integration_check_health();
    }
    
    return 0;
}

int integration_get_dependencies(SubsystemType subsystem, SubsystemType* deps, size_t* count) {
    if (subsystem < 0 || subsystem >= 9 || !deps || !count) {
        return -1;
    }
    
    uint32_t map_size = sizeof(g_dependencies) / sizeof(g_dependencies[0]);
    
    for (uint32_t i = 0; i < map_size; i++) {
        if (g_dependencies[i].subsystem == subsystem) {
            *count = g_dependencies[i].dependency_count;
            
            for (uint32_t j = 0; j < *count; j++) {
                deps[j] = g_dependencies[i].dependencies[j];
            }
            
            return 0;
        }
    }
    
    *count = 0;
    return 0;
}

int integration_verify_all(void) {
    int all_verified = 1;
    
    for (int i = 0; i < 9; i++) {
        SubsystemType deps[MAX_DEPENDENCIES_PER_SUBSYSTEM];
        size_t dep_count = 0;
        
        integration_get_dependencies(i, deps, &dep_count);
        
        for (uint32_t j = 0; j < dep_count; j++) {
            if (g_statuses[deps[j]].state != INTEGRATION_STATE_READY) {
                all_verified = 0;
                g_statuses[i].state = INTEGRATION_STATE_DEGRADED;
            }
        }
    }
    
    return all_verified ? 0 : -1;
}

int integration_register_callback(IntegrationCallback callback) {
    if (!callback || g_callback_count >= MAX_CALLBACKS) {
        return -1;
    }
    
    g_callbacks[g_callback_count++] = callback;
    return 0;
}
