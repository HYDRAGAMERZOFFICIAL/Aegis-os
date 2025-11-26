#ifndef AEGIS_KERNEL_PANIC_H
#define AEGIS_KERNEL_PANIC_H

#include <kernel/types.h>

typedef enum {
    PANIC_TYPE_MEMORY,
    PANIC_TYPE_STACK_OVERFLOW,
    PANIC_TYPE_DOUBLE_FAULT,
    PANIC_TYPE_GENERAL_PROTECTION,
    PANIC_TYPE_PAGE_FAULT,
    PANIC_TYPE_UNHANDLED_EXCEPTION,
    PANIC_TYPE_WATCHDOG_TIMEOUT,
    PANIC_TYPE_ASSERTION,
    PANIC_TYPE_CUSTOM
} panic_type_t;

typedef struct {
    panic_type_t type;
    const char *message;
    const char *file;
    u32 line;
    u64 timestamp;
    u64 rip;
    u64 rsp;
    u64 rbp;
    u32 cpu_id;
    u64 error_code;
} panic_info_t;

typedef struct {
    panic_info_t *panic_log;
    u32 panic_count;
    u32 max_panics;
    bool halt_on_panic;
    bool reboot_on_panic;
    u32 reboot_timeout_seconds;
    bool dump_enabled;
    const char *dump_file;
} panic_state_t;

void panic(const char *fmt, ...);
void panic_with_error(panic_type_t type, u64 error_code, const char *fmt, ...);
void panic_assert_fail(const char *condition, const char *file, u32 line);
int panic_init(void);
int panic_enable_dump(const char *dump_file);
int panic_disable_dump(void);
int panic_set_halt_on_panic(bool enabled);
int panic_set_reboot_on_panic(bool enabled, u32 timeout_seconds);
panic_info_t *panic_get_last_panic(void);
panic_info_t **panic_get_panic_history(u32 *count);
int panic_clear_history(void);
int panic_dump_stack_trace(u64 base_pointer);
int panic_dump_registers(void);
int panic_dump_memory(u64 address, u32 size);
const char *panic_type_to_string(panic_type_t type);

#endif
