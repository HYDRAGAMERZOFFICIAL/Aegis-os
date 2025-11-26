#ifndef AEGIS_KERNEL_DEBUGGER_H
#define AEGIS_KERNEL_DEBUGGER_H

#include <kernel/types.h>

typedef enum {
    BREAKPOINT_TYPE_INSTRUCTION,
    BREAKPOINT_TYPE_DATA_READ,
    BREAKPOINT_TYPE_DATA_WRITE,
    BREAKPOINT_TYPE_EXECUTE
} breakpoint_type_t;

typedef enum {
    DBG_STATE_DISABLED,
    DBG_STATE_RUNNING,
    DBG_STATE_STOPPED,
    DBG_STATE_STEPPING
} debugger_exec_state_t;

typedef struct {
    u64 address;
    breakpoint_type_t type;
    bool enabled;
    u32 hit_count;
    u64 condition;
} breakpoint_t;

typedef struct {
    u64 address;
    u32 size;
    const char *name;
    u32 index;
} watchpoint_t;

typedef struct {
    u64 rax, rbx, rcx, rdx;
    u64 rsi, rdi, rbp, rsp;
    u64 r8, r9, r10, r11;
    u64 r12, r13, r14, r15;
    u64 rip, rflags;
    u64 cs, ss, ds, es, fs, gs;
} cpu_registers_t;

typedef struct {
    u64 address;
    const char *filename;
    u32 line_number;
    const char *function_name;
} stack_frame_t;

typedef struct {
    debugger_exec_state_t state;
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
} debugger_state_t;

int debugger_init(void);
int debugger_enable(void);
int debugger_disable(void);
debugger_state_t debugger_get_state(void);

int debugger_set_breakpoint(u64 address, breakpoint_type_t type);
int debugger_remove_breakpoint(u64 address);
int debugger_enable_breakpoint(u64 address);
int debugger_disable_breakpoint(u64 address);
int debugger_set_conditional_breakpoint(u64 address, u64 condition);
breakpoint_t *debugger_get_breakpoints(u32 *count);

int debugger_set_watchpoint(u64 address, u32 size, const char *name);
int debugger_remove_watchpoint(u64 address);
watchpoint_t *debugger_get_watchpoints(u32 *count);

int debugger_single_step(void);
int debugger_continue(void);
int debugger_step_over(void);
int debugger_step_into(void);
int debugger_step_out(void);

int debugger_read_memory(u64 address, u8 *buffer, u32 size);
int debugger_write_memory(u64 address, const u8 *buffer, u32 size);
cpu_registers_t *debugger_get_registers(void);
int debugger_set_register(const char *reg_name, u64 value);

stack_frame_t *debugger_get_call_stack(u32 *depth);
int debugger_print_stack_trace(void);
int debugger_set_symbol_table(const char *symbol_file);

int debugger_attach_process(u64 pid);
int debugger_detach_process(void);
int debugger_list_threads(u64 **thread_ids, u32 *count);
int debugger_switch_thread(u64 thread_id);

int debugger_enable_logging(const char *log_file);
int debugger_disable_logging(void);
int debugger_log_message(const char *fmt, ...);

#endif
