#include <kernel/process.h>
#include <kernel/memory.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    process_t *processes[MAX_PROCESSES];
    u32 process_count;
    u64 next_pid;
    u64 next_tid;
    thread_t *run_queues[MAX_CPUS];
    uint lock;
} pmgr_state_t;

static pmgr_state_t pmgr_state = {0};

int pmgr_init(void)
{
    for (int i = 0; i < MAX_CPUS; i++) {
        pmgr_state.run_queues[i] = NULL;
    }
    pmgr_state.next_pid = 1;
    pmgr_state.next_tid = 1;
    pmgr_state.process_count = 0;
    pmgr_state.lock = 0;
    return 0;
}

process_t *pmgr_create_process(const char *name, void *entry_point, u32 priority)
{
    process_t *proc = (process_t *)malloc(sizeof(process_t));
    if (!proc) return NULL;

    proc->pid = pmgr_state.next_pid++;
    proc->parent_pid = 0;
    proc->state = PROCESS_STATE_NEW;
    proc->priority = priority;
    proc->page_table = mmgr_create_address_space();
    proc->heap_base = NULL;
    proc->heap_size = 0;
    proc->threads = NULL;
    proc->thread_count = 0;
    proc->cpu_time = 0;
    proc->next = NULL;
    proc->prev = NULL;

    if (pmgr_state.process_count < MAX_PROCESSES) {
        pmgr_state.processes[pmgr_state.process_count++] = proc;
    }

    return proc;
}

int pmgr_destroy_process(u64 pid)
{
    process_t *proc = pmgr_get_process(pid);
    if (!proc) return -1;

    while (proc->threads) {
        thread_t *thread = proc->threads;
        proc->threads = thread->next;
        free(thread->kernel_stack);
        free(thread->user_stack);
        free(thread);
    }

    if (proc->page_table) {
        mmgr_destroy_address_space((address_space_t *)proc->page_table);
    }

    for (u32 i = 0; i < pmgr_state.process_count; i++) {
        if (pmgr_state.processes[i]->pid == pid) {
            for (u32 j = i; j < pmgr_state.process_count - 1; j++) {
                pmgr_state.processes[j] = pmgr_state.processes[j + 1];
            }
            pmgr_state.process_count--;
            break;
        }
    }

    free(proc);
    return 0;
}

thread_t *pmgr_create_thread(u64 pid, void *entry_point, void *arg)
{
    process_t *proc = pmgr_get_process(pid);
    if (!proc) return NULL;

    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
    if (!thread) return NULL;

    thread->tid = pmgr_state.next_tid++;
    thread->pid = pid;
    thread->state = PROCESS_STATE_NEW;
    thread->priority = proc->priority;
    thread->cpu_affinity = 0;
    thread->time_slice_remaining = 10;
    thread->kernel_stack = mmgr_alloc_pages(2);
    thread->user_stack = mmgr_alloc_pages(4);
    thread->next = NULL;

    if (proc->thread_count < MAX_THREADS_PER_PROCESS) {
        thread->next = proc->threads;
        proc->threads = thread;
        proc->thread_count++;
    }

    return thread;
}

int pmgr_destroy_thread(u64 tid)
{
    for (u32 i = 0; i < pmgr_state.process_count; i++) {
        process_t *proc = pmgr_state.processes[i];
        thread_t *thread = proc->threads;
        thread_t *prev = NULL;

        while (thread) {
            if (thread->tid == tid) {
                if (prev) {
                    prev->next = thread->next;
                } else {
                    proc->threads = thread->next;
                }
                free(thread->kernel_stack);
                free(thread->user_stack);
                free(thread);
                proc->thread_count--;
                return 0;
            }
            prev = thread;
            thread = thread->next;
        }
    }
    return -1;
}

int pmgr_set_thread_priority(u64 tid, u32 priority)
{
    thread_t *thread = pmgr_get_thread(tid);
    if (!thread) return -1;

    thread->priority = priority;
    return 0;
}

int pmgr_set_cpu_affinity(u64 tid, u32 cpu_mask)
{
    thread_t *thread = pmgr_get_thread(tid);
    if (!thread) return -1;

    thread->cpu_affinity = cpu_mask;
    return 0;
}

process_t *pmgr_get_process(u64 pid)
{
    for (u32 i = 0; i < pmgr_state.process_count; i++) {
        if (pmgr_state.processes[i]->pid == pid) {
            return pmgr_state.processes[i];
        }
    }
    return NULL;
}

thread_t *pmgr_get_thread(u64 tid)
{
    for (u32 i = 0; i < pmgr_state.process_count; i++) {
        process_t *proc = pmgr_state.processes[i];
        thread_t *thread = proc->threads;
        while (thread) {
            if (thread->tid == tid) {
                return thread;
            }
            thread = thread->next;
        }
    }
    return NULL;
}

int pmgr_schedule_thread(thread_t *thread)
{
    if (!thread) return -1;

    thread->state = PROCESS_STATE_READY;
    u32 cpu = thread->cpu_affinity & 0xFF;
    if (cpu >= MAX_CPUS) cpu = 0;

    thread->next = pmgr_state.run_queues[cpu];
    pmgr_state.run_queues[cpu] = thread;
    return 0;
}

thread_t *pmgr_get_next_runnable_thread(u32 cpu_id)
{
    if (cpu_id >= MAX_CPUS) return NULL;

    thread_t *thread = pmgr_state.run_queues[cpu_id];
    if (thread) {
        pmgr_state.run_queues[cpu_id] = thread->next;
        thread->next = NULL;
    }
    return thread;
}
