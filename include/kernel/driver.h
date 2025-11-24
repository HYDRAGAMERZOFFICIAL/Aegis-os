#ifndef AEGIS_KERNEL_DRIVER_H
#define AEGIS_KERNEL_DRIVER_H

#include <kernel/types.h>

typedef enum {
    DRIVER_TYPE_STORAGE,
    DRIVER_TYPE_NETWORK,
    DRIVER_TYPE_DISPLAY,
    DRIVER_TYPE_INPUT,
    DRIVER_TYPE_AUDIO,
    DRIVER_TYPE_CUSTOM
} driver_type_t;

typedef enum {
    DRIVER_STATE_UNLOADED,
    DRIVER_STATE_LOADING,
    DRIVER_STATE_LOADED,
    DRIVER_STATE_RUNNING,
    DRIVER_STATE_SUSPENDED,
    DRIVER_STATE_ERROR
} driver_state_t;

typedef enum {
    SANDBOX_MODE_NONE,
    SANDBOX_MODE_WASM,
    SANDBOX_MODE_VM,
    SANDBOX_MODE_CONTAINER
} sandbox_mode_t;

typedef struct {
    u32 device_id;
    u32 vendor_id;
    u32 class_code;
} pci_device_t;

typedef struct {
    u64 id;
    const char *name;
    driver_type_t type;
    driver_state_t state;
    void *module;
    sandbox_mode_t sandbox;
    u32 permission_mask;
    int (*probe)(void *dev);
    int (*remove)(void *dev);
    int (*suspend)(void *dev);
    int (*resume)(void *dev);
} driver_t;

typedef struct {
    void (*init)(void);
    void (*destroy)(void);
    int (*open)(void);
    int (*close)(void);
    int (*read)(void *buf, u64 size);
    int (*write)(const void *buf, u64 size);
    int (*ioctl)(u32 cmd, void *arg);
} driver_ops_t;

int driver_mgr_init(void);
driver_t *driver_register(const char *name, driver_type_t type, driver_ops_t *ops);
int driver_unregister(u64 driver_id);
int driver_load(const char *driver_path);
int driver_unload(u64 driver_id);
int driver_set_sandbox_mode(u64 driver_id, sandbox_mode_t mode);
int driver_set_permissions(u64 driver_id, u32 permissions);
int driver_probe_devices(void);
int driver_enable_isolation(u64 driver_id);
int driver_create_wasm_sandbox(u64 driver_id, const u8 *wasm_module, u64 module_size);
driver_state_t driver_get_state(u64 driver_id);
int driver_suspend(u64 driver_id);
int driver_resume(u64 driver_id);

#endif
