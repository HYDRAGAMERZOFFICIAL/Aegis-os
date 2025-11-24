#include <kernel/ram_compression.h>
#include <string.h>

int ramcomp_init(void) { return 0; }
int ramcomp_enable(void) { return 0; }
int ramcomp_disable(void) { return 0; }
int ramcomp_set_compression_level(compression_level_t level) { return 0; }
int ramcomp_get_compression_level(compression_level_t *level) { return 0; }
int ramcomp_compress_inactive_pages(void) { return 0; }
int ramcomp_decompress_pages_on_demand(void *address, uint32_t size) { return 0; }
uint64_t ramcomp_get_available_ram(void) { return 0; }
uint64_t ramcomp_get_compressed_bytes(void) { return 0; }
int ramcomp_get_stats(ram_compression_stats_t *stats) { return 0; }
int ramcomp_reset_stats(void) { return 0; }
int ramcomp_set_inactive_threshold(uint64_t milliseconds) { return 0; }
float ramcomp_get_memory_savings_percent(void) { return 0.0f; }
int ramcomp_enable_transparent_compression(void) { return 0; }
int ramcomp_disable_transparent_compression(void) { return 0; }
