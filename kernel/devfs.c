#include "devfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

devfs_t *devfs_init(void) {
    devfs_t *devfs = malloc(sizeof(devfs_t));
    if (!devfs) return NULL;
    
    memset(devfs, 0, sizeof(devfs_t));
    
    devfs->devices = malloc(sizeof(devfs_device_t *) * DEVFS_MAX_DEVICES);
    devfs->ops = malloc(sizeof(devfs_operations_t *) * DEVFS_MAX_DEVICES);
    
    if (!devfs->devices || !devfs->ops) {
        free(devfs->devices);
        free(devfs->ops);
        free(devfs);
        return NULL;
    }
    
    memset(devfs->devices, 0, sizeof(devfs_device_t *) * DEVFS_MAX_DEVICES);
    memset(devfs->ops, 0, sizeof(devfs_operations_t *) * DEVFS_MAX_DEVICES);
    
    devfs->next_major = 1;
    return devfs;
}

int devfs_register_device(devfs_t *devfs, const char *name, devfs_device_type_t type,
                          devfs_operations_t *ops, void *private_data) {
    if (!devfs || !name || !ops || devfs->device_count >= DEVFS_MAX_DEVICES) {
        return -1;
    }
    
    devfs_device_t *dev = malloc(sizeof(devfs_device_t));
    if (!dev) return -1;
    
    memset(dev, 0, sizeof(devfs_device_t));
    strncpy(dev->name, name, DEVFS_MAX_DEVICE_NAME - 1);
    dev->type = type;
    dev->major = devfs->next_major;
    dev->minor = 0;
    dev->mode = 0644;
    dev->uid = 0;
    dev->gid = 0;
    dev->private_data = private_data;
    dev->registered = true;
    
    devfs_operations_t *ops_copy = malloc(sizeof(devfs_operations_t));
    if (!ops_copy) {
        free(dev);
        return -1;
    }
    
    memcpy(ops_copy, ops, sizeof(devfs_operations_t));
    
    size_t idx = devfs->device_count;
    devfs->devices[idx] = dev;
    devfs->ops[idx] = ops_copy;
    devfs->device_count++;
    
    return dev->major;
}

int devfs_unregister_device(devfs_t *devfs, const char *name) {
    if (!devfs || !name) return -1;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] && strcmp(devfs->devices[i]->name, name) == 0) {
            devfs->devices[i]->registered = false;
            free(devfs->ops[i]);
            devfs->ops[i] = NULL;
            
            memmove(&devfs->devices[i], &devfs->devices[i + 1],
                   sizeof(devfs_device_t *) * (devfs->device_count - i - 1));
            memmove(&devfs->ops[i], &devfs->ops[i + 1],
                   sizeof(devfs_operations_t *) * (devfs->device_count - i - 1));
            
            devfs->device_count--;
            free(devfs->devices[devfs->device_count]);
            return 0;
        }
    }
    
    return -1;
}

int devfs_register_char_device(devfs_t *devfs, const char *name, uint32_t major,
                               uint32_t minor, devfs_operations_t *ops) {
    if (!devfs || !name) return -1;
    
    devfs_device_t *dev = malloc(sizeof(devfs_device_t));
    if (!dev) return -1;
    
    memset(dev, 0, sizeof(devfs_device_t));
    strncpy(dev->name, name, DEVFS_MAX_DEVICE_NAME - 1);
    dev->type = DEVFS_TYPE_CHAR;
    dev->major = major;
    dev->minor = minor;
    dev->mode = 0666;
    dev->uid = 0;
    dev->gid = 0;
    dev->registered = true;
    
    devfs_operations_t *ops_copy = malloc(sizeof(devfs_operations_t));
    if (!ops_copy) {
        free(dev);
        return -1;
    }
    
    if (ops) memcpy(ops_copy, ops, sizeof(devfs_operations_t));
    
    size_t idx = devfs->device_count;
    if (idx >= DEVFS_MAX_DEVICES) {
        free(dev);
        free(ops_copy);
        return -1;
    }
    
    devfs->devices[idx] = dev;
    devfs->ops[idx] = ops_copy;
    devfs->device_count++;
    
    return major;
}

int devfs_register_block_device(devfs_t *devfs, const char *name, uint32_t major,
                                uint32_t minor, uint64_t size, devfs_operations_t *ops) {
    if (!devfs || !name) return -1;
    
    devfs_device_t *dev = malloc(sizeof(devfs_device_t));
    if (!dev) return -1;
    
    memset(dev, 0, sizeof(devfs_device_t));
    strncpy(dev->name, name, DEVFS_MAX_DEVICE_NAME - 1);
    dev->type = DEVFS_TYPE_BLOCK;
    dev->major = major;
    dev->minor = minor;
    dev->size = size;
    dev->mode = 0660;
    dev->uid = 0;
    dev->gid = 6;
    dev->registered = true;
    
    devfs_operations_t *ops_copy = malloc(sizeof(devfs_operations_t));
    if (!ops_copy) {
        free(dev);
        return -1;
    }
    
    if (ops) memcpy(ops_copy, ops, sizeof(devfs_operations_t));
    
    size_t idx = devfs->device_count;
    if (idx >= DEVFS_MAX_DEVICES) {
        free(dev);
        free(ops_copy);
        return -1;
    }
    
    devfs->devices[idx] = dev;
    devfs->ops[idx] = ops_copy;
    devfs->device_count++;
    
    return major;
}

