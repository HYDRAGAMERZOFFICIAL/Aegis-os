#ifndef AEGIS_KERNEL_ADAPTIVE_SCHEDULER_H
#define AEGIS_KERNEL_ADAPTIVE_SCHEDULER_H

#include <kernel/types.h>

typedef enum {
    ADAPTIVE_MODE_INTERACTIVE = 1,
    ADAPTIVE_MODE_BACKGROUND = 2,
    ADAPTIVE_MODE_GAMING = 3,
    ADAPTIVE_MODE_POWER_SAVING = 4,
    ADAPTIVE_MODE_BALANCED = 5,
} adaptive_mode_t;

typedef struct {
    uint32_t pid;
    uint64_t cpu_time_us;
    uint32_t context_switches;
    float io_wait_percent;
    adaptive_mode_t detected_mode;
} adaptive_workload_t;

int adaptive_init(void);
int adaptive_enable(void);
int adaptive_disable(void);

int adaptive_detect_workload(uint32_t pid, adaptive_workload_t *workload);
int adaptive_set_mode(uint32_t pid, adaptive_mode_t mode);
int adaptive_get_mode(uint32_t pid, adaptive_mode_t *mode);

int adaptive_boost_cpu(uint32_t pid, uint32_t boost_percent);
int adaptive_reduce_cpu(uint32_t pid, uint32_t reduce_percent);

int adaptive_allocate_cores(uint32_t pid, uint8_t num_cores);
int adaptive_release_cores(uint32_t pid);

int adaptive_optimize_latency(void);
int adaptive_optimize_throughput(void);

float adaptive_get_responsiveness_score(void);
int adaptive_auto_tune(void);

#endif
