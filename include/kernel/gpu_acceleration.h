#ifndef AEGIS_KERNEL_GPU_ACCEL_H
#define AEGIS_KERNEL_GPU_ACCEL_H

#include <kernel/types.h>

typedef enum {
    GPU_TASK_UI_RENDERING = 1,
    GPU_TASK_SHADER_CACHE = 2,
    GPU_TASK_TEXTURE_PROCESSING = 3,
    GPU_TASK_VIDEO_DECODE = 4,
    GPU_TASK_COMPUTE = 5,
} gpu_task_type_t;

typedef struct {
    uint32_t task_id;
    gpu_task_type_t task_type;
    uint32_t priority;
    uint64_t deadline_us;
} gpu_task_t;

int gpu_accel_init(void);
int gpu_accel_enable(void);
int gpu_accel_disable(void);

int gpu_accel_submit_task(gpu_task_t *task);
int gpu_accel_cancel_task(uint32_t task_id);
int gpu_accel_wait_task(uint32_t task_id, uint32_t timeout_ms);

int gpu_accel_enable_ui_rendering(void);
int gpu_accel_disable_ui_rendering(void);

int gpu_accel_enable_shader_cache(void);
uint32_t gpu_accel_get_cache_hit_rate(void);

int gpu_accel_enable_vsync(void);
int gpu_accel_disable_vsync(void);

uint64_t gpu_accel_get_memory_usage(void);
float gpu_accel_get_utilization_percent(void);

int gpu_accel_enable_power_management(void);
int gpu_accel_disable_power_management(void);

#endif
