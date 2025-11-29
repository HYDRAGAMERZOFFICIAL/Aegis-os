#include "device_tree.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FDT_MAGIC 0xd00dfeed
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

typedef struct {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
} fdt_header_t;

static inline uint32_t be32_to_cpu(uint32_t value) {
    return ((value & 0xff) << 24) | ((value & 0xff00) << 8) |
           ((value & 0xff0000) >> 8) | ((value >> 24) & 0xff);
}

static inline uint64_t be64_to_cpu(uint64_t value) {
    uint32_t low = be32_to_cpu((uint32_t)(value & 0xffffffff));
    uint32_t high = be32_to_cpu((uint32_t)((value >> 32) & 0xffffffff));
    return ((uint64_t)low << 32) | high;
}

static char *dt_add_string(dt_tree_t *tree, const char *str) {
    if (!tree || !str) return NULL;
    
    size_t len = strlen(str) + 1;
    if (tree->string_offset + len > DT_STRING_BUFFER_SIZE) {
        return NULL;
    }
    
    char *result = &tree->string_buffer[tree->string_offset];
    memcpy(result, str, len);
    tree->string_offset += len;
    return result;
}

static dt_node_t *dt_create_node(dt_tree_t *tree, const char *name) {
    if (!tree || tree->node_count >= DT_MAX_NODES) return NULL;
    
    dt_node_t *node = malloc(sizeof(dt_node_t));
    if (!node) return NULL;
    
    memset(node, 0, sizeof(dt_node_t));
    node->name = dt_add_string(tree, name);
    node->children = malloc(sizeof(dt_node_t *) * 32);
    node->properties = malloc(sizeof(dt_property_t) * 64);
    
    if (!node->children || !node->properties) {
        free(node->children);
        free(node->properties);
        free(node);
        return NULL;
    }
    
    tree->nodes[tree->node_count++] = node;
    return node;
}

dt_tree_t *dt_parse_blob(const void *blob, size_t size) {
    if (!blob || size < sizeof(fdt_header_t)) {
        return NULL;
    }
    
    const fdt_header_t *header = (const fdt_header_t *)blob;
    uint32_t magic = be32_to_cpu(header->magic);
    
    if (magic != FDT_MAGIC) {
        return NULL;
    }
    
    dt_tree_t *tree = malloc(sizeof(dt_tree_t));
    if (!tree) return NULL;
    
    memset(tree, 0, sizeof(dt_tree_t));
    tree->nodes = malloc(sizeof(dt_node_t *) * DT_MAX_NODES);
    tree->string_buffer = malloc(DT_STRING_BUFFER_SIZE);
    tree->phandle_map = malloc(sizeof(uint32_t) * DT_MAX_NODES * 2);
    
    if (!tree->nodes || !tree->string_buffer || !tree->phandle_map) {
        free(tree->nodes);
        free(tree->string_buffer);
        free(tree->phandle_map);
        free(tree);
        return NULL;
    }
    
    uint32_t off_dt_struct = be32_to_cpu(header->off_dt_struct);
    uint32_t off_dt_strings = be32_to_cpu(header->off_dt_strings);
    uint32_t off_mem_rsvmap = be32_to_cpu(header->off_mem_rsvmap);
    uint32_t size_dt_struct = be32_to_cpu(header->size_dt_struct);
    
    const uint8_t *dt_struct = (const uint8_t *)blob + off_dt_struct;
    const char *dt_strings = (const char *)blob + off_dt_strings;
    const uint8_t *mem_rsvmap = (const uint8_t *)blob + off_mem_rsvmap;
    
    uint32_t *phandle_idx = tree->phandle_map;
    dt_node_t *current_node = NULL;
    uint32_t offset = 0;
    
    tree->root = dt_create_node(tree, "");
    current_node = tree->root;
    
    uint32_t phandle_counter = 1;
    
    while (offset < size_dt_struct) {
        uint32_t token = be32_to_cpu(*(uint32_t *)&dt_struct[offset]);
        offset += 4;
        
        switch (token) {
            case FDT_BEGIN_NODE: {
                const char *node_name = (const char *)&dt_struct[offset];
                offset += ALIGN(strlen(node_name) + 1, 4);
                
                dt_node_t *new_node = dt_create_node(tree, node_name);
                if (new_node && current_node) {
                    new_node->parent = current_node;
                    current_node->children[current_node->child_count++] = new_node;
                    current_node = new_node;
                }
                break;
            }
            
            case FDT_END_NODE:
                if (current_node && current_node->parent) {
                    current_node = current_node->parent;
                }
                break;
            
            case FDT_PROP: {
                uint32_t prop_len = be32_to_cpu(*(uint32_t *)&dt_struct[offset]);
                offset += 4;
                uint32_t prop_nameoff = be32_to_cpu(*(uint32_t *)&dt_struct[offset]);
                offset += 4;
                
                const char *prop_name = &dt_strings[prop_nameoff];
                const void *prop_value = &dt_struct[offset];
                offset += ALIGN(prop_len, 4);
                
                if (current_node && current_node->property_count < 64) {
                    dt_property_t *prop = &current_node->properties[current_node->property_count++];
                    prop->name = dt_add_string(tree, prop_name);
                    prop->length = prop_len;
                    
                    void *value_copy = malloc(prop_len);
                    if (value_copy) {
                        memcpy(value_copy, prop_value, prop_len);
                        prop->value = value_copy;
                    }
                    
                    if (strcmp(prop_name, "phandle") == 0 && prop_len == 4) {
                        prop->phandle = be32_to_cpu(*(uint32_t *)prop_value);
                        current_node->phandle = prop->phandle;
                        phandle_idx[tree->phandle_count * 2] = prop->phandle;
                        phandle_idx[tree->phandle_count * 2 + 1] = (uint32_t)(uintptr_t)current_node;
                        tree->phandle_count++;
                    }
                }
                break;
            }
            
            case FDT_NOP:
                break;
            
            case FDT_END:
                goto parse_done;
        }
    }
    
parse_done:
    uint64_t *rsvmap = (uint64_t *)mem_rsvmap;
    for (size_t i = 0; i < DT_MAX_RESERVED_ENTRIES; i++) {
        uint64_t addr = be64_to_cpu(rsvmap[i * 2]);
        uint64_t size = be64_to_cpu(rsvmap[i * 2 + 1]);
        
        if (size == 0) break;
        
        tree->reserved[tree->reserved_count].address = addr;
        tree->reserved[tree->reserved_count].size = size;
        tree->reserved_count++;
    }
    
    return tree;
}

