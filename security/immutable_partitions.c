#include <security/immutable_partitions.h>
#include <string.h>

int immut_init(void) { return 0; }
int immut_lock_partition(immut_partition_type_t partition) { return 0; }
int immut_unlock_partition(immut_partition_type_t partition) { return 0; }
int immut_get_partition_info(immut_partition_type_t partition, immut_partition_info_t *info) { return 0; }
int immut_verify_partition(immut_partition_type_t partition, uint8_t *expected_hash) { return 0; }
int immut_create_snapshot(immut_partition_type_t partition, const char *snapshot_name) { return 0; }
int immut_rollback_snapshot(immut_partition_type_t partition, const char *snapshot_name) { return 0; }
int immut_detect_tampering(immut_partition_type_t partition) { return 0; }
int immut_auto_repair(immut_partition_type_t partition) { return 0; }
