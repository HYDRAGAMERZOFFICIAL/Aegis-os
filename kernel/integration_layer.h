#ifndef INTEGRATION_LAYER_H
#define INTEGRATION_LAYER_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    SUBSYSTEM_KERNEL = 0,
    SUBSYSTEM_SERVICES = 1,
    SUBSYSTEM_SECURITY = 2,
    SUBSYSTEM_UI = 3,
    SUBSYSTEM_FILESYSTEM = 4,
    SUBSYSTEM_NETWORK = 5,
    SUBSYSTEM_STORAGE = 6,
    SUBSYSTEM_CONTAINER = 7,
    SUBSYSTEM_WASM = 8,
} SubsystemType;

typedef enum {
    INTEGRATION_STATE_OFFLINE = 0,
    INTEGRATION_STATE_INITIALIZING = 1,
    INTEGRATION_STATE_READY = 2,
    INTEGRATION_STATE_DEGRADED = 3,
    INTEGRATION_STATE_FAILED = 4,
} IntegrationState;

typedef struct {
    SubsystemType subsystem;
    IntegrationState state;
    uint64_t initialized_at;
    uint32_t health_score;
    char error_msg[256];
} SubsystemStatus;

typedef struct {
    SubsystemStatus statuses[9];
    uint32_t total_subsystems;
    IntegrationState overall_state;
    uint64_t last_check_time;
} SystemHealth;

int integration_init_all(void);
int integration_init_subsystem(SubsystemType subsystem);
int integration_get_status(SubsystemType subsystem, SubsystemStatus* status);
int integration_get_system_health(SystemHealth* health);
int integration_check_health(void);
int integration_handle_fault(SubsystemType subsystem, const char* error);
int integration_recover(SubsystemType subsystem);
int integration_get_dependencies(SubsystemType subsystem, SubsystemType* deps, size_t* count);
int integration_verify_all(void);

typedef void (*IntegrationCallback)(SubsystemType subsystem, int event);
int integration_register_callback(IntegrationCallback callback);

#endif
