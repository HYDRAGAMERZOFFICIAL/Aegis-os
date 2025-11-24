#ifndef AEGIS_SERVICE_MANAGER_H
#define AEGIS_SERVICE_MANAGER_H

#include <kernel/types.h>

typedef enum {
    SERVICE_STATE_STOPPED,
    SERVICE_STATE_STARTING,
    SERVICE_STATE_RUNNING,
    SERVICE_STATE_STOPPING,
    SERVICE_STATE_FAILED,
    SERVICE_STATE_DISABLED
} service_state_t;

typedef struct {
    u64 service_id;
    const char *name;
    service_state_t state;
    u64 pid;
    u64 start_time;
    u64 restart_count;
    bool auto_restart;
    char **dependencies;
    u32 dependency_count;
} service_t;

typedef int (*service_callback_t)(void *arg);

int asm_init(void);
service_t *asm_register_service(const char *name, const char **deps, u32 dep_count);
int asm_start_service(u64 service_id);
int asm_stop_service(u64 service_id);
int asm_restart_service(u64 service_id);
service_state_t asm_get_service_state(u64 service_id);
int asm_set_auto_restart(u64 service_id, bool enabled);
int asm_log_crash(u64 service_id, const char *reason);
int asm_enable_monitoring(u64 service_id);
service_t *asm_get_service(u64 service_id);
u32 asm_get_service_count(void);

#endif
