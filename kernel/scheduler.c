#include <kernel/scheduler.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    sched_entity_t **entities;
    u32 entity_count;
    u64 min_vruntime;
    u64 total_weight;
} scheduler_state_t;

static scheduler_state_t sched_state = {0};
static u64 cpu_loads[MAX_CPUS] = {0};

int scheduler_init(void)
{
    sched_state.entities = (sched_entity_t **)malloc(MAX_PROCESSES * MAX_THREADS_PER_PROCESS * sizeof(sched_entity_t *));
    if (!sched_state.entities) return -1;

    sched_state.entity_count = 0;
    sched_state.min_vruntime = 0;
    sched_state.total_weight = 0;

    for (u32 i = 0; i < MAX_CPUS; i++) {
        cpu_loads[i] = 0;
    }

    return 0;
}

int scheduler_enqueue(sched_entity_t *entity)
{
    if (!entity) return -1;
    if (sched_state.entity_count >= MAX_PROCESSES * MAX_THREADS_PER_PROCESS) {
        return -1;
    }

    sched_state.entities[sched_state.entity_count++] = entity;

    if (entity->sched_class == SCHED_CLASS_FAIR) {
        sched_state.total_weight += 1024;
        if (entity->ctx.cfs.vruntime < sched_state.min_vruntime) {
            sched_state.min_vruntime = entity->ctx.cfs.vruntime;
        }
    }

    return 0;
}

int scheduler_dequeue(sched_entity_t *entity)
{
    if (!entity) return -1;

    for (u32 i = 0; i < sched_state.entity_count; i++) {
        if (sched_state.entities[i] == entity) {
            for (u32 j = i; j < sched_state.entity_count - 1; j++) {
                sched_state.entities[j] = sched_state.entities[j + 1];
            }
            sched_state.entity_count--;

            if (entity->sched_class == SCHED_CLASS_FAIR) {
                sched_state.total_weight -= 1024;
            }

            return 0;
        }
    }

    return -1;
}

sched_entity_t *scheduler_pick_next(u32 cpu_id)
{
    sched_entity_t *next = NULL;
    u64 min_vruntime_ent = -1;

    for (u32 i = 0; i < sched_state.entity_count; i++) {
        sched_entity_t *entity = sched_state.entities[i];

        if (entity->sched_class == SCHED_CLASS_RT && entity->thread->state == PROCESS_STATE_READY) {
            return entity;
        }

        if (entity->sched_class == SCHED_CLASS_FAIR && entity->thread->state == PROCESS_STATE_READY) {
            if (entity->ctx.cfs.vruntime < min_vruntime_ent) {
                min_vruntime_ent = entity->ctx.cfs.vruntime;
                next = entity;
            }
        }
    }

    return next;
}

void scheduler_tick(u32 cpu_id)
{
    if (cpu_id >= MAX_CPUS) return;

    for (u32 i = 0; i < sched_state.entity_count; i++) {
        sched_entity_t *entity = sched_state.entities[i];
        if (entity->thread->state == PROCESS_STATE_RUNNING) {
            entity->thread->time_slice_remaining--;
            entity->ctx.cfs.sum_exec_runtime++;

            if (entity->thread->time_slice_remaining == 0) {
                entity->thread->state = PROCESS_STATE_READY;
                entity->thread->time_slice_remaining = 10;
            }

            if (entity->sched_class == SCHED_CLASS_DEADLINE) {
                if (entity->ctx.deadline.runtime > 0) {
                    entity->ctx.deadline.runtime--;
                }
            }
        }
    }
}

int scheduler_set_class(u64 tid, sched_class_t sched_class)
{
    for (u32 i = 0; i < sched_state.entity_count; i++) {
        if (sched_state.entities[i]->thread->tid == tid) {
            sched_state.entities[i]->sched_class = sched_class;
            return 0;
        }
    }

    return -1;
}

int scheduler_enable_energy_aware(void)
{
    return 0;
}

u64 scheduler_get_cpu_load(u32 cpu_id)
{
    if (cpu_id >= MAX_CPUS) return 0;
    return cpu_loads[cpu_id];
}

int scheduler_balance_load(void)
{
    u64 avg_load = 0;
    u32 overloaded_cpus[MAX_CPUS];
    u32 underloaded_cpus[MAX_CPUS];
    u32 over_count = 0, under_count = 0;

    for (u32 i = 0; i < MAX_CPUS; i++) {
        avg_load += cpu_loads[i];
    }
    avg_load /= MAX_CPUS;

    for (u32 i = 0; i < MAX_CPUS; i++) {
        if (cpu_loads[i] > avg_load + 2) {
            overloaded_cpus[over_count++] = i;
        } else if (cpu_loads[i] < avg_load - 2) {
            underloaded_cpus[under_count++] = i;
        }
    }

    return 0;
}

void scheduler_switch_context(thread_t *prev, thread_t *next)
{
    if (!prev || !next) return;

    prev->state = PROCESS_STATE_READY;
    next->state = PROCESS_STATE_RUNNING;
}
