#ifndef AEGIS_SYSCALL_GATEKEEPER_H
#define AEGIS_SYSCALL_GATEKEEPER_H

#include <kernel/types.h>

typedef enum {
    SYSCALL_ALLOW,
    SYSCALL_DENY,
    SYSCALL_LOG,
    SYSCALL_SANDBOX
} syscall_action_t;

typedef enum {
    THREAT_ROP_CHAIN,
    THREAT_STACK_PIVOT,
    THREAT_JOP_GADGET,
    THREAT_HEAP_SPRAY,
    THREAT_PRIVILEGE_ESCALATION
} threat_type_t;

typedef struct {
    u64 syscall_num;
    u64 arg1, arg2, arg3, arg4, arg5, arg6;
    u64 rip;
    u64 rsp;
} syscall_context_t;

typedef struct {
    u64 syscall_num;
    u64 process_id;
    syscall_action_t action;
    bool log_args;
    u64 max_call_freq;
} syscall_policy_t;

typedef struct {
    u64 threat_id;
    u64 pid;
    threat_type_t threat_type;
    u64 detected_at;
    const char *description;
} threat_detection_t;

int gatekeeper_init(void);
int gatekeeper_register_policy(syscall_policy_t *policy);
int gatekeeper_check_syscall(syscall_context_t *ctx);
int gatekeeper_filter_syscall(u64 syscall_num, u64 process_id);
int gatekeeper_detect_rop_chain(u64 rsp, u64 *gadget_addrs, u32 *count);
int gatekeeper_detect_stack_pivot(syscall_context_t *ctx);
int gatekeeper_detect_jop_gadget(u64 rip);
int gatekeeper_detect_heap_spray(u64 pid);
int gatekeeper_prevent_privilege_escalation(u64 pid);
threat_detection_t **gatekeeper_get_detected_threats(u32 *count);
int gatekeeper_enable_rop_protection(void);
int gatekeeper_enable_cfi(void);
int gatekeeper_log_syscall_trace(u64 pid);
int gatekeeper_block_suspicious_syscall(u64 syscall_num);

#endif
