#include <kernel/driver.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    driver_t *drivers[256];
    u32 driver_count;
    uint lock;
} driver_mgr_state_t;

static driver_mgr_state_t driver_state = {0};
static u64 next_driver_id = 1;

int driver_mgr_init(void)
{
    memset(&driver_state, 0, sizeof(driver_state));
    return 0;
}

driver_t *driver_register(const char *name, driver_type_t type, driver_ops_t *ops)
{
    if (!name || !ops) return NULL;

    driver_t *drv = (driver_t *)malloc(sizeof(driver_t));
    if (!drv) return NULL;

    drv->id = next_driver_id++;
    drv->name = name;
    drv->type = type;
    drv->state = DRIVER_STATE_UNLOADED;
    drv->module = NULL;
    drv->sandbox = SANDBOX_MODE_NONE;
    drv->permission_mask = 0;
    drv->probe = NULL;
    drv->remove = NULL;
    drv->suspend = NULL;
    drv->resume = NULL;

    if (driver_state.driver_count < 256) {
        driver_state.drivers[driver_state.driver_count++] = drv;
        return drv;
    }

    free(drv);
    return NULL;
}

int driver_unregister(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            if (driver_state.drivers[i]->state == DRIVER_STATE_RUNNING) {
                return -1;
            }

            for (u32 j = i; j < driver_state.driver_count - 1; j++) {
                driver_state.drivers[j] = driver_state.drivers[j + 1];
            }
            driver_state.driver_count--;
            return 0;
        }
    }

    return -1;
}

int driver_load(const char *driver_path)
{
    if (!driver_path) return -1;
    return 0;
}

int driver_unload(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_t *drv = driver_state.drivers[i];
            if (drv->state == DRIVER_STATE_RUNNING) {
                drv->state = DRIVER_STATE_UNLOADED;
                return 0;
            }
            return -1;
        }
    }

    return -1;
}

int driver_set_sandbox_mode(u64 driver_id, sandbox_mode_t mode)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->sandbox = mode;
            return 0;
        }
    }

    return -1;
}

int driver_set_permissions(u64 driver_id, u32 permissions)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->permission_mask = permissions;
            return 0;
        }
    }

    return -1;
}

int driver_probe_devices(void)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        driver_t *drv = driver_state.drivers[i];
        if (drv->probe) {
            drv->probe(NULL);
        }
    }

    return 0;
}

int driver_enable_isolation(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->sandbox = SANDBOX_MODE_VM;
            return 0;
        }
    }

    return -1;
}

int driver_create_wasm_sandbox(u64 driver_id, const u8 *wasm_module, u64 module_size)
{
    if (!wasm_module) return -1;

    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->sandbox = SANDBOX_MODE_WASM;
            driver_state.drivers[i]->module = (void *)wasm_module;
            return 0;
        }
    }

    return -1;
}

driver_state_t driver_get_state(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            return driver_state.drivers[i]->state;
        }
    }

    return DRIVER_STATE_ERROR;
}

int driver_suspend(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_t *drv = driver_state.drivers[i];
            if (drv->suspend) {
                drv->suspend(NULL);
            }
            drv->state = DRIVER_STATE_SUSPENDED;
            return 0;
        }
    }

    return -1;
}

int driver_resume(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_t *drv = driver_state.drivers[i];
            if (drv->resume) {
                drv->resume(NULL);
            }
            drv->state = DRIVER_STATE_RUNNING;
            return 0;
        }
    }

    return -1;
}
