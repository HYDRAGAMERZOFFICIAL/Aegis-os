#ifndef AEGIS_HAL_GPU_DISPLAY_H
#define AEGIS_HAL_GPU_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

#define MAX_DISPLAYS 16
#define MAX_PLANES 32
#define MAX_FRAMEBUFFERS 256
#define MAX_DMA_BUF_FDS 512

typedef enum {
    HAL_PIXEL_FORMAT_UNKNOWN = 0,
    HAL_PIXEL_FORMAT_XRGB8888,
    HAL_PIXEL_FORMAT_ARGB8888,
    HAL_PIXEL_FORMAT_RGB565,
    HAL_PIXEL_FORMAT_NV12,
    HAL_PIXEL_FORMAT_YUV420,
    HAL_PIXEL_FORMAT_YUV422,
} hal_pixel_format_t;

typedef enum {
    HAL_CONNECTOR_UNKNOWN = 0,
    HAL_CONNECTOR_HDMI,
    HAL_CONNECTOR_DP,
    HAL_CONNECTOR_LVDS,
    HAL_CONNECTOR_VGA,
    HAL_CONNECTOR_DVI,
    HAL_CONNECTOR_DSI,
} hal_connector_type_t;

typedef enum {
    HAL_PLANE_TYPE_PRIMARY = 0,
    HAL_PLANE_TYPE_OVERLAY,
    HAL_PLANE_TYPE_CURSOR,
} hal_plane_type_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t refresh_hz;
    uint32_t clock_khz;
    uint16_t h_sync_start;
    uint16_t h_sync_end;
    uint16_t h_total;
    uint16_t v_sync_start;
    uint16_t v_sync_end;
    uint16_t v_total;
    hal_pixel_format_t pixel_format;
    bool interlaced;
} hal_display_mode_t;

typedef struct {
    uint32_t display_id;
    uint32_t connector_id;
    hal_connector_type_t connector_type;
    uint32_t width_mm;
    uint32_t height_mm;
    bool connected;
    bool enabled;
    uint32_t mode_count;
    hal_display_mode_t *modes;
    hal_display_mode_t current_mode;
} hal_display_t;

typedef struct {
    uint32_t plane_id;
    uint32_t display_id;
    hal_plane_type_t plane_type;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint64_t framebuffer_id;
    bool enabled;
    uint8_t alpha;
    uint32_t zpos;
} hal_plane_t;

typedef struct {
    uint64_t framebuffer_id;
    uint32_t width;
    uint32_t height;
    hal_pixel_format_t pixel_format;
    uint32_t pitch;
    uint32_t bpp;
    uint64_t physical_address;
    uint64_t size;
    uint8_t *virtual_address;
    uint32_t dma_buf_fd;
    bool secure;
    bool compressed;
} hal_framebuffer_t;

typedef struct {
    uint64_t buffer_handle;
    uint64_t physical_address;
    uint64_t size;
    hal_pixel_format_t pixel_format;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t dma_buf_fd;
    bool mapped;
    bool secure;
} hal_dma_buffer_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
} hal_cursor_config_t;

typedef struct {
    uint32_t display_id;
    uint64_t vsync_fence_id;
    uint64_t flip_fence_id;
} hal_flip_event_t;

typedef void (*hal_flip_callback_t)(const hal_flip_event_t *event, void *context);
typedef void (*hal_vblank_callback_t)(uint32_t display_id, uint64_t timestamp, void *context);

hal_status_t hal_gpu_display_init(void);
hal_status_t hal_gpu_display_fini(void);

hal_status_t hal_gpu_enumerate_displays(hal_display_t *displays, uint32_t *display_count);
hal_status_t hal_gpu_get_display(uint32_t display_id, hal_display_t *display);
hal_status_t hal_gpu_set_display_mode(uint32_t display_id, const hal_display_mode_t *mode);
hal_status_t hal_gpu_get_display_modes(uint32_t display_id, hal_display_mode_t *modes, 
                                        uint32_t *mode_count);
hal_status_t hal_gpu_enable_display(uint32_t display_id);
hal_status_t hal_gpu_disable_display(uint32_t display_id);

hal_status_t hal_gpu_enumerate_planes(uint32_t display_id, hal_plane_t *planes, 
                                       uint32_t *plane_count);
hal_status_t hal_gpu_get_plane(uint32_t plane_id, hal_plane_t *plane);
hal_status_t hal_gpu_set_plane_config(uint32_t plane_id, const hal_plane_t *config);
hal_status_t hal_gpu_enable_plane(uint32_t plane_id);
hal_status_t hal_gpu_disable_plane(uint32_t plane_id);

hal_status_t hal_gpu_allocate_framebuffer(uint32_t width, uint32_t height, 
                                          hal_pixel_format_t format, uint64_t *framebuffer_id);
hal_status_t hal_gpu_free_framebuffer(uint64_t framebuffer_id);
hal_status_t hal_gpu_get_framebuffer(uint64_t framebuffer_id, hal_framebuffer_t *framebuffer);
hal_status_t hal_gpu_map_framebuffer(uint64_t framebuffer_id, uint8_t **vaddr);
hal_status_t hal_gpu_unmap_framebuffer(uint64_t framebuffer_id);

hal_status_t hal_gpu_allocate_dma_buffer(uint64_t size, hal_pixel_format_t format, 
                                         uint64_t *buffer_handle);
hal_status_t hal_gpu_free_dma_buffer(uint64_t buffer_handle);
hal_status_t hal_gpu_get_dma_buffer(uint64_t buffer_handle, hal_dma_buffer_t *buffer);
hal_status_t hal_gpu_map_dma_buffer(uint64_t buffer_handle, uint8_t **vaddr);
hal_status_t hal_gpu_unmap_dma_buffer(uint64_t buffer_handle);
hal_status_t hal_gpu_export_dma_buffer(uint64_t buffer_handle, uint32_t *fd);
hal_status_t hal_gpu_import_dma_buffer(uint32_t fd, uint64_t *buffer_handle);

hal_status_t hal_gpu_page_flip(uint32_t plane_id, uint64_t framebuffer_id, 
                               hal_flip_callback_t callback, void *context);
hal_status_t hal_gpu_wait_vsync(uint32_t display_id, uint64_t timeout_ms);
hal_status_t hal_gpu_register_vblank_callback(uint32_t display_id, 
                                              hal_vblank_callback_t callback, void *context);
hal_status_t hal_gpu_unregister_vblank_callback(uint32_t display_id);

hal_status_t hal_gpu_set_cursor(uint32_t display_id, const hal_cursor_config_t *config, 
                                const uint8_t *cursor_data, uint32_t data_size);
hal_status_t hal_gpu_move_cursor(uint32_t display_id, uint32_t x, uint32_t y);
hal_status_t hal_gpu_hide_cursor(uint32_t display_id);

hal_status_t hal_gpu_get_edid(uint32_t display_id, uint8_t *edid, uint32_t *edid_size);

#endif
