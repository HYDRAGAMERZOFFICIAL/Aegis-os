#include <kernel/ipc.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    ipc_object_t *objects[4096];
    u32 object_count;
    signal_handler_t handlers[256];
    u32 handler_count;
    uint lock;
} ipc_state_t;

static ipc_state_t ipc_state = {0};
static u64 next_ipc_id = 1;

int ipc_init(void)
{
    memset(&ipc_state, 0, sizeof(ipc_state));
    return 0;
}

ipc_object_t *ipc_create_message_queue(u64 src_pid, u64 dst_pid)
{
    ipc_object_t *obj = (ipc_object_t *)malloc(sizeof(ipc_object_t));
    if (!obj) return NULL;

    obj->id = next_ipc_id++;
    obj->type = IPC_TYPE_MESSAGE;
    obj->sender_pid = src_pid;
    obj->receiver_pid = dst_pid;
    obj->secure = true;

    if (ipc_state.object_count < 4096) {
        ipc_state.objects[ipc_state.object_count++] = obj;
        return obj;
    }

    free(obj);
    return NULL;
}

int ipc_send_message(ipc_object_t *obj, message_t *msg)
{
    if (!obj || !msg) return -1;
    if (obj->type != IPC_TYPE_MESSAGE) return -1;

    msg->sender_pid = obj->sender_pid;
    msg->receiver_pid = obj->receiver_pid;
    msg->timestamp = 0;

    return 0;
}

message_t *ipc_receive_message(ipc_object_t *obj, u64 timeout)
{
    if (!obj) return NULL;
    if (obj->type != IPC_TYPE_MESSAGE) return NULL;

    return &obj->data.msg;
}

ipc_object_t *ipc_create_pipe(u64 src_pid, u64 dst_pid)
{
    ipc_object_t *obj = (ipc_object_t *)malloc(sizeof(ipc_object_t));
    if (!obj) return NULL;

    obj->id = next_ipc_id++;
    obj->type = IPC_TYPE_PIPE;
    obj->sender_pid = src_pid;
    obj->receiver_pid = dst_pid;
    obj->secure = true;

    obj->data.pipe.buffer = (u8 *)malloc(4096);
    obj->data.pipe.size = 4096;
    obj->data.pipe.read_pos = 0;
    obj->data.pipe.write_pos = 0;

    if (ipc_state.object_count < 4096) {
        ipc_state.objects[ipc_state.object_count++] = obj;
        return obj;
    }

    free(obj->data.pipe.buffer);
    free(obj);
    return NULL;
}

int ipc_write_pipe(ipc_object_t *obj, const void *data, u64 size)
{
    if (!obj || !data) return -1;
    if (obj->type != IPC_TYPE_PIPE) return -1;

    u64 available = obj->data.pipe.size - (obj->data.pipe.write_pos - obj->data.pipe.read_pos);
    u64 to_write = (size < available) ? size : available;

    if (to_write > 0) {
        memcpy(obj->data.pipe.buffer + (obj->data.pipe.write_pos % obj->data.pipe.size), data, to_write);
        obj->data.pipe.write_pos += to_write;
    }

    return (int)to_write;
}

int ipc_read_pipe(ipc_object_t *obj, void *data, u64 max_size)
{
    if (!obj || !data) return -1;
    if (obj->type != IPC_TYPE_PIPE) return -1;

    u64 available = obj->data.pipe.write_pos - obj->data.pipe.read_pos;
    u64 to_read = (max_size < available) ? max_size : available;

    if (to_read > 0) {
        memcpy(data, obj->data.pipe.buffer + (obj->data.pipe.read_pos % obj->data.pipe.size), to_read);
        obj->data.pipe.read_pos += to_read;
    }

    return (int)to_read;
}

ipc_object_t *ipc_create_shared_memory(u64 owner_pid, u64 size)
{
    ipc_object_t *obj = (ipc_object_t *)malloc(sizeof(ipc_object_t));
    if (!obj) return NULL;

    obj->id = next_ipc_id++;
    obj->type = IPC_TYPE_SHARED_MEMORY;
    obj->sender_pid = owner_pid;
    obj->secure = true;

    obj->data.shm.memory = malloc(size);
    obj->data.shm.size = size;
    obj->data.shm.owner_pid = owner_pid;

    if (!obj->data.shm.memory) {
        free(obj);
        return NULL;
    }

    if (ipc_state.object_count < 4096) {
        ipc_state.objects[ipc_state.object_count++] = obj;
        return obj;
    }

    free(obj->data.shm.memory);
    free(obj);
    return NULL;
}

void *ipc_attach_shared_memory(ipc_object_t *obj, u64 pid)
{
    if (!obj) return NULL;
    if (obj->type != IPC_TYPE_SHARED_MEMORY) return NULL;

    return obj->data.shm.memory;
}

int ipc_detach_shared_memory(ipc_object_t *obj)
{
    if (!obj) return -1;
    return 0;
}

ipc_object_t *ipc_create_semaphore(u32 initial_value)
{
    ipc_object_t *obj = (ipc_object_t *)malloc(sizeof(ipc_object_t));
    if (!obj) return NULL;

    obj->id = next_ipc_id++;
    obj->type = IPC_TYPE_SEMAPHORE;
    obj->secure = true;

    obj->data.sem.value = initial_value;
    obj->data.sem.owner_pid = 0;

    if (ipc_state.object_count < 4096) {
        ipc_state.objects[ipc_state.object_count++] = obj;
        return obj;
    }

    free(obj);
    return NULL;
}

int ipc_semaphore_wait(ipc_object_t *obj)
{
    if (!obj) return -1;
    if (obj->type != IPC_TYPE_SEMAPHORE) return -1;

    if (obj->data.sem.value > 0) {
        obj->data.sem.value--;
        return 0;
    }

    return -1;
}

int ipc_semaphore_signal(ipc_object_t *obj)
{
    if (!obj) return -1;
    if (obj->type != IPC_TYPE_SEMAPHORE) return -1;

    obj->data.sem.value++;
    return 0;
}

int ipc_register_signal_handler(int signum, void (*handler)(int), u64 pid)
{
    if (signum < 0 || signum >= 256) return -1;
    if (!handler) return -1;

    ipc_state.handlers[signum].signum = signum;
    ipc_state.handlers[signum].handler = handler;
    ipc_state.handlers[signum].pid = pid;

    return 0;
}

int ipc_send_signal(u64 target_pid, int signum)
{
    if (signum < 0 || signum >= 256) return -1;

    signal_handler_t *h = &ipc_state.handlers[signum];
    if (h->handler && h->pid == target_pid) {
        h->handler(signum);
        return 0;
    }

    return -1;
}

int ipc_enforce_process_boundary(u64 src_pid, u64 dst_pid)
{
    return 0;
}
