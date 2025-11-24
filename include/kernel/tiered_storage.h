#ifndef AEGIS_KERNEL_TIERED_STORAGE_H
#define AEGIS_KERNEL_TIERED_STORAGE_H

#include <kernel/types.h>

typedef enum {
    TIER_L1_RAM = 1,
    TIER_L2_SSD = 2,
    TIER_L3_HDD = 3,
    TIER_L4_NETWORK = 4,
} tier_level_t;

typedef struct {
    char device_path[128];
    tier_level_t tier;
    uint64_t total_bytes;
    uint64_t used_bytes;
    uint32_t access_latency_us;
    uint64_t bandwidth_mbps;
} tier_info_t;

typedef struct {
    tier_level_t tier;
    uint64_t accessed_bytes;
    uint32_t access_count;
    uint32_t promotion_count;
    uint32_t demotion_count;
} tier_stats_t;

int tier_init(void);
int tier_enable(void);
int tier_disable(void);

int tier_add_storage(const char *device_path, tier_level_t tier);
int tier_remove_storage(const char *device_path);

int tier_get_info(tier_level_t tier, tier_info_t *info);
int tier_get_stats(tier_level_t tier, tier_stats_t *stats);

int tier_promote_block(uint64_t block_addr, tier_level_t source, tier_level_t dest);
int tier_demote_block(uint64_t block_addr, tier_level_t source, tier_level_t dest);

int tier_set_promotion_threshold(tier_level_t tier, uint32_t access_count);
int tier_set_demotion_threshold(tier_level_t tier, uint64_t age_ms);

int tier_optimize_placement(void);
int tier_enable_predictive_placement(void);
int tier_disable_predictive_placement(void);

float tier_get_avg_latency(void);
uint64_t tier_get_total_bandwidth(void);

#endif
