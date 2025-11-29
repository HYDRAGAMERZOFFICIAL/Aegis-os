#ifndef AEGIS_KERNEL_SYSCALLS_H
#define AEGIS_KERNEL_SYSCALLS_H

#include <stdint.h>

typedef int (*syscall_handler_t)(void);

#define SYSCALL_PRIVILEGE_USER      0
#define SYSCALL_PRIVILEGE_KERNEL   1
#define SYSCALL_PRIVILEGE_ADMIN    2

void syscall_gate_init(void);

int syscall_register(int syscall_num, syscall_handler_t handler, const char *name, 
                     int privilege_level, int arg_count);

int syscall_unregister(int syscall_num);

int syscall_dispatch(int syscall_num, void *args);

int syscall_get_count(void);

const char *syscall_get_name(int syscall_num);

int syscall_get_privilege_level(int syscall_num);

int syscall_is_registered(int syscall_num);

void syscall_gate_print_table(void);

int syscall_validate_privilege(int syscall_num, int current_privilege);

int syscall_validate_args(int syscall_num, int arg_count);

#define ENOSYS 38

#endif
