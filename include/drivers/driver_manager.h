#ifndef __DRIVER_MANAGER_H__
#define __DRIVER_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>

#define MAX_DRIVERS 128
#define MAX_DRIVER_NAME 64
#define MAX_DRIVER_PATH 256

typedef enum {
    DRIVER_CLASS_INPUT = 0,
    DRIVER_CLASS_GPU = 1,
    DRIVER_CLASS_STORAGE = 2,
    DRIVER_CLASS_NETWORK = 3,
    DRIVER_CLASS_SENSOR = 4,
    DRIVER_CLASS_AUDIO = 5,
    DRIVER_CLASS_USB = 6,
    DRIVER_CLASS_UNKNOWN = 7
} driver_class_t;

typedef enum {
    DRIVER_STATUS_UNINSTALLED = 0,
    DRIVER_STATUS_INSTALLING = 1,
    DRIVER_STATUS_INSTALLED = 2,
    DRIVER_STATUS_LOADING = 3,
    DRIVER_STATUS_LOADED = 4,
    DRIVER_STATUS_FAILED = 5,
    DRIVER_STATUS_UNLOADING = 6
} driver_status_t;

typedef struct {
    char name[MAX_DRIVER_NAME];
    char path[MAX_DRIVER_PATH];
    driver_class_t class;
    driver_status_t status;
    uint32_t version;
    uint32_t size;
    void* handle;
    uint32_t flags;
    int error_code;
} driver_t;

typedef struct {
    uint32_t total_drivers;
    uint32_t installed_drivers;
    uint32_t loaded_drivers;
    uint32_t failed_drivers;
    driver_t drivers[MAX_DRIVERS];
} driver_registry_t;

int driver_manager_init(void);
int driver_manager_shutdown(void);

int driver_install(const char* name, const char* path, driver_class_t class);
int driver_uninstall(const char* name);
int driver_load(const char* name);
int driver_unload(const char* name);
int driver_reload(const char* name);

int driver_get_status(const char* name);
driver_t* driver_get_info(const char* name);
driver_t** driver_list_by_class(driver_class_t class, uint32_t* count);

int driver_enable(const char* name);
int driver_disable(const char* name);
int driver_is_enabled(const char* name);

int driver_verify_signature(const char* path);
int driver_check_compatibility(const char* name);
int driver_get_error(const char* name);

driver_registry_t* driver_get_registry(void);
int driver_registry_save(const char* path);
int driver_registry_load(const char* path);

int driver_install_dependencies(const char* name);
int driver_resolve_conflicts(const char* name1, const char* name2);

int driver_update_available(const char* name);
int driver_install_update(const char* name);

int driver_rollback(const char* name);
int driver_backup(const char* name);

#endif
