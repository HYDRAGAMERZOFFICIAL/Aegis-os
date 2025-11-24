#ifndef AEGIS_KERNEL_UNIVERSAL_CACHE_H
#define AEGIS_KERNEL_UNIVERSAL_CACHE_H

#include <kernel/types.h>

typedef enum {
    CACHE_TYPE_FILESYSTEM = 1,
    CACHE_TYPE_NETWORK = 2,
    CACHE_TYPE_APPLICATION = 3,
    CACHE_TYPE_SHADER = 4,
    CACHE_TYPE_LIBRARY = 5,
} cache_type_t;

typedef struct {
    cache_type_t type;
    uint64_t total_size_bytes;
    uint64_t used_bytes;
    uint32_t hit_count;
    uint32_t miss_count;
    float hit_ratio;
} cache_stats_t;

int cache_init(void);
int cache_enable_layer(cache_type_t type);
int cache_disable_layer(cache_type_t type);

int cache_get(cache_type_t type, const char *key, void *buffer, uint32_t *size);
int cache_put(cache_type_t type, const char *key, const void *data, uint32_t size);
int cache_invalidate(cache_type_t type, const char *key);
int cache_flush(cache_type_t type);

int cache_set_size_limit(cache_type_t type, uint64_t max_bytes);
int cache_get_stats(cache_type_t type, cache_stats_t *stats);

int cache_enable_compression(cache_type_t type);
int cache_disable_compression(cache_type_t type);

int cache_set_ttl(cache_type_t type, uint64_t ttl_ms);
int cache_enable_persistence(cache_type_t type, const char *storage_path);

int cache_optimize_all_layers(void);
int cache_clear_all(void);

uint64_t cache_get_total_hit_ratio(void);

#endif
