#include <kernel/syscalls.h>
#include <kernel/process.h>
#include <kernel/memory.h>
#include <kernel/ipc.h>
#include <kernel/filesystem.h>
#include <kernel/interrupt.h>
#include <common/string.h>
#include <stdio.h>

typedef int (*syscall_handler_t)(void);

typedef struct {
    syscall_handler_t handler;
    const char *name;
    int privilege_level;
    int arg_count;
} syscall_entry_t;

#define MAX_SYSCALLS 512
static syscall_entry_t syscall_table[MAX_SYSCALLS];
static int syscall_count = 0;

void syscall_gate_init(void)
{
    memset(syscall_table, 0, sizeof(syscall_table));
    syscall_count = 0;
}

int syscall_register(int syscall_num, syscall_handler_t handler, const char *name, 
                     int privilege_level, int arg_count)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return -1;
    }
    
    if (syscall_table[syscall_num].handler != NULL) {
        return -1;
    }
    
    syscall_table[syscall_num].handler = handler;
    syscall_table[syscall_num].name = name;
    syscall_table[syscall_num].privilege_level = privilege_level;
    syscall_table[syscall_num].arg_count = arg_count;
    
    syscall_count++;
    return 0;
}

int syscall_unregister(int syscall_num)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return -1;
    }
    
    if (syscall_table[syscall_num].handler == NULL) {
        return -1;
    }
    
    syscall_table[syscall_num].handler = NULL;
    syscall_table[syscall_num].name = NULL;
    syscall_count--;
    return 0;
}

int syscall_dispatch(int syscall_num, void *args)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return -ENOSYS;
    }
    
    if (syscall_table[syscall_num].handler == NULL) {
        return -ENOSYS;
    }
    
    return syscall_table[syscall_num].handler();
}

int syscall_get_count(void)
{
    return syscall_count;
}

const char *syscall_get_name(int syscall_num)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return NULL;
    }
    
    return syscall_table[syscall_num].name;
}

int syscall_get_privilege_level(int syscall_num)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return -1;
    }
    
    return syscall_table[syscall_num].privilege_level;
}

int syscall_is_registered(int syscall_num)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return 0;
    }
    
    return syscall_table[syscall_num].handler != NULL;
}

void syscall_gate_print_table(void)
{
    printf("\n=== System Call Table ===\n");
    printf("%-4s | %-25s | %-4s | %-4s\n", "Num", "Name", "Priv", "Args");
    printf("-----|---------------------------|------|-----\n");
    
    for (int i = 0; i < MAX_SYSCALLS; i++) {
        if (syscall_table[i].handler != NULL) {
            printf("%-4d | %-25s | %-4d | %-4d\n",
                   i,
                   syscall_table[i].name,
                   syscall_table[i].privilege_level,
                   syscall_table[i].arg_count);
        }
    }
    
    printf("\nTotal: %d syscalls registered\n", syscall_count);
}

int syscall_validate_privilege(int syscall_num, int current_privilege)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return 0;
    }
    
    if (syscall_table[syscall_num].handler == NULL) {
        return 0;
    }
    
    return current_privilege >= syscall_table[syscall_num].privilege_level;
}

int syscall_validate_args(int syscall_num, int arg_count)
{
    if (syscall_num < 0 || syscall_num >= MAX_SYSCALLS) {
        return 0;
    }
    
    if (syscall_table[syscall_num].handler == NULL) {
        return 0;
    }
    
    return arg_count >= syscall_table[syscall_num].arg_count;
}
