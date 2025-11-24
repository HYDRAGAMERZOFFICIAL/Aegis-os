#ifndef AEGIS_DRIVERS_GPU_H
#define AEGIS_DRIVERS_GPU_H

#include <kernel/types.h>

typedef enum {
    GPU_API_VULKAN = 1,
    GPU_API_OPENGL = 2,
    GPU_API_OPENGL_ES = 3,
} gpu_api_type_t;

typedef enum {
    GPU_FEATURE_ASYNC_COMPUTE = 0x01,
    GPU_FEATURE_RAY_TRACING = 0x02,
    GPU_FEATURE_BINDLESS = 0x04,
    GPU_FEATURE_MESH_SHADING = 0x08,
    GPU_FEATURE_VARIABLE_RATE_SHADING = 0x10,
} gpu_feature_flags_t;

typedef struct {
    char name[64];
    uint32_t vendor_id;
    uint32_t device_id;
    uint64_t vram_size;
    uint32_t feature_flags;
    uint16_t max_compute_units;
    uint32_t max_memory_bandwidth_gbps;
} gpu_device_info_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint8_t format;
    uint32_t usage_flags;
} gpu_buffer_desc_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} gpu_viewport_t;

typedef struct gpu_device gpu_device_t;
typedef struct gpu_context gpu_context_t;
typedef struct gpu_buffer gpu_buffer_t;
typedef struct gpu_shader gpu_shader_t;

typedef int (*gpu_completion_callback_fn)(void *ctx);

int gpu_driver_init(void);
int gpu_driver_shutdown(void);

int gpu_enumerate_devices(gpu_device_info_t *devices, uint32_t *count, uint32_t max_devices);
gpu_device_t *gpu_open_device(uint32_t device_id);
int gpu_close_device(gpu_device_t *dev);

gpu_context_t *gpu_create_context(gpu_device_t *dev, gpu_api_type_t api);
int gpu_destroy_context(gpu_context_t *ctx);
int gpu_make_context_current(gpu_context_t *ctx);

gpu_buffer_t *gpu_create_buffer(gpu_context_t *ctx, const gpu_buffer_desc_t *desc);
int gpu_destroy_buffer(gpu_buffer_t *buf);
int gpu_buffer_write(gpu_buffer_t *buf, uint64_t offset, const void *data, uint64_t size);
int gpu_buffer_read(gpu_buffer_t *buf, uint64_t offset, void *data, uint64_t size);

gpu_shader_t *gpu_create_shader(gpu_context_t *ctx, const char *source, uint32_t source_len, uint8_t stage);
int gpu_destroy_shader(gpu_shader_t *shader);

int gpu_clear_viewport(gpu_context_t *ctx, gpu_viewport_t *vp, uint32_t color);
int gpu_set_viewport(gpu_context_t *ctx, const gpu_viewport_t *vp);

int gpu_submit_work(gpu_context_t *ctx, uint32_t work_dim, const uint32_t *global_size, 
                    const uint32_t *local_size);
int gpu_wait_completion(gpu_context_t *ctx, uint64_t timeout_ms);

int gpu_enable_security_sandbox(gpu_context_t *ctx);
int gpu_disable_security_sandbox(gpu_context_t *ctx);

int gpu_set_completion_callback(gpu_context_t *ctx, gpu_completion_callback_fn callback, void *user_ctx);
int gpu_get_device_info(gpu_device_t *dev, gpu_device_info_t *info);

int gpu_enable_vsync(gpu_device_t *dev);
int gpu_disable_vsync(gpu_device_t *dev);

uint64_t gpu_get_memory_usage(gpu_device_t *dev);
int gpu_profile_performance(gpu_context_t *ctx, char *result, uint32_t max_len);

#endif
