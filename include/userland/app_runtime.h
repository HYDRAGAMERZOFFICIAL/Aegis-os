#ifndef AEGIS_APP_RUNTIME_H
#define AEGIS_APP_RUNTIME_H

#include <kernel/types.h>

typedef enum {
    APP_TYPE_NATIVE,
    APP_TYPE_WASM,
    APP_TYPE_CONTAINER,
    APP_TYPE_VIRTUAL_MACHINE
} app_type_t;

typedef enum {
    APP_STATE_STOPPED,
    APP_STATE_STARTING,
    APP_STATE_RUNNING,
    APP_STATE_PAUSED,
    APP_STATE_STOPPING,
    APP_STATE_CRASHED
} app_state_t;

typedef struct {
    u64 app_id;
    const char *app_name;
    const char *app_path;
    app_type_t type;
    app_state_t state;
    u64 memory_usage;
    u64 cpu_usage;
    u64 pid;
} app_instance_t;

typedef struct {
    const char *key;
    const char *value;
} env_var_t;

int app_runtime_init(void);
app_instance_t *app_runtime_start_app(const char *app_path, app_type_t type);
int app_runtime_stop_app(u64 app_id);
int app_runtime_pause_app(u64 app_id);
int app_runtime_resume_app(u64 app_id);
int app_runtime_load_wasm_module(const u8 *module, u64 module_size);
int app_runtime_create_container(const char *app_name, const char *image);
int app_runtime_create_vm(const char *vm_name, u64 memory, u32 cpus);
app_state_t app_runtime_get_app_state(u64 app_id);
int app_runtime_set_environment(u64 app_id, env_var_t *vars, u32 var_count);
int app_runtime_get_resource_usage(u64 app_id, u64 *memory, u64 *cpu);
app_instance_t **app_runtime_get_running_apps(u32 *count);
int app_runtime_kill_app(u64 app_id);
int app_runtime_enable_app_isolation(u64 app_id);

#endif
