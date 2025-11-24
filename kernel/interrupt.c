#include <kernel/interrupt.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    irq_descriptor_t irq_table[MAX_IRQ_HANDLERS];
    irq_stat_t irq_stats[MAX_IRQ_HANDLERS];
    event_queue_t event_queue;
    uint lock;
} ied_state_t;

static ied_state_t ied_state = {0};

int ied_init(void)
{
    memset(ied_state.irq_table, 0, sizeof(ied_state.irq_table));
    memset(ied_state.irq_stats, 0, sizeof(ied_state.irq_stats));
    memset(&ied_state.event_queue, 0, sizeof(ied_state.event_queue));
    ied_state.lock = 0;

    for (u32 i = 0; i < MAX_IRQ_HANDLERS; i++) {
        ied_state.irq_table[i].irq = i;
        ied_state.irq_table[i].handler = NULL;
        ied_state.irq_stats[i].irq = i;
        ied_state.irq_stats[i].count = 0;
    }

    ied_state.event_queue.head = 0;
    ied_state.event_queue.tail = 0;
    ied_state.event_queue.event_count = 0;

    return 0;
}

int ied_register_irq(u32 irq, irq_handler_t handler, void *dev_id, irq_type_t type)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    ied_state.irq_table[irq].irq = irq;
    ied_state.irq_table[irq].handler = handler;
    ied_state.irq_table[irq].dev_id = dev_id;
    ied_state.irq_table[irq].type = type;

    return 0;
}

int ied_unregister_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    ied_state.irq_table[irq].handler = NULL;
    ied_state.irq_table[irq].dev_id = NULL;

    return 0;
}

int ied_enable_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;
    return 0;
}

int ied_disable_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;
    return 0;
}

int ied_dispatch_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    irq_descriptor_t *desc = &ied_state.irq_table[irq];
    if (desc->handler) {
        desc->handler(irq);
        ied_state.irq_stats[irq].count++;
        ied_state.irq_stats[irq].last_handled = 0;
        return 0;
    }

    return -1;
}

int ied_post_event(event_t *event)
{
    if (!event) return -1;

    if (ied_state.event_queue.event_count >= 1024) {
        return -1;
    }

    u32 idx = (ied_state.event_queue.tail) % 1024;
    ied_state.event_queue.events[idx] = event;
    ied_state.event_queue.tail++;
    ied_state.event_queue.event_count++;

    return 0;
}

event_t *ied_get_event(void)
{
    if (ied_state.event_queue.event_count == 0) {
        return NULL;
    }

    u32 idx = (ied_state.event_queue.head) % 1024;
    event_t *event = ied_state.event_queue.events[idx];
    ied_state.event_queue.head++;
    ied_state.event_queue.event_count--;

    return event;
}

int ied_process_events(void)
{
    event_t *event;
    while ((event = ied_get_event()) != NULL) {
        if (event->handler) {
            event->handler(event->data);
        }
    }

    return 0;
}

int ied_set_irq_affinity(u32 irq, u32 cpu_mask)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;
    return 0;
}

u64 ied_get_irq_stat(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return 0;
    return ied_state.irq_stats[irq].count;
}
