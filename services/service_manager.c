#include <services/service_manager.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_SERVICES 256
#define MAX_DEPENDENCIES 16

static service_t g_services[MAX_SERVICES];
static u32 g_service_count = 0;
static u64 g_next_service_id = 1;

int asm_init(void) {
    memset(g_services, 0, sizeof(g_services));
    g_service_count = 0;
    g_next_service_id = 1;
    return 0;
}

service_t *asm_register_service(const char *name, const char **deps, u32 dep_count) {
    if (!name || g_service_count >= MAX_SERVICES) {
        return NULL;
    }

    // Check if service already exists
    for (u32 i = 0; i < g_service_count; i++) {
        if (strcmp(g_services[i].name, name) == 0) {
            return &g_services[i];
        }
    }

    service_t *svc = &g_services[g_service_count++];
    svc->service_id = g_next_service_id++;
    svc->name = name; // In a real OS, we'd strdup this
    svc->state = SERVICE_STATE_STOPPED;
    svc->pid = 0;
    svc->start_time = 0;
    svc->restart_count = 0;
    svc->auto_restart = true;
    
    if (deps && dep_count > 0) {
        if (dep_count > MAX_DEPENDENCIES) dep_count = MAX_DEPENDENCIES;
        svc->dependencies = (char **)malloc(sizeof(char*) * dep_count);
        if (svc->dependencies) {
            for (u32 i = 0; i < dep_count; i++) {
                svc->dependencies[i] = (char*)deps[i]; // In real OS, strdup
            }
            svc->dependency_count = dep_count;
        }
    } else {
        svc->dependencies = NULL;
        svc->dependency_count = 0;
    }

    return svc;
}

int asm_start_service(u64 service_id) {
    service_t *svc = asm_get_service(service_id);
    if (!svc) return -1;

    if (svc->state == SERVICE_STATE_RUNNING) return 0;

    // Check dependencies
    if (svc->dependency_count > 0 && svc->dependencies) {
        for (u32 i = 0; i < svc->dependency_count; i++) {
            bool dep_found = false;
            for (u32 j = 0; j < g_service_count; j++) {
                if (strcmp(g_services[j].name, svc->dependencies[i]) == 0) {
                    if (g_services[j].state != SERVICE_STATE_RUNNING) {
                        return -2; // Dependency not running
                    }
                    dep_found = true;
                    break;
                }
            }
            if (!dep_found) return -3; // Dependency missing
        }
    }

    svc->state = SERVICE_STATE_RUNNING;
    svc->pid = 1000 + svc->service_id; // Fake PID
    svc->start_time = 123456789; // Fake timestamp
    return 0;
}

int asm_stop_service(u64 service_id) {
    service_t *svc = asm_get_service(service_id);
    if (!svc) return -1;

    if (svc->state == SERVICE_STATE_STOPPED) return 0;

    svc->state = SERVICE_STATE_STOPPED;
    svc->pid = 0;
    svc->start_time = 0;
    return 0;
}

int asm_restart_service(u64 service_id) {
    int res = asm_stop_service(service_id);
    if (res != 0) return res;
    
    service_t *svc = asm_get_service(service_id);
    if (svc) svc->restart_count++;
    
    return asm_start_service(service_id);
}

service_state_t asm_get_service_state(u64 service_id) {
    service_t *svc = asm_get_service(service_id);
    if (!svc) return SERVICE_STATE_FAILED;
    return svc->state;
}

int asm_set_auto_restart(u64 service_id, bool enabled) {
    service_t *svc = asm_get_service(service_id);
    if (!svc) return -1;
    svc->auto_restart = enabled;
    return 0;
}

int asm_log_crash(u64 service_id, const char *reason) {
    service_t *svc = asm_get_service(service_id);
    if (!svc) return -1;
    
    svc->state = SERVICE_STATE_FAILED;
    // In a real implementation, we would write to a log file here
    return 0;
}

int asm_enable_monitoring(u64 service_id) {
    service_t *svc = asm_get_service(service_id);
    if (!svc) return -1;
    return 0; // Monitoring enabled (simulated)
}

service_t *asm_get_service(u64 service_id) {
    for (u32 i = 0; i < g_service_count; i++) {
        if (g_services[i].service_id == service_id) {
            return &g_services[i];
        }
    }
    return NULL;
}

u32 asm_get_service_count(void) {
    return g_service_count;
}
