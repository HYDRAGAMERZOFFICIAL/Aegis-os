#ifndef AEGIS_SYSFS_H
#define AEGIS_SYSFS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SYSFS_MAX_ENTRIES 2048
#define SYSFS_MAX_ENTRY_NAME 256
#define SYSFS_MAX_VALUE_SIZE 65536
#define SYSFS_MAX_OBSERVERS 256

typedef enum {
    SYSFS_TYPE_FILE = 1,
    SYSFS_TYPE_DIR = 2,
    SYSFS_TYPE_LINK = 3,
    SYSFS_TYPE_ATTR = 4
} sysfs_entry_type_t;

typedef struct sysfs_entry sysfs_entry_t;

typedef int (*sysfs_read_fn_t)(sysfs_entry_t *entry, void *buffer, size_t size);
typedef int (*sysfs_write_fn_t)(sysfs_entry_t *entry, const void *buffer, size_t size);
typedef void (*sysfs_notify_fn_t)(sysfs_entry_t *entry, const char *attr, const char *value);

typedef struct {
    sysfs_read_fn_t read;
    sysfs_write_fn_t write;
} sysfs_operations_t;

typedef struct sysfs_entry {
    char name[SYSFS_MAX_ENTRY_NAME];
    sysfs_entry_type_t type;
    sysfs_entry_t *parent;
    sysfs_entry_t **children;
    size_t child_count;
    void *value;
    size_t value_size;
    uint32_t permissions;
    void *private_data;
    sysfs_operations_t *ops;
    sysfs_notify_fn_t observers[SYSFS_MAX_OBSERVERS];
    size_t observer_count;
} sysfs_entry_t;

typedef struct {
    sysfs_entry_t *root;
    sysfs_entry_t **entries;
    size_t entry_count;
} sysfs_t;

sysfs_t *sysfs_init(void);
sysfs_entry_t *sysfs_create_dir(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent);
sysfs_entry_t *sysfs_create_file(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent,
                                  sysfs_operations_t *ops);
sysfs_entry_t *sysfs_create_attr(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent,
                                  const char *value, size_t value_size);
sysfs_entry_t *sysfs_create_link(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent,
                                  sysfs_entry_t *target);

int sysfs_remove_entry(sysfs_t *sysfs, sysfs_entry_t *entry);
sysfs_entry_t *sysfs_find_entry(sysfs_t *sysfs, const char *path);
sysfs_entry_t *sysfs_find_by_name(sysfs_entry_t *parent, const char *name);

int sysfs_read_attr(sysfs_entry_t *entry, void *buffer, size_t size);
int sysfs_write_attr(sysfs_entry_t *entry, const void *buffer, size_t size);
int sysfs_get_attr_string(sysfs_entry_t *entry, char *buffer, size_t size);
int sysfs_set_attr_string(sysfs_entry_t *entry, const char *value);

bool sysfs_update_value(sysfs_entry_t *entry, const void *value, size_t size);
bool sysfs_register_observer(sysfs_entry_t *entry, sysfs_notify_fn_t callback);
bool sysfs_unregister_observer(sysfs_entry_t *entry, sysfs_notify_fn_t callback);
void sysfs_notify_observers(sysfs_entry_t *entry, const char *attr, const char *value);

sysfs_entry_t *sysfs_get_parent(sysfs_entry_t *entry);
sysfs_entry_t **sysfs_get_children(sysfs_entry_t *entry, size_t *count);
size_t sysfs_get_entry_count(sysfs_t *sysfs);

int sysfs_set_permissions(sysfs_entry_t *entry, uint32_t mode);
int sysfs_get_permissions(sysfs_entry_t *entry);

void sysfs_dump_hierarchy(sysfs_t *sysfs);
void sysfs_free(sysfs_t *sysfs);

#endif
