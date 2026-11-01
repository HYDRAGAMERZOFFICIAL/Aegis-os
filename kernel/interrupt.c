#include <kernel/interrupt.h>
#include <string.h>
#include <stdlib.h>

/*
 * Interrupt Event Dispatcher (IED) Subsystem
 * Controls hardware interrupt lines via 8259A PIC & APIC controllers,
 * dispatches registered IRQ handlers, and processes kernel event queues.
 */

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI   0x20

#if defined(__x86_64__) && !defined(_WIN32)
static inline void port_outb(u16 port, u8 val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline u8 port_inb(u16 port) {
    u8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
#else
// Host simulation state
static u8 sim_pic1_imr = 0xFF; // All masked initially
static u8 sim_pic2_imr = 0xFF;
static inline void port_outb(u16 port, u8 val) {
    if (port == PIC1_DATA) sim_pic1_imr = val;
    else if (port == PIC2_DATA) sim_pic2_imr = val;
}
static inline u8 port_inb(u16 port) {
    if (port == PIC1_DATA) return sim_pic1_imr;
    if (port == PIC2_DATA) return sim_pic2_imr;
    return 0;
}
#endif

typedef struct {
    irq_descriptor_t irq_table[MAX_IRQ_HANDLERS];
    irq_stat_t irq_stats[MAX_IRQ_HANDLERS];
    u32 irq_affinity[MAX_IRQ_HANDLERS];
    u64 irq_enabled_mask; // 64-bit mask of active IRQs
    event_queue_t event_queue;
    uint lock;
    bool pic_remapped;
} ied_state_t;

static ied_state_t ied_state = {0};

/*
 * Remap legacy 8259 PIC vectors to 0x20-0x2F (vectors 32-47)
 * to avoid collision with standard 32 x86-64 CPU exceptions.
 */
static void pic_remap(void)
{
    // Start initialization sequence in cascade mode (ICW1)
    port_outb(PIC1_CMD, 0x11);
    port_outb(PIC2_CMD, 0x11);

    // ICW2: Vector offsets
    port_outb(PIC1_DATA, 0x20); // Master IRQ0-7 -> vectors 0x20..0x27
    port_outb(PIC2_DATA, 0x28); // Slave  IRQ8-15 -> vectors 0x28..0x2F

    // ICW3: Cascade wiring
    port_outb(PIC1_DATA, 0x04); // Master has slave on IRQ2 (bit 2 = 4)
    port_outb(PIC2_DATA, 0x02); // Slave identity = 2

    // ICW4: 8086 mode
    port_outb(PIC1_DATA, 0x01);
    port_outb(PIC2_DATA, 0x01);

    // Start with all masked except cascade line (IRQ2)
    port_outb(PIC1_DATA, 0xFB); // 1111 1011 (IRQ2 unmasked)
    port_outb(PIC2_DATA, 0xFF); // 1111 1111 (all slave IRQs masked)

    ied_state.pic_remapped = true;
}

int ied_init(void)
{
    memset(ied_state.irq_table, 0, sizeof(ied_state.irq_table));
    memset(ied_state.irq_stats, 0, sizeof(ied_state.irq_stats));
    memset(ied_state.irq_affinity, 0, sizeof(ied_state.irq_affinity));
    memset(&ied_state.event_queue, 0, sizeof(ied_state.event_queue));
    ied_state.lock = 0;
    ied_state.irq_enabled_mask = 0;

    for (u32 i = 0; i < MAX_IRQ_HANDLERS; i++) {
        ied_state.irq_table[i].irq = i;
        ied_state.irq_table[i].handler = NULL;
        ied_state.irq_stats[i].irq = i;
        ied_state.irq_stats[i].count = 0;
        ied_state.irq_affinity[i] = 0x01; // CPU 0 default
    }

    ied_state.event_queue.head = 0;
    ied_state.event_queue.tail = 0;
    ied_state.event_queue.event_count = 0;

    pic_remap();
    return 0;
}

int ied_register_irq(u32 irq, irq_handler_t handler, void *dev_id, irq_type_t type)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    ied_state.irq_table[irq].irq = irq;
    ied_state.irq_table[irq].handler = handler;
    ied_state.irq_table[irq].dev_id = dev_id;
    ied_state.irq_table[irq].type = type;

    // Automatically enable IRQ in hardware controller once registered
    ied_enable_irq(irq);

    return 0;
}

int ied_unregister_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    ied_disable_irq(irq);
    ied_state.irq_table[irq].handler = NULL;
    ied_state.irq_table[irq].dev_id = NULL;

    return 0;
}

int ied_enable_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    // Mark in software enabled mask
    ied_state.irq_enabled_mask |= (1ULL << irq);

    // Unmask in hardware 8259 PIC controller
    if (irq < 8) {
        u8 mask = port_inb(PIC1_DATA);
        mask &= ~(1 << irq);
        port_outb(PIC1_DATA, mask);
    } else if (irq < 16) {
        // Ensure cascade IRQ2 on master is unmasked
        u8 master_mask = port_inb(PIC1_DATA);
        master_mask &= ~(1 << 2);
        port_outb(PIC1_DATA, master_mask);

        // Unmask slave line
        u8 slave_mask = port_inb(PIC2_DATA);
        slave_mask &= ~(1 << (irq - 8));
        port_outb(PIC2_DATA, slave_mask);
    }

    return 0;
}

int ied_disable_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    // Clear in software mask
    ied_state.irq_enabled_mask &= ~(1ULL << irq);

    // Mask in hardware 8259 PIC controller
    if (irq < 8) {
        u8 mask = port_inb(PIC1_DATA);
        mask |= (1 << irq);
        port_outb(PIC1_DATA, mask);
    } else if (irq < 16) {
        u8 slave_mask = port_inb(PIC2_DATA);
        slave_mask |= (1 << (irq - 8));
        port_outb(PIC2_DATA, slave_mask);
    }

    return 0;
}

void ied_send_eoi(u32 irq)
{
    if (irq >= 8) {
        port_outb(PIC2_CMD, PIC_EOI);
    }
    port_outb(PIC1_CMD, PIC_EOI);
}

int ied_dispatch_irq(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return -1;

    irq_descriptor_t *desc = &ied_state.irq_table[irq];
    if (desc->handler) {
        desc->handler(irq);
        ied_state.irq_stats[irq].count++;
        ied_state.irq_stats[irq].last_handled = 0;
        ied_send_eoi(irq);
        return 0;
    }

    ied_send_eoi(irq);
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
    ied_state.irq_affinity[irq] = cpu_mask;
    return 0;
}

u64 ied_get_irq_stat(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return 0;
    return ied_state.irq_stats[irq].count;
}

bool ied_is_irq_enabled(u32 irq)
{
    if (irq >= MAX_IRQ_HANDLERS) return false;
    return (ied_state.irq_enabled_mask & (1ULL << irq)) != 0;
}
