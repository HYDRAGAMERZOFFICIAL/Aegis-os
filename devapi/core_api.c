#include <devapi/core_api.h>
#include <string.h>

int aegis_process_create(const char *name, const char *entrypoint, 
                         int argc, const char **argv, aegis_pid_t *pid) { return 0; }
int aegis_process_create_with_memory(const char *name, const char *entrypoint, 
                                      uint64_t memory_limit, 
                                      int argc, const char **argv, aegis_pid_t *pid) { return 0; }
int aegis_process_terminate(aegis_pid_t pid, int exit_code) { return 0; }
int aegis_process_wait(aegis_pid_t pid, int *exit_code, uint32_t timeout_ms) { return 0; }
int aegis_process_suspend(aegis_pid_t pid) { return 0; }
int aegis_process_resume(aegis_pid_t pid) { return 0; }
int aegis_process_get_info(aegis_pid_t pid, aegis_process_info_t *info) { return 0; }
int aegis_process_get_current(aegis_pid_t *pid) { return 0; }

int aegis_thread_create(const char *name, aegis_thread_func_t func, 
                        void *arg, aegis_tid_t *tid) { return 0; }
int aegis_thread_create_with_priority(const char *name, aegis_thread_func_t func, 
                                      void *arg, aegis_thread_priority_t priority,
                                      aegis_tid_t *tid) { return 0; }
int aegis_thread_create_realtime(const char *name, aegis_thread_func_t func, 
                                 void *arg, uint32_t deadline_us, aegis_tid_t *tid) { return 0; }
int aegis_thread_terminate(aegis_tid_t tid) { return 0; }
int aegis_thread_join(aegis_tid_t tid, uint32_t timeout_ms) { return 0; }
int aegis_thread_detach(aegis_tid_t tid) { return 0; }
int aegis_thread_get_info(aegis_tid_t tid, aegis_thread_info_t *info) { return 0; }
int aegis_thread_get_current(aegis_tid_t *tid) { return 0; }
int aegis_thread_set_priority(aegis_tid_t tid, aegis_thread_priority_t priority) { return 0; }
int aegis_thread_set_cpu_affinity(aegis_tid_t tid, uint8_t cpu_mask) { return 0; }
int aegis_thread_set_cleanup_handler(aegis_cleanup_callback_t callback, void *arg) { return 0; }

void *aegis_memory_alloc(uint64_t size) { return 0; }
void *aegis_memory_alloc_aligned(uint64_t size, uint32_t alignment) { return 0; }
int aegis_memory_free(void *ptr) { return 0; }
int aegis_memory_resize(void **ptr, uint64_t new_size) { return 0; }
uint64_t aegis_memory_get_usage(void) { return 0; }
uint64_t aegis_memory_get_limit(void) { return 0; }

int aegis_device_open(const char *device_name, uint32_t flags, aegis_handle_t *handle) { return 0; }
int aegis_device_close(aegis_handle_t handle) { return 0; }
int aegis_device_ioctl(aegis_handle_t handle, uint32_t command, void *data, uint32_t data_len) { return 0; }
int aegis_device_read(aegis_handle_t handle, void *buffer, uint32_t size, uint32_t *bytes_read) { return 0; }
int aegis_device_write(aegis_handle_t handle, const void *buffer, uint32_t size, uint32_t *bytes_written) { return 0; }

int aegis_time_get_monotonic(uint64_t *time_us) { return 0; }
int aegis_time_get_wall_clock(uint64_t *time_us) { return 0; }
int aegis_time_sleep(uint64_t duration_us) { return 0; }
int aegis_time_set_deadline(uint64_t deadline_us) { return 0; }

int aegis_error_to_string(aegis_error_t error, char *buffer, uint32_t buffer_len) { return 0; }
const char *aegis_get_version(void) { return "1.0.0"; }

int aegis_syscall_wrapper(uint32_t syscall_num, void *args) { return 0; }
int aegis_syscall_get_available(uint32_t *syscalls, uint32_t *count, uint32_t max_syscalls) { return 0; }

int aegis_capability_grant(aegis_pid_t pid, uint32_t capability_mask) { return 0; }
int aegis_capability_revoke(aegis_pid_t pid, uint32_t capability_mask) { return 0; }
int aegis_capability_check(aegis_pid_t pid, uint32_t capability) { return 0; }

int aegis_event_create(const char *event_name, uint8_t manual_reset, uint8_t initial_state) { return 0; }
int aegis_event_set(const char *event_name) { return 0; }
int aegis_event_clear(const char *event_name) { return 0; }
int aegis_event_wait(const char *event_name, uint32_t timeout_ms) { return 0; }
