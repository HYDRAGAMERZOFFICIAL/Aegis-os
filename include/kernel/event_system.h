#ifndef AEGIS_KERNEL_EVENT_SYSTEM_H
#define AEGIS_KERNEL_EVENT_SYSTEM_H

#include <stdint.h>

#define MAX_EVENT_TYPES 256

#define EVENT_PROCESS_CREATED   0
#define EVENT_PROCESS_DESTROYED 1
#define EVENT_PROCESS_SCHEDULED 2
#define EVENT_MEMORY_ALLOCATED  3
#define EVENT_MEMORY_FREED      4
#define EVENT_PAGE_FAULT        5
#define EVENT_IRQ_RECEIVED      6
#define EVENT_IPC_MESSAGE_SENT  7
#define EVENT_IPC_MESSAGE_RECV  8
#define EVENT_DEVICE_ATTACHED   9
#define EVENT_DEVICE_DETACHED   10
#define EVENT_SECURITY_ALERT    11
#define EVENT_SYSCALL_ENTER     12
#define EVENT_SYSCALL_EXIT      13

typedef struct {
    uint32_t event_type;
    uint32_t event_id;
    uint32_t timestamp;
    int source_id;
    uint8_t data[256];
    uint32_t data_size;
} kernel_event_t;

typedef int (*event_callback_t)(const kernel_event_t *event, void *context);

void event_system_init(void);

int event_subscribe(int subscriber_id, uint32_t event_type, event_callback_t callback, void *context);

int event_unsubscribe(int subscriber_id, uint32_t event_type);

int event_publish(const kernel_event_t *event);

uint32_t event_get_count(uint32_t event_type);

uint32_t event_get_last_timestamp(uint32_t event_type);

int event_get_subscriber_count(uint32_t event_type);

void event_print_registry(void);

int event_clear_subscribers(uint32_t event_type);

int event_subscribe_all(int subscriber_id, event_callback_t callback, void *context);

int event_unsubscribe_all(int subscriber_id);

#endif
