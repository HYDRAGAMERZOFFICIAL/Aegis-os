#include "sysfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

sysfs_t *sysfs_init(void) {
    sysfs_t *sysfs = malloc(sizeof(sysfs_t));
    if (!sysfs) return NULL;
    
    memset(sysfs, 0, sizeof(sysfs_t));
    
    sysfs->entries = malloc(sizeof(sysfs_entry_t *) * SYSFS_MAX_ENTRIES);
    if (!sysfs->entries) {
        free(sysfs);
        return NULL;
    }
    
    memset(sysfs->entries, 0, sizeof(sysfs_entry_t *) * SYSFS_MAX_ENTRIES);
    
    sysfs->root = malloc(sizeof(sysfs_entry_t));
    if (!sysfs->root) {
        free(sysfs->entries);
        free(sysfs);
        return NULL;
    }
    
    memset(sysfs->root, 0, sizeof(sysfs_entry_t));
    strcpy(sysfs->root->name, "/");
    sysfs->root->type = SYSFS_TYPE_DIR;
    sysfs->root->permissions = 0755;
    sysfs->root->children = malloc(sizeof(sysfs_entry_t *) * 256);
    
    if (!sysfs->root->children) {
        free(sysfs->entries);
        free(sysfs->root);
        free(sysfs);
        return NULL;
    }
    
    memset(sysfs->root->children, 0, sizeof(sysfs_entry_t *) * 256);
    sysfs->entries[0] = sysfs->root;
    sysfs->entry_count = 1;
    
    return sysfs;
}

sysfs_entry_t *sysfs_create_dir(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent) {
    if (!sysfs || !name || sysfs->entry_count >= SYSFS_MAX_ENTRIES) return NULL;
    
    if (!parent) parent = sysfs->root;
    
    sysfs_entry_t *entry = malloc(sizeof(sysfs_entry_t));
    if (!entry) return NULL;
    
    memset(entry, 0, sizeof(sysfs_entry_t));
    strncpy(entry->name, name, SYSFS_MAX_ENTRY_NAME - 1);
    entry->type = SYSFS_TYPE_DIR;
    entry->parent = parent;
    entry->permissions = 0755;
    entry->children = malloc(sizeof(sysfs_entry_t *) * 256);
    
    if (!entry->children) {
        free(entry);
        return NULL;
    }
    
    memset(entry->children, 0, sizeof(sysfs_entry_t *) * 256);
    
    if (parent && parent->child_count < 256) {
        parent->children[parent->child_count++] = entry;
    }
    
    sysfs->entries[sysfs->entry_count++] = entry;
    return entry;
}

sysfs_entry_t *sysfs_create_file(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent,
                                  sysfs_operations_t *ops) {
    if (!sysfs || !name || sysfs->entry_count >= SYSFS_MAX_ENTRIES) return NULL;
    
    if (!parent) parent = sysfs->root;
    
    sysfs_entry_t *entry = malloc(sizeof(sysfs_entry_t));
    if (!entry) return NULL;
    
    memset(entry, 0, sizeof(sysfs_entry_t));
    strncpy(entry->name, name, SYSFS_MAX_ENTRY_NAME - 1);
    entry->type = SYSFS_TYPE_FILE;
    entry->parent = parent;
    entry->permissions = 0644;
    
    if (ops) {
        entry->ops = malloc(sizeof(sysfs_operations_t));
        if (!entry->ops) {
            free(entry);
            return NULL;
        }
        memcpy(entry->ops, ops, sizeof(sysfs_operations_t));
    }
    
    if (parent && parent->child_count < 256) {
        parent->children[parent->child_count++] = entry;
    }
    
    sysfs->entries[sysfs->entry_count++] = entry;
    return entry;
}

sysfs_entry_t *sysfs_create_attr(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent,
                                  const char *value, size_t value_size) {
    if (!sysfs || !name || sysfs->entry_count >= SYSFS_MAX_ENTRIES) return NULL;
    
    if (!parent) parent = sysfs->root;
    
    sysfs_entry_t *entry = malloc(sizeof(sysfs_entry_t));
    if (!entry) return NULL;
    
    memset(entry, 0, sizeof(sysfs_entry_t));
    strncpy(entry->name, name, SYSFS_MAX_ENTRY_NAME - 1);
    entry->type = SYSFS_TYPE_ATTR;
    entry->parent = parent;
    entry->permissions = 0644;
    
    if (value && value_size > 0) {
        entry->value = malloc(value_size);
        if (!entry->value) {
            free(entry);
            return NULL;
        }
        memcpy(entry->value, value, value_size);
        entry->value_size = value_size;
    }
    
    if (parent && parent->child_count < 256) {
        parent->children[parent->child_count++] = entry;
    }
    
    sysfs->entries[sysfs->entry_count++] = entry;
    return entry;
}

