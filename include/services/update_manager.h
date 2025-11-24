#ifndef AEGIS_UPDATE_MANAGER_H
#define AEGIS_UPDATE_MANAGER_H

#include <kernel/types.h>

typedef enum {
    UPDATE_TYPE_SECURITY,
    UPDATE_TYPE_FEATURE,
    UPDATE_TYPE_BUGFIX,
    UPDATE_TYPE_DRIVER,
    UPDATE_TYPE_KERNEL
} update_type_t;

typedef enum {
    UPDATE_STATE_AVAILABLE,
    UPDATE_STATE_DOWNLOADING,
    UPDATE_STATE_DOWNLOADED,
    UPDATE_STATE_VERIFYING,
    UPDATE_STATE_INSTALLING,
    UPDATE_STATE_COMPLETED,
    UPDATE_STATE_FAILED,
    UPDATE_STATE_ROLLED_BACK
} update_state_t;

typedef struct {
    u64 update_id;
    update_type_t type;
    update_state_t state;
    char *version;
    u64 size;
    u8 signature[256];
    u32 priority;
    bool requires_reboot;
} update_t;

int update_init(void);
update_t **update_check_for_updates(u32 *count);
int update_download(u64 update_id);
int update_verify_signature(update_t *update);
int update_apply_delta(const u8 *delta, u64 size);
int update_install(u64 update_id);
int update_schedule_install(u64 update_id, u64 time);
int update_kpatch_apply(const u8 *patch, u64 size);
int update_rollback(u64 update_id);
int update_get_current_version(char *version_buf, u64 buf_size);
update_state_t update_get_state(u64 update_id);
int update_enable_auto_update(bool enabled);

#endif
