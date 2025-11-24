#include <kernel/universal_cache.h>
#include <string.h>

int cache_init(void) { return 0; }
int cache_enable_layer(cache_type_t type) { return 0; }
int cache_disable_layer(cache_type_t type) { return 0; }
int cache_get(cache_type_t type, const char *key, void *buffer, uint32_t *size) { return 0; }
int cache_put(cache_type_t type, const char *key, const void *data, uint32_t size) { return 0; }
int cache_invalidate(cache_type_t type, const char *key) { return 0; }
int cache_flush(cache_type_t type) { return 0; }
int cache_set_size_limit(cache_type_t type, uint64_t max_bytes) { return 0; }
int cache_get_stats(cache_type_t type, cache_stats_t *stats) { return 0; }
int cache_enable_compression(cache_type_t type) { return 0; }
int cache_disable_compression(cache_type_t type) { return 0; }
int cache_set_ttl(cache_type_t type, uint64_t ttl_ms) { return 0; }
int cache_enable_persistence(cache_type_t type, const char *storage_path) { return 0; }
int cache_optimize_all_layers(void) { return 0; }
int cache_clear_all(void) { return 0; }
uint64_t cache_get_total_hit_ratio(void) { return 0; }