devfs_device_t *devfs_get_device(devfs_t *devfs, const char *name) {
    if (!devfs || !name) return NULL;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] && strcmp(devfs->devices[i]->name, name) == 0) {
            return devfs->devices[i];
        }
    }
    
    return NULL;
}

devfs_device_t *devfs_get_device_by_major_minor(devfs_t *devfs, uint32_t major, uint32_t minor) {
    if (!devfs) return NULL;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] && devfs->devices[i]->major == major &&
            devfs->devices[i]->minor == minor) {
            return devfs->devices[i];
        }
    }
    
    return NULL;
}

devfs_device_t **devfs_get_devices_by_type(devfs_t *devfs, devfs_device_type_t type, size_t *count) {
    if (!devfs || !count) return NULL;
    
    devfs_device_t **result = malloc(sizeof(devfs_device_t *) * devfs->device_count);
    if (!result) {
        *count = 0;
        return NULL;
    }
    
    *count = 0;
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] && devfs->devices[i]->type == type) {
            result[(*count)++] = devfs->devices[i];
        }
    }
    
    return result;
}

int devfs_read(devfs_t *devfs, const char *name, uint64_t offset, void *buffer, size_t size) {
    if (!devfs || !name) return -1;
    
    devfs_device_t *dev = devfs_get_device(devfs, name);
    if (!dev) return -1;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] == dev && devfs->ops[i] && devfs->ops[i]->read) {
            return devfs->ops[i]->read(dev, offset, buffer, size);
        }
    }
    
    return -1;
}

int devfs_write(devfs_t *devfs, const char *name, uint64_t offset, const void *buffer, size_t size) {
    if (!devfs || !name) return -1;
    
    devfs_device_t *dev = devfs_get_device(devfs, name);
    if (!dev) return -1;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] == dev && devfs->ops[i] && devfs->ops[i]->write) {
            return devfs->ops[i]->write(dev, offset, buffer, size);
        }
    }
    
    return -1;
}

int devfs_ioctl(devfs_t *devfs, const char *name, unsigned long cmd, void *arg) {
    if (!devfs || !name) return -1;
    
    devfs_device_t *dev = devfs_get_device(devfs, name);
    if (!dev) return -1;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] == dev && devfs->ops[i] && devfs->ops[i]->ioctl) {
            return devfs->ops[i]->ioctl(dev, cmd, arg);
        }
    }
    
    return -1;
}

int devfs_open(devfs_t *devfs, const char *name) {
    if (!devfs || !name) return -1;
    
    devfs_device_t *dev = devfs_get_device(devfs, name);
    if (!dev) return -1;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] == dev && devfs->ops[i] && devfs->ops[i]->open) {
            return devfs->ops[i]->open(dev);
        }
    }
    
    return 0;
}

int devfs_close(devfs_t *devfs, const char *name) {
    if (!devfs || !name) return -1;
    
    devfs_device_t *dev = devfs_get_device(devfs, name);
    if (!dev) return -1;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        if (devfs->devices[i] == dev && devfs->ops[i] && devfs->ops[i]->close) {
            return devfs->ops[i]->close(dev);
        }
    }
    
    return 0;
}

bool devfs_set_permissions(devfs_device_t *dev, uint32_t mode) {
    if (!dev) return false;
    dev->mode = mode;
    return true;
}

bool devfs_change_owner(devfs_device_t *dev, uint32_t uid, uint32_t gid) {
    if (!dev) return false;
    dev->uid = uid;
    dev->gid = gid;
    return true;
}

bool devfs_enable_hotplug(devfs_device_t *dev) {
    if (!dev) return false;
    dev->hotplug = true;
    return true;
}

bool devfs_disable_hotplug(devfs_device_t *dev) {
    if (!dev) return false;
    dev->hotplug = false;
    return true;
}

int devfs_enumerate_devices(devfs_t *devfs, devfs_device_t **devices, size_t max_count) {
    if (!devfs || !devices) return -1;
    
    size_t count = 0;
    for (size_t i = 0; i < devfs->device_count && count < max_count; i++) {
        if (devfs->devices[i]) {
            devices[count++] = devfs->devices[i];
        }
    }
    
    return count;
}

int devfs_get_device_info(devfs_t *devfs, const char *name, devfs_device_t *info) {
    if (!devfs || !name || !info) return -1;
    
    devfs_device_t *dev = devfs_get_device(devfs, name);
    if (!dev) return -1;
    
    memcpy(info, dev, sizeof(devfs_device_t));
    return 0;
}

void devfs_free(devfs_t *devfs) {
    if (!devfs) return;
    
    for (size_t i = 0; i < devfs->device_count; i++) {
        free(devfs->devices[i]);
        free(devfs->ops[i]);
    }
    
    free(devfs->devices);
    free(devfs->ops);
    free(devfs);
}
