#include <kernel/panic.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    panic_info_t *panic_log;
    u32 panic_count;
    u32 max_panics;
    bool halt_on_panic;
    bool reboot_on_panic;
    u32 reboot_timeout_seconds;
    bool dump_enabled;
    const char *dump_file;
} panic_internal_state_t;

static panic_internal_state_t panic_state = {0};

int panic_init(void)
{
    panic_state.max_panics = 256;
    panic_state.panic_count = 0;
    panic_state.halt_on_panic = 1;
    panic_state.reboot_on_panic = 0;
    panic_state.reboot_timeout_seconds = 5;
    panic_state.dump_enabled = 0;
    panic_state.dump_file = NULL;

    panic_state.panic_log = (panic_info_t *)calloc(panic_state.max_panics, sizeof(panic_info_t));
    if (!panic_state.panic_log) return -1;

    return 0;
}

const char *panic_type_to_string(panic_type_t type)
{
    switch (type) {
        case PANIC_TYPE_MEMORY:
            return "Memory Error";
        case PANIC_TYPE_STACK_OVERFLOW:
            return "Stack Overflow";
        case PANIC_TYPE_DOUBLE_FAULT:
            return "Double Fault";
        case PANIC_TYPE_GENERAL_PROTECTION:
            return "General Protection Fault";
        case PANIC_TYPE_PAGE_FAULT:
            return "Page Fault";
        case PANIC_TYPE_UNHANDLED_EXCEPTION:
            return "Unhandled Exception";
        case PANIC_TYPE_WATCHDOG_TIMEOUT:
            return "Watchdog Timeout";
        case PANIC_TYPE_ASSERTION:
            return "Assertion Failed";
        case PANIC_TYPE_CUSTOM:
            return "Custom Panic";
        default:
            return "Unknown Panic";
    }
}

void panic(const char *fmt, ...)
{
    if (!fmt) return;

    if (panic_state.panic_count >= panic_state.max_panics) return;

    panic_info_t *info = &panic_state.panic_log[panic_state.panic_count];
    info->type = PANIC_TYPE_CUSTOM;
    info->message = fmt;
    info->timestamp = 0;
    info->cpu_id = 0;
    info->error_code = 0;

    panic_state.panic_count++;

    printf("KERNEL PANIC: %s\n", fmt);

    if (panic_state.dump_enabled && panic_state.dump_file) {
        panic_dump_stack_trace(0);
        panic_dump_registers();
    }

    if (panic_state.halt_on_panic) {
        while (1) { __asm__("hlt"); }
    }

    if (panic_state.reboot_on_panic) {
        printf("Rebooting in %u seconds...\n", panic_state.reboot_timeout_seconds);
    }
}

void panic_with_error(panic_type_t type, u64 error_code, const char *fmt, ...)
{
    if (!fmt) return;

    if (panic_state.panic_count >= panic_state.max_panics) return;

    panic_info_t *info = &panic_state.panic_log[panic_state.panic_count];
    info->type = type;
    info->message = fmt;
    info->error_code = error_code;
    info->timestamp = 0;
    info->cpu_id = 0;

    panic_state.panic_count++;

    printf("KERNEL PANIC [%s] (0x%llx): %s\n", panic_type_to_string(type), error_code, fmt);

    if (panic_state.dump_enabled && panic_state.dump_file) {
        panic_dump_stack_trace(0);
        panic_dump_registers();
    }

    if (panic_state.halt_on_panic) {
        while (1) { __asm__("hlt"); }
    }
}

void panic_assert_fail(const char *condition, const char *file, u32 line)
{
    if (!condition || !file) return;

    if (panic_state.panic_count >= panic_state.max_panics) return;

    panic_info_t *info = &panic_state.panic_log[panic_state.panic_count];
    info->type = PANIC_TYPE_ASSERTION;
    info->message = condition;
    info->file = file;
    info->line = line;
    info->timestamp = 0;
    info->cpu_id = 0;

    panic_state.panic_count++;

    printf("ASSERTION FAILED: %s at %s:%u\n", condition, file, line);

    if (panic_state.dump_enabled && panic_state.dump_file) {
        panic_dump_stack_trace(0);
        panic_dump_registers();
    }

    if (panic_state.halt_on_panic) {
        while (1) { __asm__("hlt"); }
    }
}

int panic_enable_dump(const char *dump_file)
{
    if (!dump_file) return -1;

    panic_state.dump_enabled = 1;
    panic_state.dump_file = dump_file;

    return 0;
}

int panic_disable_dump(void)
{
    panic_state.dump_enabled = 0;
    panic_state.dump_file = NULL;

    return 0;
}

int panic_set_halt_on_panic(bool enabled)
{
    panic_state.halt_on_panic = enabled ? 1 : 0;
    return 0;
}

int panic_set_reboot_on_panic(bool enabled, u32 timeout_seconds)
{
    panic_state.reboot_on_panic = enabled ? 1 : 0;
    panic_state.reboot_timeout_seconds = timeout_seconds;

    return 0;
}

panic_info_t *panic_get_last_panic(void)
{
    if (panic_state.panic_count == 0) return NULL;

    return &panic_state.panic_log[panic_state.panic_count - 1];
}

panic_info_t **panic_get_panic_history(u32 *count)
{
    if (!count) return NULL;

    panic_info_t **result = (panic_info_t **)calloc(panic_state.panic_count, sizeof(panic_info_t *));
    if (!result) return NULL;

    for (u32 i = 0; i < panic_state.panic_count; i++) {
        result[i] = &panic_state.panic_log[i];
    }

    *count = panic_state.panic_count;

    return result;
}

int panic_clear_history(void)
{
    panic_state.panic_count = 0;
    return 0;
}

int panic_dump_stack_trace(u64 base_pointer)
{
    printf("Stack trace (base pointer: 0x%llx):\n", base_pointer);
    return 0;
}

int panic_dump_registers(void)
{
    printf("Register dump:\n");
    printf("RAX: 0x0000000000000000\n");
    printf("RBX: 0x0000000000000000\n");
    printf("RCX: 0x0000000000000000\n");
    printf("RDX: 0x0000000000000000\n");
    printf("RSI: 0x0000000000000000\n");
    printf("RDI: 0x0000000000000000\n");
    printf("RBP: 0x0000000000000000\n");
    printf("RSP: 0x0000000000000000\n");

    return 0;
}

int panic_dump_memory(u64 address, u32 size)
{
    if (size == 0) return -1;

    printf("Memory dump at 0x%llx (size: %u bytes):\n", address, size);
    for (u32 i = 0; i < size; i += 16) {
        printf("0x%llx: ", address + i);
        for (u32 j = 0; j < 16 && (i + j) < size; j++) {
            printf("%02x ", 0x00);
        }
        printf("\n");
    }

    return 0;
}