dt_node_t *dt_find_node(dt_tree_t *tree, const char *path) {
    if (!tree || !path || !tree->root) return NULL;
    
    if (strcmp(path, "/") == 0) return tree->root;
    
    dt_node_t *current = tree->root;
    char *path_copy = malloc(strlen(path) + 1);
    if (!path_copy) return NULL;
    strcpy(path_copy, path);
    
    char *saveptr = NULL;
    char *token = strtok_s(path_copy, "/", &saveptr);
    
    while (token && *token) {
        dt_node_t *found = NULL;
        
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

dt_node_t *dt_find_node_by_compatible(dt_tree_t *tree, const char *compatible) {
    if (!tree || !compatible) return NULL;
    
    for (size_t i = 0; i < tree->node_count; i++) {
        dt_node_t *node = tree->nodes[i];
        dt_property_t *prop = dt_get_property(node, "compatible");
        if (prop && prop->value) {
            if (strstr((const char *)prop->value, compatible)) {
                return node;
            }
        }
    }
    
    return NULL;
}

dt_property_t *dt_get_property(dt_node_t *node, const char *name) {
    if (!node || !name) return NULL;
    
    for (size_t i = 0; i < node->property_count; i++) {
        if (strcmp(node->properties[i].name, name) == 0) {
            return &node->properties[i];
        }
    }
    
    return NULL;
}

const char *dt_get_string_property(dt_node_t *node, const char *name) {
    dt_property_t *prop = dt_get_property(node, name);
    return (prop && prop->value) ? (const char *)prop->value : NULL;
}

uint32_t dt_get_u32_property(dt_node_t *node, const char *name) {
    dt_property_t *prop = dt_get_property(node, name);
    if (prop && prop->length >= 4 && prop->value) {
        return be32_to_cpu(*(uint32_t *)prop->value);
    }
    return 0;
}

uint64_t dt_get_u64_property(dt_node_t *node, const char *name) {
    dt_property_t *prop = dt_get_property(node, name);
    if (prop && prop->length >= 8 && prop->value) {
        return be64_to_cpu(*(uint64_t *)prop->value);
    }
    return 0;
}

bool dt_get_bytes_property(dt_node_t *node, const char *name, void *buffer, size_t size) {
    dt_property_t *prop = dt_get_property(node, name);
    if (!prop || !buffer || size > prop->length) return false;
    
    memcpy(buffer, prop->value, size);
    return true;
}

dt_device_matches_t *dt_match_compatible(dt_tree_t *tree, const char *compatible) {
    if (!tree || !compatible) return NULL;
    
    dt_device_matches_t *matches = malloc(sizeof(dt_device_matches_t));
    if (!matches) return NULL;
    
    matches->matches = malloc(sizeof(dt_device_match_t) * DT_MAX_NODES);
    matches->count = 0;
    matches->capacity = DT_MAX_NODES;
    
    if (!matches->matches) {
        free(matches);
        return NULL;
    }
    
    for (size_t i = 0; i < tree->node_count; i++) {
        dt_node_t *node = tree->nodes[i];
        const char *compat = dt_get_string_property(node, "compatible");
        
        if (compat && strstr(compat, compatible)) {
            if (matches->count < matches->capacity) {
                matches->matches[matches->count].name = node->name;
                matches->matches[matches->count].node = node;
                matches->matches[matches->count].phandle = node->phandle;
                matches->count++;
            }
        }
    }
    
    return matches;
}

dt_node_t *dt_find_by_phandle(dt_tree_t *tree, uint32_t phandle) {
    if (!tree) return NULL;
    
    for (size_t i = 0; i < tree->phandle_count; i++) {
        if (tree->phandle_map[i * 2] == phandle) {
            return (dt_node_t *)(uintptr_t)tree->phandle_map[i * 2 + 1];
        }
    }
    
    return NULL;
}

dt_node_t *dt_get_parent(dt_node_t *node) {
    return node ? node->parent : NULL;
}

dt_node_t **dt_get_children(dt_node_t *node, size_t *count) {
    if (!node || !count) return NULL;
    *count = node->child_count;
    return node->children;
}

bool dt_load_overlay(dt_tree_t *tree, const void *overlay_blob, size_t size) {
    if (!tree || !overlay_blob) return false;
    
    dt_tree_t *overlay = dt_parse_blob(overlay_blob, size);
    if (!overlay) return false;
    
    dt_free_tree(overlay);
    return true;
}

bool dt_remove_node(dt_tree_t *tree, dt_node_t *node) {
    if (!tree || !node || !node->parent) return false;
    
    for (size_t i = 0; i < node->parent->child_count; i++) {
        if (node->parent->children[i] == node) {
            memmove(&node->parent->children[i],
                   &node->parent->children[i + 1],
                   sizeof(dt_node_t *) * (node->parent->child_count - i - 1));
            node->parent->child_count--;
            return true;
        }
    }
    
    return false;
}

bool dt_add_property(dt_node_t *node, const char *name, const void *value, size_t length) {
    if (!node || !name || node->property_count >= 64) return false;
    
    dt_property_t *prop = &node->properties[node->property_count++];
    prop->name = (char *)name;
    prop->length = length;
    
    void *value_copy = malloc(length);
    if (!value_copy) {
        node->property_count--;
        return false;
    }
    
    memcpy(value_copy, value, length);
    prop->value = value_copy;
    return true;
}

int dt_validate_tree(dt_tree_t *tree) {
    if (!tree || !tree->root) return -1;
    
    int errors = 0;
    
    for (size_t i = 0; i < tree->node_count; i++) {
        dt_node_t *node = tree->nodes[i];
        
        if (!node->name) errors++;
        
        for (size_t j = 0; j < node->property_count; j++) {
            if (!node->properties[j].name || !node->properties[j].value) {
                errors++;
            }
        }
    }
    
    return errors;
}

void dt_free_tree(dt_tree_t *tree) {
    if (!tree) return;
    
    for (size_t i = 0; i < tree->node_count; i++) {
        dt_node_t *node = tree->nodes[i];
        if (node) {
            for (size_t j = 0; j < node->property_count; j++) {
                free(node->properties[j].value);
            }
            free(node->properties);
            free(node->children);
            free(node);
        }
    }
    
    free(tree->nodes);
    free(tree->string_buffer);
    free(tree->phandle_map);
    free(tree);
}

void dt_free_matches(dt_device_matches_t *matches) {
    if (matches) {
        free(matches->matches);
        free(matches);
    }
}

void dt_print_tree(dt_tree_t *tree) {
    if (!tree) return;
    printf("Device Tree:\n");
    printf("  Nodes: %zu\n", tree->node_count);
    printf("  Phandles: %zu\n", tree->phandle_count);
    printf("  Reserved Regions: %zu\n", tree->reserved_count);
}

void dt_traverse_tree(dt_tree_t *tree, dt_node_callback_t callback, void *context) {
    if (!tree || !callback) return;
    
    for (size_t i = 0; i < tree->node_count; i++) {
        if (callback(tree->nodes[i], context) != 0) {
            break;
        }
    }
}
