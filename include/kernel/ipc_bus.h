#ifndef AEGIS_KERNEL_IPC_BUS_H
#define AEGIS_KERNEL_IPC_BUS_H

#include <stdint.h>

#define MAX_IPC_ROUTES 1024
#define IPC_MAX_QUEUE_SIZE 256
#define IPC_MAX_PRIORITY 10
#define IPC_MESSAGE_SIZE 512

typedef struct {
    int source_id;
    int dest_id;
    int priority;
    uint32_t msg_type;
    uint32_t msg_id;
    uint8_t payload[IPC_MESSAGE_SIZE];
    uint32_t payload_size;
    uint32_t timestamp;
} ipc_message_t;

void ipc_bus_init(void);

int ipc_bus_register_route(int source_id, int dest_id, int priority);

int ipc_bus_send_message(const ipc_message_t *msg);

int ipc_bus_receive_message(int dest_id, ipc_message_t *msg);

int ipc_bus_get_queue_size(int dest_id);

uint32_t ipc_bus_get_dropped_messages(int dest_id);

void ipc_bus_clear_queue(int dest_id);

void ipc_bus_print_stats(void);

int ipc_bus_is_route_available(int source_id, int dest_id);

#endif
