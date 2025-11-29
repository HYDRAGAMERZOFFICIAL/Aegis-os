#ifndef AEGIS_DEVICE_TREE_H
#define AEGIS_DEVICE_TREE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define DT_MAX_NODES 512
#define DT_MAX_PROPERTIES 2048
#define DT_MAX_STRINGS 4096
#define DT_STRING_BUFFER_SIZE 65536
#define DT_MAX_RESERVED_ENTRIES 64
#define DT_MAX_OVERLAYS 32

typedef enum {
    DT_PROP_EMPTY = 0,
    DT_PROP_U32 = 1,
    DT_PROP_U64 = 2,
    DT_PROP_STRING = 3,
    DT_PROP_BYTES = 4,
    DT_PROP_PHANDLE = 5,
    DT_PROP_STRINGLIST = 6
} dt_property_type_t;

typedef struct {
    char *name;
    void *value;
    size_t length;
    dt_property_type_t type;
    uint32_t phandle;
} dt_property_t;

typedef struct dt_node {
    char *name;
    struct dt_node *parent;
    struct dt_node **children;
    size_t child_count;
    dt_property_t *properties;
    size_t property_count;
    uint32_t phandle;
    uint32_t addr;
    uint32_t size;
} dt_node_t;

typedef struct {
    dt_node_t *root;
    dt_node_t **nodes;
    size_t node_count;
    char *string_buffer;
    size_t string_offset;
    uint32_t *phandle_map;
    size_t phandle_count;
    struct {
        uint64_t address;
        uint64_t size;
    } reserved[DT_MAX_RESERVED_ENTRIES];
    size_t reserved_count;
} dt_tree_t;

typedef struct {
    const char *name;
    dt_node_t *node;
    bool compatible_match;
    uint32_t phandle;
} dt_device_match_t;

typedef struct {
    dt_device_match_t *matches;
    size_t count;
    size_t capacity;
} dt_device_matches_t;

dt_tree_t *dt_parse_blob(const void *blob, size_t size);
dt_node_t *dt_find_node(dt_tree_t *tree, const char *path);
dt_node_t *dt_find_node_by_compatible(dt_tree_t *tree, const char *compatible);
dt_property_t *dt_get_property(dt_node_t *node, const char *name);
const char *dt_get_string_property(dt_node_t *node, const char *name);
uint32_t dt_get_u32_property(dt_node_t *node, const char *name);
uint64_t dt_get_u64_property(dt_node_t *node, const char *name);
bool dt_get_bytes_property(dt_node_t *node, const char *name, void *buffer, size_t size);

dt_device_matches_t *dt_match_compatible(dt_tree_t *tree, const char *compatible);
dt_node_t *dt_find_by_phandle(dt_tree_t *tree, uint32_t phandle);
dt_node_t *dt_get_parent(dt_node_t *node);
dt_node_t **dt_get_children(dt_node_t *node, size_t *count);

bool dt_load_overlay(dt_tree_t *tree, const void *overlay_blob, size_t size);
bool dt_remove_node(dt_tree_t *tree, dt_node_t *node);
bool dt_add_property(dt_node_t *node, const char *name, const void *value, size_t length);

int dt_validate_tree(dt_tree_t *tree);
void dt_free_tree(dt_tree_t *tree);
void dt_free_matches(dt_device_matches_t *matches);
void dt_print_tree(dt_tree_t *tree);

typedef int (*dt_node_callback_t)(dt_node_t *node, void *context);
void dt_traverse_tree(dt_tree_t *tree, dt_node_callback_t callback, void *context);

#endif
