#ifndef AEGIS_KERNEL_RAM_COMPRESSION_H
#define AEGIS_KERNEL_RAM_COMPRESSION_H

#include <kernel/types.h>

typedef enum {
    COMPRESSION_LEVEL_LOW = 1,
    COMPRESSION_LEVEL_MEDIUM = 2,
    COMPRESSION_LEVEL_HIGH = 3,
    COMPRESSION_LEVEL_MAX = 4,
} compression_level_t;

typedef struct {
    uint64_t uncompressed_bytes;
    uint64_t compressed_bytes;
    uint32_t pages_compressed;
    float compression_ratio;
    uint64_t compression_time_us;
} ram_compression_stats_t;

int ramcomp_init(void);
int ramcomp_enable(void);
int ramcomp_disable(void);

int ramcomp_set_compression_level(compression_level_t level);
int ramcomp_get_compression_level(compression_level_t *level);

int ramcomp_compress_inactive_pages(void);
int ramcomp_decompress_pages_on_demand(void *address, uint32_t size);

uint64_t ramcomp_get_available_ram(void);
uint64_t ramcomp_get_compressed_bytes(void);

int ramcomp_get_stats(ram_compression_stats_t *stats);
int ramcomp_reset_stats(void);

int ramcomp_set_inactive_threshold(uint64_t milliseconds);
float ramcomp_get_memory_savings_percent(void);

int ramcomp_enable_transparent_compression(void);
int ramcomp_disable_transparent_compression(void);

#endif
