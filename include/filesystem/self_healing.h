#ifndef AEGIS_FILESYSTEM_SELF_HEALING_H
#define AEGIS_FILESYSTEM_SELF_HEALING_H

#include <kernel/types.h>

typedef enum {
    HEAL_CORRUPTION = 1,
    HEAL_LOST_METADATA = 2,
    HEAL_ORPHANED_BLOCKS = 3,
    HEAL_CHECKSUM_MISMATCH = 4,
} healing_type_t;

typedef struct {
    uint64_t block_address;
    healing_type_t corruption_type;
    uint8_t healed;
    uint64_t heal_time_ms;
} healing_record_t;

int selfheal_init(void);
int selfheal_enable(void);
int selfheal_disable(void);

int selfheal_scan_partition(const char *partition, healing_record_t *records, 
                            uint32_t *count, uint32_t max_records);
int selfheal_repair_corruption(uint64_t block_address);
int selfheal_restore_metadata(const char *path);

int selfheal_validate_merkle_tree(const char *path, uint8_t *is_valid);
int selfheal_rebuild_merkle_tree(const char *path);

int selfheal_detect_silent_data_corruption(uint64_t *corrupted_blocks, uint32_t *count, uint32_t max_blocks);
int selfheal_repair_all_corruptions(void);

int selfheal_auto_repair_on_access(uint8_t enabled);
int selfheal_background_scan_interval(uint32_t hours);

int selfheal_get_corruption_stats(char *stats, uint32_t max_len);
int selfheal_get_last_healed_blocks(uint64_t *blocks, uint32_t *count, uint32_t max_blocks);

int selfheal_enable_prediction(void);
int selfheal_predict_failure(const char *path, uint8_t *will_fail);

#endif
