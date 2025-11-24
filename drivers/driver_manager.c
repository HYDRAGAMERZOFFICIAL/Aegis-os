#include <drivers/driver_manager.h>
#include <string.h>
#include <stdlib.h>

static driver_registry_t driver_registry = {0};
static int driver_manager_initialized = 0;

int driver_manager_init(void) {
    if (driver_manager_initialized) {
        return 0;
    }

    memset(&driver_registry, 0, sizeof(driver_registry_t));
    driver_manager_initialized = 1;

    return 0;
}

int driver_manager_shutdown(void) {
    if (!driver_manager_initialized) {
        return -1;
    }

    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (driver_registry.drivers[i].status == DRIVER_STATUS_LOADED) {
            driver_unload(driver_registry.drivers[i].name);
        }
    }

    memset(&driver_registry, 0, sizeof(driver_registry_t));
    driver_manager_initialized = 0;

    return 0;
}

int driver_install(const char* name, const char* path, driver_class_t class) {
    if (!driver_manager_initialized) {
        return -1;
    }

    if (driver_registry.total_drivers >= MAX_DRIVERS) {
        return -2;
    }

    if (name == NULL || path == NULL) {
        return -3;
    }

    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (strcmp(driver_registry.drivers[i].name, name) == 0) {
            return -4;
        }
    }

    uint32_t index = driver_registry.total_drivers;
    strncpy(driver_registry.drivers[index].name, name, MAX_DRIVER_NAME - 1);
    strncpy(driver_registry.drivers[index].path, path, MAX_DRIVER_PATH - 1);
    driver_registry.drivers[index].class = class;
    driver_registry.drivers[index].status = DRIVER_STATUS_INSTALLED;
    driver_registry.drivers[index].version = 1;
    driver_registry.drivers[index].flags = 0;
    driver_registry.drivers[index].error_code = 0;

    driver_registry.total_drivers++;
    driver_registry.installed_drivers++;

    return 0;
}

int driver_uninstall(const char* name) {
    if (!driver_manager_initialized) {
        return -1;
    }

    if (name == NULL) {
        return -2;
    }

    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (strcmp(driver_registry.drivers[i].name, name) == 0) {
            if (driver_registry.drivers[i].status == DRIVER_STATUS_LOADED) {
                driver_unload(name);
            }

            if (driver_registry.drivers[i].status == DRIVER_STATUS_INSTALLED ||
                driver_registry.drivers[i].status == DRIVER_STATUS_FAILED) {
                memmove(&driver_registry.drivers[i],
                        &driver_registry.drivers[i + 1],
                        (driver_registry.total_drivers - i - 1) * sizeof(driver_t));
                driver_registry.total_drivers--;
                driver_registry.installed_drivers--;
                return 0;
            }

            return -3;
        }
    }

    return -4;
}

int driver_load(const char* name) {
    if (!driver_manager_initialized) {
        return -1;
    }

    if (name == NULL) {
        return -2;
    }

    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (strcmp(driver_registry.drivers[i].name, name) == 0) {
            if (driver_registry.drivers[i].status == DRIVER_STATUS_LOADED) {
                return 0;
            }

            if (driver_registry.drivers[i].status != DRIVER_STATUS_INSTALLED) {
                return -3;
            }

            driver_registry.drivers[i].status = DRIVER_STATUS_LOADING;

            driver_registry.drivers[i].handle = (void*)1;
            driver_registry.drivers[i].status = DRIVER_STATUS_LOADED;
            driver_registry.loaded_drivers++;

            return 0;
        }
    }

    return -4;
}

int driver_unload(const char* name) {
    if (!driver_manager_initialized) {
        return -1;
    }

    if (name == NULL) {
        return -2;
    }

    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (strcmp(driver_registry.drivers[i].name, name) == 0) {
            if (driver_registry.drivers[i].status != DRIVER_STATUS_LOADED) {
                return -3;
            }

            driver_registry.drivers[i].status = DRIVER_STATUS_UNLOADING;
            driver_registry.drivers[i].handle = NULL;
            driver_registry.drivers[i].status = DRIVER_STATUS_INSTALLED;
            driver_registry.loaded_drivers--;

            return 0;
        }
    }

    return -4;
}

int driver_reload(const char* name) {
    if (driver_unload(name) != 0) {
        return -1;
    }

    return driver_load(name);
}

int driver_get_status(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (strcmp(driver_registry.drivers[i].name, name) == 0) {
            return driver_registry.drivers[i].status;
        }
    }

    return -1;
}

driver_t* driver_get_info(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return NULL;
    }

    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (strcmp(driver_registry.drivers[i].name, name) == 0) {
            return &driver_registry.drivers[i];
        }
    }

    return NULL;
}

driver_t** driver_list_by_class(driver_class_t class, uint32_t* count) {
    if (!driver_manager_initialized || count == NULL) {
        return NULL;
    }

    driver_t** list = (driver_t**)malloc(sizeof(driver_t*) * MAX_DRIVERS);
    if (list == NULL) {
        return NULL;
    }

    *count = 0;
    for (uint32_t i = 0; i < driver_registry.total_drivers; i++) {
        if (driver_registry.drivers[i].class == class) {
            list[*count] = &driver_registry.drivers[i];
            (*count)++;
        }
    }

    return list;
}

int driver_enable(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    driver_t* driver = driver_get_info(name);
    if (driver == NULL) {
        return -2;
    }

    driver->flags |= 0x01;
    return 0;
}

int driver_disable(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    driver_t* driver = driver_get_info(name);
    if (driver == NULL) {
        return -2;
    }

    driver->flags &= ~0x01;
    return 0;
}

int driver_is_enabled(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    driver_t* driver = driver_get_info(name);
    if (driver == NULL) {
        return -1;
    }

    return (driver->flags & 0x01) != 0 ? 1 : 0;
}

int driver_verify_signature(const char* path) {
    if (path == NULL) {
        return -1;
    }

    return 0;
}

int driver_check_compatibility(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    driver_t* driver = driver_get_info(name);
    if (driver == NULL) {
        return -1;
    }

    return 1;
}

int driver_get_error(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    driver_t* driver = driver_get_info(name);
    if (driver == NULL) {
        return -1;
    }

    return driver->error_code;
}

driver_registry_t* driver_get_registry(void) {
    if (!driver_manager_initialized) {
        return NULL;
    }

    return &driver_registry;
}

int driver_registry_save(const char* path) {
    if (!driver_manager_initialized || path == NULL) {
        return -1;
    }

    return 0;
}

int driver_registry_load(const char* path) {
    if (!driver_manager_initialized || path == NULL) {
        return -1;
    }

    return 0;
}

int driver_install_dependencies(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    return 0;
}

int driver_resolve_conflicts(const char* name1, const char* name2) {
    if (!driver_manager_initialized || name1 == NULL || name2 == NULL) {
        return -1;
    }

    return 0;
}

int driver_update_available(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    return 0;
}

int driver_install_update(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    return 0;
}

int driver_rollback(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    return 0;
}

int driver_backup(const char* name) {
    if (!driver_manager_initialized || name == NULL) {
        return -1;
    }

    return 0;
}
