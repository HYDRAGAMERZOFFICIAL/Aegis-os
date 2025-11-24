#ifndef AEGIS_JOURNALING_H
#define AEGIS_JOURNALING_H

#include <kernel/types.h>

typedef enum {
    JOURNAL_OP_WRITE,
    JOURNAL_OP_DELETE,
    JOURNAL_OP_MKDIR,
    JOURNAL_OP_RMDIR,
    JOURNAL_OP_CHMOD,
    JOURNAL_OP_CHOWN
} journal_op_type_t;

typedef enum {
    JOURNAL_STATE_PENDING,
    JOURNAL_STATE_COMMITTED,
    JOURNAL_STATE_ABORTED,
    JOURNAL_STATE_RECOVERED
} journal_state_t;

typedef struct {
    u64 entry_id;
    u64 timestamp;
    journal_op_type_t op_type;
    const char *path;
    const char *data;
    u64 data_size;
    journal_state_t state;
    bool checksum_valid;
} journal_entry_t;

typedef struct {
    u64 txn_id;
    u64 start_time;
    u64 end_time;
    journal_state_t state;
    journal_entry_t *entries;
    u32 entry_count;
} journal_transaction_t;

int journaling_init(void);
int journaling_start_transaction(void);
int journaling_log_operation(journal_op_type_t op, const char *path, const void *data, u64 size);
int journaling_commit_transaction(void);
int journaling_rollback_transaction(void);
int journaling_recover_after_crash(void);
journal_entry_t **journaling_get_entries(u64 start_time, u64 end_time, u32 *count);
int journaling_verify_journal_integrity(void);
int journaling_compact_journal(void);
int journaling_get_transaction_state(u64 txn_id, journal_state_t *state);
int journaling_enable_checksums(void);
u64 journaling_get_journal_size(void);

#endif
