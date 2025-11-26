#include <kernel/debugger.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    breakpoint_t *breakpoints;
    u32 breakpoint_count;
    watchpoint_t *watchpoints;
    u32 watchpoint_count;
    cpu_registers_t registers;
    stack_frame_t *call_stack;
    u32 stack_depth;
    u32 max_breakpoints;
    u32 max_watchpoints;
    bool single_step_enabled;
    u64 current_thread_id;
    bool enabled;
    const char *log_file;
    bool logging_enabled;
} kdbg_internal_state_t;

static kdbg_internal_state_t kdbg_state = {0};

int debugger_init(void)
{
    kdbg_state.max_breakpoints = 256;
    kdbg_state.max_watchpoints = 64;
    kdbg_state.breakpoint_count = 0;
    kdbg_state.watchpoint_count = 0;
    kdbg_state.stack_depth = 0;
    kdbg_state.enabled = 0;
    kdbg_state.logging_enabled = 0;

    kdbg_state.breakpoints = (breakpoint_t *)calloc(kdbg_state.max_breakpoints, sizeof(breakpoint_t));
    if (!kdbg_state.breakpoints) return -1;

    kdbg_state.watchpoints = (watchpoint_t *)calloc(kdbg_state.max_watchpoints, sizeof(watchpoint_t));
    if (!kdbg_state.watchpoints) return -1;

    kdbg_state.call_stack = (stack_frame_t *)calloc(64, sizeof(stack_frame_t));
    if (!kdbg_state.call_stack) return -1;

    memset(&kdbg_state.registers, 0, sizeof(cpu_registers_t));

    return 0;
}

int debugger_enable(void)
{
    kdbg_state.enabled = 1;
    return 0;
}

int debugger_disable(void)
{
    kdbg_state.enabled = 0;
    return 0;
}

int debugger_is_enabled(void)
{
    return kdbg_state.enabled ? 1 : 0;
}

debugger_state_t debugger_get_state(void)
{
    debugger_state_t state = {0};
    state.state = (debugger_exec_state_t)(kdbg_state.enabled ? DBG_STATE_RUNNING : DBG_STATE_DISABLED);
    state.breakpoint_count = kdbg_state.breakpoint_count;
    state.watchpoint_count = kdbg_state.watchpoint_count;
    state.stack_depth = kdbg_state.stack_depth;

    return state;
}

int debugger_set_breakpoint(u64 address, breakpoint_type_t type)
{
    if (kdbg_state.breakpoint_count >= kdbg_state.max_breakpoints) return -1;

    breakpoint_t *bp = &kdbg_state.breakpoints[kdbg_state.breakpoint_count];
    bp->address = address;
    bp->type = type;
    bp->enabled = 1;
    bp->hit_count = 0;
    bp->condition = 0;

    kdbg_state.breakpoint_count++;

    if (kdbg_state.logging_enabled) {
        printf("[DEBUGGER] Breakpoint set at 0x%llx (type: %u)\n", address, type);
    }

    return 0;
}

int debugger_remove_breakpoint(u64 address)
{
    for (u32 i = 0; i < kdbg_state.breakpoint_count; i++) {
        if (kdbg_state.breakpoints[i].address == address) {
            kdbg_state.breakpoints[i] = kdbg_state.breakpoints[kdbg_state.breakpoint_count - 1];
            kdbg_state.breakpoint_count--;
            return 0;
        }
    }

    return -1;
}

int debugger_enable_breakpoint(u64 address)
{
    for (u32 i = 0; i < kdbg_state.breakpoint_count; i++) {
        if (kdbg_state.breakpoints[i].address == address) {
            kdbg_state.breakpoints[i].enabled = 1;
            return 0;
        }
    }

    return -1;
}

int debugger_disable_breakpoint(u64 address)
{
    for (u32 i = 0; i < kdbg_state.breakpoint_count; i++) {
        if (kdbg_state.breakpoints[i].address == address) {
            kdbg_state.breakpoints[i].enabled = 0;
            return 0;
        }
    }

    return -1;
}

int debugger_set_conditional_breakpoint(u64 address, u64 condition)
{
    for (u32 i = 0; i < kdbg_state.breakpoint_count; i++) {
        if (kdbg_state.breakpoints[i].address == address) {
            kdbg_state.breakpoints[i].condition = condition;
            return 0;
        }
    }

    return -1;
}

breakpoint_t *debugger_get_breakpoints(u32 *count)
{
    if (!count) return NULL;

    breakpoint_t *result = (breakpoint_t *)calloc(kdbg_state.breakpoint_count, sizeof(breakpoint_t));
    if (!result) return NULL;

    for (u32 i = 0; i < kdbg_state.breakpoint_count; i++) {
        result[i] = kdbg_state.breakpoints[i];
    }

    *count = kdbg_state.breakpoint_count;

    return result;
}