sysfs_entry_t *sysfs_create_link(sysfs_t *sysfs, const char *name, sysfs_entry_t *parent,
                                  sysfs_entry_t *target) {
    if (!sysfs || !name || !target || sysfs->entry_count >= SYSFS_MAX_ENTRIES) return NULL;
    
    if (!parent) parent = sysfs->root;
    
    sysfs_entry_t *entry = malloc(sizeof(sysfs_entry_t));
    if (!entry) return NULL;
    
    memset(entry, 0, sizeof(sysfs_entry_t));
    strncpy(entry->name, name, SYSFS_MAX_ENTRY_NAME - 1);
    entry->type = SYSFS_TYPE_LINK;
    entry->parent = parent;
    entry->value = target;
    entry->permissions = 0777;
    
    if (parent && parent->child_count < 256) {
        parent->children[parent->child_count++] = entry;
    }
    
    sysfs->entries[sysfs->entry_count++] = entry;
    return entry;
}

int sysfs_remove_entry(sysfs_t *sysfs, sysfs_entry_t *entry) {
    if (!sysfs || !entry) return -1;
    
    if (entry->parent && entry->parent->child_count > 0) {
        for (size_t i = 0; i < entry->parent->child_count; i++) {
            if (entry->parent->children[i] == entry) {
                memmove(&entry->parent->children[i],
                       &entry->parent->children[i + 1],
                       sizeof(sysfs_entry_t *) * (entry->parent->child_count - i - 1));
                entry->parent->child_count--;
                break;
            }
        }
    }
    
    for (size_t i = 0; i < sysfs->entry_count; i++) {
        if (sysfs->entries[i] == entry) {
            memmove(&sysfs->entries[i],
                   &sysfs->entries[i + 1],
                   sizeof(sysfs_entry_t *) * (sysfs->entry_count - i - 1));
            sysfs->entry_count--;
            break;
        }
    }
    
    if (entry->type == SYSFS_TYPE_DIR) free(entry->children);
    if (entry->type == SYSFS_TYPE_FILE && entry->ops) free(entry->ops);
    if (entry->type == SYSFS_TYPE_ATTR && entry->value) free(entry->value);
    
    free(entry);
    return 0;
}

sysfs_entry_t *sysfs_find_entry(sysfs_t *sysfs, const char *path) {
    if (!sysfs || !path) return NULL;
    
    if (strcmp(path, "/") == 0) return sysfs->root;
    
    sysfs_entry_t *current = sysfs->root;
    char *path_copy = malloc(strlen(path) + 1);
    if (!path_copy) return NULL;
    strcpy(path_copy, path);
    
    char *saveptr = NULL;
    char *token = strtok_s(path_copy, "/", &saveptr);
    
    while (token && *token) {
        sysfs_entry_t *found = NULL;
        
        for (size_t i = 0; i < current->child_count; i++) {
            if (strcmp(current->children[i]->name, token) == 0) {
                found = current->children[i];
                break;
            }
        }
        
        if (!found) {
            free(path_copy);
            return NULL;
        }
        
        current = found;
        token = strtok_s(NULL, "/", &saveptr);
    }
    
    free(path_copy);
    return current;
}

sysfs_entry_t *sysfs_find_by_name(sysfs_entry_t *parent, const char *name) {
    if (!parent || !name) return NULL;
    
    for (size_t i = 0; i < parent->child_count; i++) {
        if (strcmp(parent->children[i]->name, name) == 0) {
            return parent->children[i];
        }
    }
    
    return NULL;
}

int sysfs_read_attr(sysfs_entry_t *entry, void *buffer, size_t size) {
    if (!entry || !buffer || size == 0) return -1;
    
    if (entry->type == SYSFS_TYPE_FILE && entry->ops && entry->ops->read) {
        return entry->ops->read(entry, buffer, size);
    }
    
    if (entry->type == SYSFS_TYPE_ATTR && entry->value) {
        size_t copy_size = (size < entry->value_size) ? size : entry->value_size;
        memcpy(buffer, entry->value, copy_size);
        return copy_size;
    }
    
    return -1;
}

