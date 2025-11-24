#ifndef AEGIS_KERNEL_PROCESS_H
#define AEGIS_KERNEL_PROCESS_H

#include <kernel/types.h>

typedef enum {
    PROCESS_STATE_NEW,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct {
    u64 rax, rbx, rcx, rdx;
    u64 rsi, rdi, rbp, rsp;
    u64 r8, r9, r10, r11;
    u64 r12, r13, r14, r15;
    u64 rip, rflags;
} x86_64_context_t;

typedef struct {
    u32 r0, r1, r2, r3;
    u32 r4, r5, r6, r7;
    u32 r8, r9, r10, r11;
    u32 r12, r13, r14, r15;
    u32 pc, sp;
} arm_context_t;

typedef struct thread_s {
    u64 tid;
    u64 pid;
    process_state_t state;
    u32 priority;
    u32 cpu_affinity;
    u64 time_slice_remaining;
    void *kernel_stack;
    void *user_stack;
    union {
        x86_64_context_t x86_64;
        arm_context_t arm;
    } context;
    struct thread_s *next;
} thread_t;

typedef struct process_s {
    u64 pid;
    u64 parent_pid;
    process_state_t state;
    u32 priority;
    void *page_table;
    void *heap_base;
    u64 heap_size;
    thread_t *threads;
    u32 thread_count;
    u64 cpu_time;
    struct process_s *next;
    struct process_s *prev;
} process_t;

int pmgr_init(void);
process_t *pmgr_create_process(const char *name, void *entry_point, u32 priority);
int pmgr_destroy_process(u64 pid);
thread_t *pmgr_create_thread(u64 pid, void *entry_point, void *arg);
int pmgr_destroy_thread(u64 tid);
int pmgr_set_thread_priority(u64 tid, u32 priority);
int pmgr_set_cpu_affinity(u64 tid, u32 cpu_mask);
process_t *pmgr_get_process(u64 pid);
thread_t *pmgr_get_thread(u64 tid);
int pmgr_schedule_thread(thread_t *thread);
thread_t *pmgr_get_next_runnable_thread(u32 cpu_id);

#endif
