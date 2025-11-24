#ifndef AEGIS_DEVAPI_CORE_H
#define AEGIS_DEVAPI_CORE_H

#include <kernel/types.h>

#define AEGIS_MAX_PROCESS_NAME 64
#define AEGIS_MAX_THREAD_NAME 32
#define AEGIS_MAX_ERROR_MSG 256

typedef uint32_t aegis_pid_t;
typedef uint32_t aegis_tid_t;
typedef uint32_t aegis_handle_t;

typedef enum {
    AEGIS_ERROR_OK = 0,
    AEGIS_ERROR_INVALID_PARAM = -1,
    AEGIS_ERROR_OUT_OF_MEMORY = -2,
    AEGIS_ERROR_PERMISSION_DENIED = -3,
    AEGIS_ERROR_NOT_FOUND = -4,
    AEGIS_ERROR_ALREADY_EXISTS = -5,
    AEGIS_ERROR_TIMEOUT = -6,
    AEGIS_ERROR_INTERRUPTED = -7,
    AEGIS_ERROR_UNKNOWN = -8,
} aegis_error_t;

typedef enum {
    AEGIS_THREAD_PRIORITY_LOW = 0,
    AEGIS_THREAD_PRIORITY_NORMAL = 1,
    AEGIS_THREAD_PRIORITY_HIGH = 2,
    AEGIS_THREAD_PRIORITY_REALTIME = 3,
} aegis_thread_priority_t;

typedef struct {
    char name[AEGIS_MAX_PROCESS_NAME];
    aegis_pid_t pid;
    uint32_t thread_count;
    uint64_t memory_usage_bytes;
    uint8_t state;
} aegis_process_info_t;

typedef struct {
    char name[AEGIS_MAX_THREAD_NAME];
    aegis_tid_t tid;
    aegis_pid_t parent_pid;
    aegis_thread_priority_t priority;
    uint8_t cpu_affinity;
    uint8_t state;
} aegis_thread_info_t;

typedef void (*aegis_thread_func_t)(void *arg);
typedef void (*aegis_cleanup_callback_t)(void *arg);

int aegis_process_create(const char *name, const char *entrypoint, 
                         int argc, const char **argv, aegis_pid_t *pid);
int aegis_process_create_with_memory(const char *name, const char *entrypoint, 
                                      uint64_t memory_limit, 
                                      int argc, const char **argv, aegis_pid_t *pid);
int aegis_process_terminate(aegis_pid_t pid, int exit_code);
int aegis_process_wait(aegis_pid_t pid, int *exit_code, uint32_t timeout_ms);
int aegis_process_suspend(aegis_pid_t pid);
int aegis_process_resume(aegis_pid_t pid);
int aegis_process_get_info(aegis_pid_t pid, aegis_process_info_t *info);
int aegis_process_get_current(aegis_pid_t *pid);

int aegis_thread_create(const char *name, aegis_thread_func_t func, 
                        void *arg, aegis_tid_t *tid);
int aegis_thread_create_with_priority(const char *name, aegis_thread_func_t func, 
                                      void *arg, aegis_thread_priority_t priority,
                                      aegis_tid_t *tid);
int aegis_thread_create_realtime(const char *name, aegis_thread_func_t func, 
                                 void *arg, uint32_t deadline_us, aegis_tid_t *tid);
int aegis_thread_terminate(aegis_tid_t tid);
int aegis_thread_join(aegis_tid_t tid, uint32_t timeout_ms);
int aegis_thread_detach(aegis_tid_t tid);
int aegis_thread_get_info(aegis_tid_t tid, aegis_thread_info_t *info);
int aegis_thread_get_current(aegis_tid_t *tid);
int aegis_thread_set_priority(aegis_tid_t tid, aegis_thread_priority_t priority);
int aegis_thread_set_cpu_affinity(aegis_tid_t tid, uint8_t cpu_mask);
int aegis_thread_set_cleanup_handler(aegis_cleanup_callback_t callback, void *arg);

void *aegis_memory_alloc(uint64_t size);
void *aegis_memory_alloc_aligned(uint64_t size, uint32_t alignment);
int aegis_memory_free(void *ptr);
int aegis_memory_resize(void **ptr, uint64_t new_size);
uint64_t aegis_memory_get_usage(void);
uint64_t aegis_memory_get_limit(void);

int aegis_device_open(const char *device_name, uint32_t flags, aegis_handle_t *handle);
int aegis_device_close(aegis_handle_t handle);
int aegis_device_ioctl(aegis_handle_t handle, uint32_t command, void *data, uint32_t data_len);
int aegis_device_read(aegis_handle_t handle, void *buffer, uint32_t size, uint32_t *bytes_read);
int aegis_device_write(aegis_handle_t handle, const void *buffer, uint32_t size, uint32_t *bytes_written);

int aegis_time_get_monotonic(uint64_t *time_us);
int aegis_time_get_wall_clock(uint64_t *time_us);
int aegis_time_sleep(uint64_t duration_us);
int aegis_time_set_deadline(uint64_t deadline_us);

int aegis_error_to_string(aegis_error_t error, char *buffer, uint32_t buffer_len);
const char *aegis_get_version(void);

int aegis_syscall_wrapper(uint32_t syscall_num, void *args);
int aegis_syscall_get_available(uint32_t *syscalls, uint32_t *count, uint32_t max_syscalls);

int aegis_capability_grant(aegis_pid_t pid, uint32_t capability_mask);
int aegis_capability_revoke(aegis_pid_t pid, uint32_t capability_mask);
int aegis_capability_check(aegis_pid_t pid, uint32_t capability);

int aegis_event_create(const char *event_name, uint8_t manual_reset, uint8_t initial_state);
int aegis_event_set(const char *event_name);
int aegis_event_clear(const char *event_name);
int aegis_event_wait(const char *event_name, uint32_t timeout_ms);

#endif
