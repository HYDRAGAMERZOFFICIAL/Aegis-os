#ifndef AEGIS_KERNEL_SCHEDULER_H
#define AEGIS_KERNEL_SCHEDULER_H

#include <kernel/types.h>
#include <kernel/process.h>

typedef enum {
    SCHED_CLASS_RT,
    SCHED_CLASS_FAIR,
    SCHED_CLASS_DEADLINE
} sched_class_t;

typedef enum {
    SCHED_MODE_CFS,
    SCHED_MODE_PRIORITY,
    SCHED_MODE_DEADLINE
} sched_mode_t;

typedef struct {
    u64 vruntime;
    u64 sum_exec_runtime;
    u64 prev_sum_exec_runtime;
} cfs_context_t;

typedef struct {
    u64 deadline;
    u64 period;
    u64 runtime;
} deadline_context_t;

typedef struct {
    thread_t *thread;
    sched_class_t sched_class;
    u64 last_scheduled;
    u64 scheduled_count;
    union {
        cfs_context_t cfs;
        deadline_context_t deadline;
    } ctx;
} sched_entity_t;

typedef struct {
    sched_entity_t *entities[MAX_PROCESSES * MAX_THREADS_PER_PROCESS];
    u32 entity_count;
    u64 min_vruntime;
} cfs_rbtree_t;

int scheduler_init(void);
int scheduler_enqueue(sched_entity_t *entity);
int scheduler_dequeue(sched_entity_t *entity);
sched_entity_t *scheduler_pick_next(u32 cpu_id);
void scheduler_tick(u32 cpu_id);
int scheduler_set_class(u64 tid, sched_class_t sched_class);
int scheduler_enable_energy_aware(void);
u64 scheduler_get_cpu_load(u32 cpu_id);
int scheduler_balance_load(void);
void scheduler_switch_context(thread_t *prev, thread_t *next);

#endif
