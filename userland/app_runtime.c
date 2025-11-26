#include <userland/app_runtime.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    app_instance_t *apps;
    u32 app_count;
    env_var_t *env_vars;
    u32 env_var_count;
    u32 max_apps;
    u32 max_env_vars;
    u64 next_app_id;
} runtime_state_t;

static runtime_state_t runtime_state = {0};

int app_runtime_init(void)
{
    runtime_state.max_apps = 2048;
    runtime_state.max_env_vars = 4096;
    runtime_state.app_count = 0;
    runtime_state.env_var_count = 0;
    runtime_state.next_app_id = 1;

    runtime_state.apps = (app_instance_t *)calloc(runtime_state.max_apps, sizeof(app_instance_t));
    if (!runtime_state.apps) return -1;

    runtime_state.env_vars = (env_var_t *)calloc(runtime_state.max_env_vars, sizeof(env_var_t));
    if (!runtime_state.env_vars) return -1;

    return 0;
}

app_instance_t *app_runtime_start_app(const char *app_path, app_type_t type)
{
    if (!app_path) return NULL;

    if (runtime_state.app_count >= runtime_state.max_apps) return NULL;

    app_instance_t *app = &runtime_state.apps[runtime_state.app_count];
    app->app_id = runtime_state.next_app_id++;
    app->app_name = app_path;
    app->app_path = app_path;
    app->type = type;
    app->state = APP_STATE_STARTING;
    app->memory_usage = 0;
    app->cpu_usage = 0;
    app->pid = app->app_id;

    runtime_state.app_count++;

    return app;
}

int app_runtime_stop_app(u64 app_id)
{
    for (u32 i = 0; i < runtime_state.app_count; i++) {
        if (runtime_state.apps[i].app_id == app_id) {
            runtime_state.apps[i].state = APP_STATE_STOPPING;
            runtime_state.apps[i] = runtime_state.apps[runtime_state.app_count - 1];
            runtime_state.app_count--;
            return 0;
        }
    }

    return -1;
}

int app_runtime_pause_app(u64 app_id)
{
    for (u32 i = 0; i < runtime_state.app_count; i++) {
        if (runtime_state.apps[i].app_id == app_id) {
            runtime_state.apps[i].state = APP_STATE_PAUSED;
            return 0;
        }
    }

    return -1;
}

int app_runtime_resume_app(u64 app_id)
{
    for (u32 i = 0; i < runtime_state.app_count; i++) {
        if (runtime_state.apps[i].app_id == app_id) {
            runtime_state.apps[i].state = APP_STATE_RUNNING;
            return 0;
        }
    }

    return -1;
}

int app_runtime_load_wasm_module(const u8 *module, u64 module_size)
{
    if (!module || module_size == 0) return -1;

    return 0;
}

int app_runtime_create_container(const char *app_name, const char *image)
{
    if (!app_name || !image) return -1;

    return 0;
}

int app_runtime_create_vm(const char *vm_name, u64 memory, u32 cpus)
{
    if (!vm_name || memory == 0 || cpus == 0) return -1;

    return 0;
}

app_state_t app_runtime_get_app_state(u64 app_id)
{
    for (u32 i = 0; i < runtime_state.app_count; i++) {
        if (runtime_state.apps[i].app_id == app_id) {
            return runtime_state.apps[i].state;
        }
    }

    return APP_STATE_STOPPED;
}

int app_runtime_set_environment(u64 app_id, env_var_t *vars, u32 var_count)
{
    if (!vars || var_count == 0) return -1;

    for (u32 i = 0; i < runtime_state.app_count; i++) {
        if (runtime_state.apps[i].app_id == app_id) {
            if (runtime_state.env_var_count + var_count > runtime_state.max_env_vars) return -1;

            for (u32 j = 0; j < var_count; j++) {
                runtime_state.env_vars[runtime_state.env_var_count++] = vars[j];
            }

            return 0;
        }
    }

    return -1;
}

int app_runtime_get_resource_usage(u64 app_id, u64 *memory, u64 *cpu)
{
    if (!memory || !cpu) return -1;

    for (u32 i = 0; i < runtime_state.app_count; i++) {
        if (runtime_state.apps[i].app_id == app_id) {
            *memory = runtime_state.apps[i].memory_usage;
            *cpu = runtime_state.apps[i].cpu_usage;
            return 0;
        }
    }

    return -1;
}

app_instance_t **app_runtime_get_running_apps(u32 *count)
{
    if (!count) return NULL;

    app_instance_t **result = (app_instance_t **)calloc(runtime_state.app_count, sizeof(app_instance_t *));
    if (!result) return NULL;

    for (u32 i = 0; i < runtime_state.app_count; i++) {
        result[i] = &runtime_state.apps[i];
    }

    *count = runtime_state.app_count;

    return result;
}

int app_runtime_kill_app(u64 app_id)
{
    return app_runtime_stop_app(app_id);
}

int app_runtime_enable_app_isolation(u64 app_id)
{
    for (u32 i = 0; i < runtime_state.app_count; i++) {
        if (runtime_state.apps[i].app_id == app_id) {
            return 0;
        }
    }

    return -1;
}
