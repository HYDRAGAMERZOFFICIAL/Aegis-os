#include <kernel/tiered_storage.h>
#include <string.h>

int tier_init(void) { return 0; }
int tier_enable(void) { return 0; }
int tier_disable(void) { return 0; }
int tier_add_storage(const char *device_path, tier_level_t tier) { return 0; }
int tier_remove_storage(const char *device_path) { return 0; }
int tier_get_info(tier_level_t tier, tier_info_t *info) { return 0; }
int tier_get_stats(tier_level_t tier, tier_stats_t *stats) { return 0; }
int tier_promote_block(uint64_t block_addr, tier_level_t source, tier_level_t dest) { return 0; }
int tier_demote_block(uint64_t block_addr, tier_level_t source, tier_level_t dest) { return 0; }
int tier_set_promotion_threshold(tier_level_t tier, uint32_t access_count) { return 0; }
int tier_set_demotion_threshold(tier_level_t tier, uint64_t age_ms) { return 0; }
int tier_optimize_placement(void) { return 0; }
int tier_enable_predictive_placement(void) { return 0; }
int tier_disable_predictive_placement(void) { return 0; }
float tier_get_avg_latency(void) { return 0.0f; }
uint64_t tier_get_total_bandwidth(void) { return 0; }
