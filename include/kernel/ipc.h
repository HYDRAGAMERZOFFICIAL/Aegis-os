#ifndef AEGIS_KERNEL_IPC_H
#define AEGIS_KERNEL_IPC_H

#include <kernel/types.h>

typedef enum {
    IPC_TYPE_MESSAGE,
    IPC_TYPE_PIPE,
    IPC_TYPE_SHARED_MEMORY,
    IPC_TYPE_SEMAPHORE,
    IPC_TYPE_SIGNAL
} ipc_type_t;

typedef struct {
    u32 mtype;
    u64 sender_pid;
    u64 receiver_pid;
    u64 size;
    void *data;
    u64 timestamp;
} message_t;

typedef struct {
    u64 id;
    ipc_type_t type;
    u64 sender_pid;
    u64 receiver_pid;
    bool secure;
    union {
        message_t msg;
        struct {
            u8 *buffer;
            u64 read_pos, write_pos;
            u64 size;
        } pipe;
        struct {
            void *memory;
            u64 size;
            u32 owner_pid;
        } shm;
        struct {
            u32 value;
            u32 owner_pid;
        } sem;
    } data;
} ipc_object_t;

typedef struct {
    u64 id;
    int signum;
    void (*handler)(int);
    u64 pid;
} signal_handler_t;

int ipc_init(void);
ipc_object_t *ipc_create_message_queue(u64 src_pid, u64 dst_pid);
int ipc_send_message(ipc_object_t *obj, message_t *msg);
message_t *ipc_receive_message(ipc_object_t *obj, u64 timeout);
ipc_object_t *ipc_create_pipe(u64 src_pid, u64 dst_pid);
int ipc_write_pipe(ipc_object_t *obj, const void *data, u64 size);
int ipc_read_pipe(ipc_object_t *obj, void *data, u64 max_size);
ipc_object_t *ipc_create_shared_memory(u64 owner_pid, u64 size);
void *ipc_attach_shared_memory(ipc_object_t *obj, u64 pid);
int ipc_detach_shared_memory(ipc_object_t *obj);
ipc_object_t *ipc_create_semaphore(u32 initial_value);
int ipc_semaphore_wait(ipc_object_t *obj);
int ipc_semaphore_signal(ipc_object_t *obj);
int ipc_register_signal_handler(int signum, void (*handler)(int), u64 pid);
int ipc_send_signal(u64 target_pid, int signum);
int ipc_enforce_process_boundary(u64 src_pid, u64 dst_pid);

#endif
