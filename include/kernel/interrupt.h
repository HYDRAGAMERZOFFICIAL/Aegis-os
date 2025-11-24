#ifndef AEGIS_KERNEL_INTERRUPT_H
#define AEGIS_KERNEL_INTERRUPT_H

#include <kernel/types.h>

typedef enum {
    IRQ_TYPE_LEVEL,
    IRQ_TYPE_EDGE
} irq_type_t;

typedef struct {
    u32 irq;
    irq_handler_t handler;
    void *dev_id;
    irq_type_t type;
} irq_descriptor_t;

typedef struct {
    u32 irq;
    u64 count;
    u64 last_handled;
} irq_stat_t;

typedef enum {
    EVENT_TYPE_TIMER,
    EVENT_TYPE_IO,
    EVENT_TYPE_SIGNAL,
    EVENT_TYPE_IPC
} event_type_t;

typedef struct {
    event_type_t type;
    u64 id;
    void *data;
    void (*handler)(void *data);
} event_t;

typedef struct {
    event_t *events[1024];
    u32 event_count;
    u32 head, tail;
} event_queue_t;

int ied_init(void);
int ied_register_irq(u32 irq, irq_handler_t handler, void *dev_id, irq_type_t type);
int ied_unregister_irq(u32 irq);
int ied_enable_irq(u32 irq);
int ied_disable_irq(u32 irq);
int ied_dispatch_irq(u32 irq);
int ied_post_event(event_t *event);
event_t *ied_get_event(void);
int ied_process_events(void);
int ied_set_irq_affinity(u32 irq, u32 cpu_mask);
u64 ied_get_irq_stat(u32 irq);

#endif
