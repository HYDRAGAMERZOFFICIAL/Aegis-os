#include <kernel/types.h>

static u32 cpu_count = 0;

typedef struct {
    u32 id;
    bool online;
    u64 frequency;
} cpu_info_t;

static cpu_info_t cpu_info[MAX_CPUS] = {0};

u32 cpu_get_count(void)
{
    return cpu_count;
}

void cpu_set_count(u32 count)
{
    cpu_count = (count < MAX_CPUS) ? count : MAX_CPUS;
}

bool cpu_is_online(u32 cpu_id)
{
    if (cpu_id >= cpu_count) return false;
    return cpu_info[cpu_id].online;
}

void cpu_set_online(u32 cpu_id, bool online)
{
    if (cpu_id < cpu_count) {
        cpu_info[cpu_id].online = online;
    }
}

u64 cpu_get_frequency(u32 cpu_id)
{
    if (cpu_id >= cpu_count) return 0;
    return cpu_info[cpu_id].frequency;
}

void cpu_set_frequency(u32 cpu_id, u64 frequency)
{
    if (cpu_id < cpu_count) {
        cpu_info[cpu_id].frequency = frequency;
    }
}

void cpu_init(u32 num_cpus)
{
    cpu_set_count(num_cpus);
    for (u32 i = 0; i < num_cpus; i++) {
        cpu_info[i].id = i;
        cpu_info[i].online = true;
        cpu_info[i].frequency = 2400000000UL;
    }
}

void cpu_halt(void)
{
    asm volatile("hlt");
}

void cpu_pause(void)
{
    asm volatile("pause");
}
