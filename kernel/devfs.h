#ifndef AEGIS_DEVFS_H
#define AEGIS_DEVFS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define DEVFS_MAX_DEVICES 1024
#define DEVFS_MAX_DEVICE_NAME 256
#define DEVFS_DEVICE_BUFFER_SIZE 65536

typedef enum {
    DEVFS_TYPE_CHAR = 1,
    DEVFS_TYPE_BLOCK = 2,
    DEVFS_TYPE_MISC = 3,
    DEVFS_TYPE_TTY = 4,
    DEVFS_TYPE_NET = 5,
    DEVFS_TYPE_USB = 6,
    DEVFS_TYPE_SENSOR = 7
} devfs_device_type_t;

typedef struct devfs_device {
    char name[DEVFS_MAX_DEVICE_NAME];
    devfs_device_type_t type;
    uint32_t major;
    uint32_t minor;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint64_t size;
    void *private_data;
    bool hotplug;
    bool registered;
} devfs_device_t;

typedef int (*devfs_read_fn_t)(devfs_device_t *dev, uint64_t offset, void *buffer, size_t size);
typedef int (*devfs_write_fn_t)(devfs_device_t *dev, uint64_t offset, const void *buffer, size_t size);
typedef int (*devfs_ioctl_fn_t)(devfs_device_t *dev, unsigned long cmd, void *arg);
typedef int (*devfs_open_fn_t)(devfs_device_t *dev);
typedef int (*devfs_close_fn_t)(devfs_device_t *dev);
typedef int (*devfs_poll_fn_t)(devfs_device_t *dev, unsigned int events);

typedef struct {
    devfs_read_fn_t read;
    devfs_write_fn_t write;
    devfs_ioctl_fn_t ioctl;
    devfs_open_fn_t open;
    devfs_close_fn_t close;
    devfs_poll_fn_t poll;
} devfs_operations_t;

typedef struct {
    devfs_device_t **devices;
    devfs_operations_t **ops;
    size_t device_count;
    uint32_t next_major;
} devfs_t;

devfs_t *devfs_init(void);
int devfs_register_device(devfs_t *devfs, const char *name, devfs_device_type_t type,
                          devfs_operations_t *ops, void *private_data);
int devfs_unregister_device(devfs_t *devfs, const char *name);
int devfs_register_char_device(devfs_t *devfs, const char *name, uint32_t major,
                               uint32_t minor, devfs_operations_t *ops);
int devfs_register_block_device(devfs_t *devfs, const char *name, uint32_t major,
                                uint32_t minor, uint64_t size, devfs_operations_t *ops);

devfs_device_t *devfs_get_device(devfs_t *devfs, const char *name);
devfs_device_t *devfs_get_device_by_major_minor(devfs_t *devfs, uint32_t major, uint32_t minor);
devfs_device_t **devfs_get_devices_by_type(devfs_t *devfs, devfs_device_type_t type, size_t *count);

int devfs_read(devfs_t *devfs, const char *name, uint64_t offset, void *buffer, size_t size);
int devfs_write(devfs_t *devfs, const char *name, uint64_t offset, const void *buffer, size_t size);
int devfs_ioctl(devfs_t *devfs, const char *name, unsigned long cmd, void *arg);
int devfs_open(devfs_t *devfs, const char *name);
int devfs_close(devfs_t *devfs, const char *name);

bool devfs_set_permissions(devfs_device_t *dev, uint32_t mode);
bool devfs_change_owner(devfs_device_t *dev, uint32_t uid, uint32_t gid);
bool devfs_enable_hotplug(devfs_device_t *dev);
bool devfs_disable_hotplug(devfs_device_t *dev);

int devfs_enumerate_devices(devfs_t *devfs, devfs_device_t **devices, size_t max_count);
int devfs_get_device_info(devfs_t *devfs, const char *name, devfs_device_t *info);

void devfs_free(devfs_t *devfs);

#endif