int sysfs_write_attr(sysfs_entry_t *entry, const void *buffer, size_t size) {
    if (!entry || !buffer || size == 0) return -1;
    
    if (entry->type == SYSFS_TYPE_FILE && entry->ops && entry->ops->write) {
        return entry->ops->write(entry, buffer, size);
    }
    
    if (entry->type == SYSFS_TYPE_ATTR) {
        void *new_value = malloc(size);
        if (!new_value) return -1;
        
        memcpy(new_value, buffer, size);
        
        if (entry->value) free(entry->value);
        entry->value = new_value;
        entry->value_size = size;
        
        return size;
    }
    
    return -1;
}

int sysfs_get_attr_string(sysfs_entry_t *entry, char *buffer, size_t size) {
    if (!entry || !buffer || size == 0) return -1;
    
    if (entry->type == SYSFS_TYPE_ATTR && entry->value) {
        size_t copy_size = (size - 1 < entry->value_size) ? (size - 1) : entry->value_size;
        memcpy(buffer, entry->value, copy_size);
        buffer[copy_size] = '\0';
        return copy_size;
    }
    
    return -1;
}

int sysfs_set_attr_string(sysfs_entry_t *entry, const char *value) {
    if (!entry || !value) return -1;
    
    return sysfs_write_attr(entry, value, strlen(value) + 1);
}

bool sysfs_update_value(sysfs_entry_t *entry, const void *value, size_t size) {
    if (!entry || !value) return false;
    
    sysfs_write_attr(entry, value, size);
    sysfs_notify_observers(entry, entry->name, (const char *)value);
    
    return true;
}

bool sysfs_register_observer(sysfs_entry_t *entry, sysfs_notify_fn_t callback) {
    if (!entry || !callback || entry->observer_count >= SYSFS_MAX_OBSERVERS) return false;
    
    entry->observers[entry->observer_count++] = callback;
    return true;
}

bool sysfs_unregister_observer(sysfs_entry_t *entry, sysfs_notify_fn_t callback) {
    if (!entry || !callback) return false;
    
    for (size_t i = 0; i < entry->observer_count; i++) {
        if (entry->observers[i] == callback) {
            memmove(&entry->observers[i],
                   &entry->observers[i + 1],
                   sizeof(sysfs_notify_fn_t) * (entry->observer_count - i - 1));
            entry->observer_count--;
            return true;
        }
    }
    
    return false;
}

void sysfs_notify_observers(sysfs_entry_t *entry, const char *attr, const char *value) {
    if (!entry) return;
    
    for (size_t i = 0; i < entry->observer_count; i++) {
        if (entry->observers[i]) {
            entry->observers[i](entry, (char *)attr, (char *)value);
        }
    }
}

sysfs_entry_t *sysfs_get_parent(sysfs_entry_t *entry) {
    return entry ? entry->parent : NULL;
}

sysfs_entry_t **sysfs_get_children(sysfs_entry_t *entry, size_t *count) {
    if (!entry || !count) return NULL;
    *count = entry->child_count;
    return entry->children;
}

size_t sysfs_get_entry_count(sysfs_t *sysfs) {
    return sysfs ? sysfs->entry_count : 0;
}

int sysfs_set_permissions(sysfs_entry_t *entry, uint32_t mode) {
    if (!entry) return -1;
    entry->permissions = mode;
    return 0;
}

int sysfs_get_permissions(sysfs_entry_t *entry) {
    return entry ? entry->permissions : -1;
}

static void sysfs_dump_recursive(sysfs_entry_t *entry, int depth) {
    if (!entry) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s (%s)\n", entry->name,
           entry->type == SYSFS_TYPE_DIR ? "dir" :
           entry->type == SYSFS_TYPE_FILE ? "file" :
           entry->type == SYSFS_TYPE_ATTR ? "attr" : "link");
    
    for (size_t i = 0; i < entry->child_count; i++) {
        sysfs_dump_recursive(entry->children[i], depth + 1);
    }
}

void sysfs_dump_hierarchy(sysfs_t *sysfs) {
    if (!sysfs || !sysfs->root) return;
    printf("sysfs hierarchy:\n");
    sysfs_dump_recursive(sysfs->root, 0);
}

void sysfs_free(sysfs_t *sysfs) {
    if (!sysfs) return;
    
    for (size_t i = 0; i < sysfs->entry_count; i++) {
        sysfs_entry_t *entry = sysfs->entries[i];
        if (entry) {
            if (entry->type == SYSFS_TYPE_DIR) free(entry->children);
            if (entry->type == SYSFS_TYPE_FILE && entry->ops) free(entry->ops);
            if (entry->type == SYSFS_TYPE_ATTR && entry->value) free(entry->value);
            free(entry);
        }
    }
    
    free(sysfs->entries);
    free(sysfs);
}