int debugger_set_watchpoint(u64 address, u32 size, const char *name)
{
    if (!name || kdbg_state.watchpoint_count >= kdbg_state.max_watchpoints) return -1;

    watchpoint_t *wp = &kdbg_state.watchpoints[kdbg_state.watchpoint_count];
    wp->address = address;
    wp->size = size;
    wp->name = name;
    wp->index = kdbg_state.watchpoint_count;

    kdbg_state.watchpoint_count++;

    if (kdbg_state.logging_enabled) {
        printf("[DEBUGGER] Watchpoint set at 0x%llx size %u (%s)\n", address, size, name);
    }

    return 0;
}

int debugger_remove_watchpoint(u64 address)
{
    for (u32 i = 0; i < kdbg_state.watchpoint_count; i++) {
        if (kdbg_state.watchpoints[i].address == address) {
            kdbg_state.watchpoints[i] = kdbg_state.watchpoints[kdbg_state.watchpoint_count - 1];
            kdbg_state.watchpoint_count--;
            return 0;
        }
    }

    return -1;
}

watchpoint_t *debugger_get_watchpoints(u32 *count)
{
    if (!count) return NULL;

    watchpoint_t *result = (watchpoint_t *)calloc(kdbg_state.watchpoint_count, sizeof(watchpoint_t));
    if (!result) return NULL;

    for (u32 i = 0; i < kdbg_state.watchpoint_count; i++) {
        result[i] = kdbg_state.watchpoints[i];
    }

    *count = kdbg_state.watchpoint_count;

    return result;
}

int debugger_single_step(void)
{
    kdbg_state.single_step_enabled = 1;
    return 0;
}

int debugger_continue(void)
{
    kdbg_state.single_step_enabled = 0;
    return 0;
}

int debugger_step_over(void)
{
    return 0;
}

int debugger_step_into(void)
{
    return 0;
}

int debugger_step_out(void)
{
    return 0;
}

int debugger_read_memory(u64 address, u8 *buffer, u32 size)
{
    if (!buffer || size == 0) return -1;

    memset(buffer, 0, size);

    return 0;
}

int debugger_write_memory(u64 address, const u8 *buffer, u32 size)
{
    if (!buffer || size == 0) return -1;

    return 0;
}

cpu_registers_t *debugger_get_registers(void)
{
    return &kdbg_state.registers;
}

int debugger_set_register(const char *reg_name, u64 value)
{
    if (!reg_name) return -1;

    return 0;
}

stack_frame_t *debugger_get_call_stack(u32 *depth)
{
    if (!depth) return NULL;

    *depth = kdbg_state.stack_depth;

    return kdbg_state.call_stack;
}

int debugger_print_stack_trace(void)
{
    printf("Call Stack Trace:\n");

    for (u32 i = 0; i < kdbg_state.stack_depth; i++) {
        stack_frame_t *frame = &kdbg_state.call_stack[i];
        printf("  [%u] 0x%llx %s (%s:%u)\n", i, frame->address, 
               frame->function_name ? frame->function_name : "unknown",
               frame->filename ? frame->filename : "unknown",
               frame->line_number);
    }

    return 0;
}

int debugger_set_symbol_table(const char *symbol_file)
{
    if (!symbol_file) return -1;

    return 0;
}

int debugger_attach_process(u64 pid)
{
    if (pid == 0) return -1;

    kdbg_state.current_thread_id = pid;

    if (kdbg_state.logging_enabled) {
        printf("[DEBUGGER] Attached to process %llu\n", pid);
    }

    return 0;
}

int debugger_detach_process(void)
{
    kdbg_state.current_thread_id = 0;

    if (kdbg_state.logging_enabled) {
        printf("[DEBUGGER] Detached from process\n");
    }

    return 0;
}

int debugger_list_threads(u64 **thread_ids, u32 *count)
{
    if (!thread_ids || !count) return -1;

    u64 *result = (u64 *)calloc(1, sizeof(u64));
    if (!result) return -1;

    result[0] = kdbg_state.current_thread_id;

    *thread_ids = result;
    *count = 1;

    return 0;
}

int debugger_switch_thread(u64 thread_id)
{
    if (thread_id == 0) return -1;

    kdbg_state.current_thread_id = thread_id;

    if (kdbg_state.logging_enabled) {
        printf("[DEBUGGER] Switched to thread %llu\n", thread_id);
    }

    return 0;
}

int debugger_enable_logging(const char *log_file)
{
    if (!log_file) return -1;

    kdbg_state.log_file = log_file;
    kdbg_state.logging_enabled = 1;

    printf("[DEBUGGER] Logging enabled to %s\n", log_file);

    return 0;
}

int debugger_disable_logging(void)
{
    kdbg_state.logging_enabled = 0;
    kdbg_state.log_file = NULL;

    return 0;
}

int debugger_log_message(const char *fmt, ...)
{
    if (!fmt) return -1;

    if (kdbg_state.logging_enabled) {
        printf("[DEBUGGER] %s\n", fmt);
    }

    return 0;
}
