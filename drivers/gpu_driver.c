#include <drivers/gpu_driver.h>
#include <string.h>

int gpu_driver_init(void) { return 0; }
int gpu_driver_shutdown(void) { return 0; }

int gpu_enumerate_devices(gpu_device_info_t *devices, uint32_t *count, uint32_t max_devices) { return 0; }
gpu_device_t *gpu_open_device(uint32_t device_id) { return 0; }
int gpu_close_device(gpu_device_t *dev) { return 0; }

gpu_context_t *gpu_create_context(gpu_device_t *dev, gpu_api_type_t api) { return 0; }
int gpu_destroy_context(gpu_context_t *ctx) { return 0; }
int gpu_make_context_current(gpu_context_t *ctx) { return 0; }

gpu_buffer_t *gpu_create_buffer(gpu_context_t *ctx, const gpu_buffer_desc_t *desc) { return 0; }
int gpu_destroy_buffer(gpu_buffer_t *buf) { return 0; }
int gpu_buffer_write(gpu_buffer_t *buf, uint64_t offset, const void *data, uint64_t size) { return 0; }
int gpu_buffer_read(gpu_buffer_t *buf, uint64_t offset, void *data, uint64_t size) { return 0; }

gpu_shader_t *gpu_create_shader(gpu_context_t *ctx, const char *source, uint32_t source_len, uint8_t stage) { return 0; }
int gpu_destroy_shader(gpu_shader_t *shader) { return 0; }

int gpu_clear_viewport(gpu_context_t *ctx, gpu_viewport_t *vp, uint32_t color) { return 0; }
int gpu_set_viewport(gpu_context_t *ctx, const gpu_viewport_t *vp) { return 0; }

int gpu_submit_work(gpu_context_t *ctx, uint32_t work_dim, const uint32_t *global_size, 
                    const uint32_t *local_size) { return 0; }
int gpu_wait_completion(gpu_context_t *ctx, uint64_t timeout_ms) { return 0; }

int gpu_enable_security_sandbox(gpu_context_t *ctx) { return 0; }
int gpu_disable_security_sandbox(gpu_context_t *ctx) { return 0; }

int gpu_set_completion_callback(gpu_context_t *ctx, gpu_completion_callback_fn callback, void *user_ctx) { return 0; }
int gpu_get_device_info(gpu_device_t *dev, gpu_device_info_t *info) { return 0; }

int gpu_enable_vsync(gpu_device_t *dev) { return 0; }
int gpu_disable_vsync(gpu_device_t *dev) { return 0; }

uint64_t gpu_get_memory_usage(gpu_device_t *dev) { return 0; }
int gpu_profile_performance(gpu_context_t *ctx, char *result, uint32_t max_len) { return 0; }
