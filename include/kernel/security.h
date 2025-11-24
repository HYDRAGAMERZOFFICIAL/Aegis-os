#ifndef AEGIS_KERNEL_SECURITY_H
#define AEGIS_KERNEL_SECURITY_H

#include <kernel/types.h>

typedef enum {
    SYSCALL_OK,
    SYSCALL_SUSPICIOUS,
    SYSCALL_BLOCKED,
    SYSCALL_ALERT
} syscall_verdict_t;

typedef struct {
    u64 syscall_num;
    u64 arg1, arg2, arg3, arg4, arg5, arg6;
    u64 timestamp;
    u32 cpu_id;
    u64 pid, tid;
} syscall_record_t;

typedef struct {
    u64 address;
    u8 expected_hash[32];
    u8 current_hash[32];
    u64 last_verified;
    bool verified;
} code_region_t;

typedef enum {
    ANOMALY_TYPE_FREQUENCY,
    ANOMALY_TYPE_PATTERN,
    ANOMALY_TYPE_RESOURCE,
    ANOMALY_TYPE_PRIVILEGE,
    ANOMALY_TYPE_NETWORK
} anomaly_type_t;

typedef struct {
    anomaly_type_t type;
    u64 pid;
    u64 severity;
    const char *description;
    u64 timestamp;
} anomaly_t;

typedef struct {
    u64 id;
    u32 call_count;
    u64 *call_sequence;
    u64 seq_len;
} syscall_pattern_t;

int ksim_init(void);
int ksim_verify_kernel_code(void);
int ksim_register_code_region(u64 addr, u64 size, const u8 *hash);
int ksim_verify_code_region(u64 addr);
syscall_verdict_t ksim_monitor_syscall(syscall_record_t *rec);
int ksim_log_syscall(syscall_record_t *rec);
int ksim_analyze_behavior(u64 pid);
anomaly_t *ksim_detect_anomaly(u64 pid);
int ksim_block_syscall(u64 syscall_num, u64 pid);
int ksim_allow_syscall(u64 syscall_num, u64 pid);
int ksim_register_pattern(syscall_pattern_t *pattern);
int ksim_enable_real_time_analysis(void);
u64 ksim_get_integrity_report(void *buffer, u64 max_size);
int ksim_respond_to_anomaly(anomaly_t *anomaly);

#endif
