#ifndef AEGIS_HAL_AUDIO_H
#define AEGIS_HAL_AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

#define HAL_AUDIO_MAX_PERMISSION_TOKENS 256
#define HAL_AUDIO_RING_BUFFER_SIZE (16 * 1024 * 1024)

typedef enum {
    HAL_AUDIO_FORMAT_PCM_S16 = 1,
    HAL_AUDIO_FORMAT_PCM_S24 = 2,
    HAL_AUDIO_FORMAT_PCM_S32 = 3,
    HAL_AUDIO_FORMAT_PCM_FLOAT = 4,
    HAL_AUDIO_FORMAT_COMPRESSED = 5,
} hal_audio_format_t;

typedef enum {
    HAL_AUDIO_CHANNELS_MONO = 1,
    HAL_AUDIO_CHANNELS_STEREO = 2,
    HAL_AUDIO_CHANNELS_5_1 = 6,
    HAL_AUDIO_CHANNELS_7_1 = 8,
} hal_audio_channels_t;

typedef enum {
    HAL_AUDIO_DEVICE_UNKNOWN = 0,
    HAL_AUDIO_DEVICE_SPEAKER,
    HAL_AUDIO_DEVICE_HEADPHONE,
    HAL_AUDIO_DEVICE_MICROPHONE,
    HAL_AUDIO_DEVICE_LINE_IN,
    HAL_AUDIO_DEVICE_LINE_OUT,
    HAL_AUDIO_DEVICE_HDMI,
    HAL_AUDIO_DEVICE_SPDIF,
} hal_audio_device_type_t;

typedef enum {
    HAL_AUDIO_PRIORITY_BACKGROUND = 0,
    HAL_AUDIO_PRIORITY_NORMAL = 1,
    HAL_AUDIO_PRIORITY_REALTIME_VOIP = 2,
    HAL_AUDIO_PRIORITY_PLAYBACK = 3,
} hal_audio_priority_t;

typedef struct {
    uint8_t device_id;
    hal_audio_device_type_t type;
    uint8_t *device_name;
    uint32_t supported_sample_rates[16];
    hal_audio_format_t *supported_formats;
    uint8_t format_count;
    bool is_connected;
    int8_t volume_db;
    bool is_muted;
} hal_audio_device_t;

typedef struct {
    hal_audio_format_t format;
    uint32_t sample_rate;
    hal_audio_channels_t channels;
    uint32_t frame_size;
    uint8_t bit_depth;
} hal_audio_config_t;

typedef struct {
    uint64_t stream_id;
    uint8_t device_id;
    hal_audio_config_t config;
    bool is_active;
    uint64_t frames_played;
    uint64_t frames_recorded;
} hal_audio_stream_t;

typedef struct {
    uint64_t total_samples;
    uint64_t current_position;
    uint32_t sample_rate;
    uint8_t channels;
    uint32_t bitrate;
    uint32_t duration_ms;
} hal_audio_playback_status_t;

typedef struct {
    uint64_t token_id;
    uint32_t container_id;
    uint8_t app_id[16];
    bool capture_allowed;
    bool playback_allowed;
    bool mixer_allowed;
    uint32_t max_capture_streams;
    bool capture_indicator_enabled;
} hal_audio_permission_token_t;

typedef struct {
    uint32_t latency_us;
    uint32_t underruns;
    uint32_t overruns;
    uint64_t bytes_transferred;
    uint32_t quality_percent;
} hal_audio_ring_buffer_stats_t;

typedef void (*hal_audio_callback_t)(uint64_t stream_id, void *buffer, uint32_t buffer_size);
typedef void (*hal_audio_hotplug_callback_t)(uint8_t device_id, bool connected, void *context);

hal_status_t hal_audio_init(void);
hal_status_t hal_audio_fini(void);
hal_status_t hal_audio_enumerate_devices(uint32_t *device_count);
hal_status_t hal_audio_get_device_info(uint8_t device_id, hal_audio_device_t *device);

hal_status_t hal_audio_create_stream(uint8_t device_id, const hal_audio_config_t *config, 
                                      uint64_t *stream_id);
hal_status_t hal_audio_destroy_stream(uint64_t stream_id);
hal_status_t hal_audio_get_stream_info(uint64_t stream_id, hal_audio_stream_t *stream);

hal_status_t hal_audio_play(uint64_t stream_id, const void *buffer, uint32_t buffer_size, 
                             uint32_t timeout_ms);
hal_status_t hal_audio_record(uint64_t stream_id, void *buffer, uint32_t buffer_size, 
                               uint32_t timeout_ms);
hal_status_t hal_audio_start_stream(uint64_t stream_id);
hal_status_t hal_audio_stop_stream(uint64_t stream_id);
hal_status_t hal_audio_pause_stream(uint64_t stream_id);
hal_status_t hal_audio_resume_stream(uint64_t stream_id);

hal_status_t hal_audio_set_volume(uint8_t device_id, int8_t volume_db);
hal_status_t hal_audio_get_volume(uint8_t device_id, int8_t *volume_db);
hal_status_t hal_audio_mute(uint8_t device_id);
hal_status_t hal_audio_unmute(uint8_t device_id);
hal_status_t hal_audio_is_muted(uint8_t device_id, bool *is_muted);

hal_status_t hal_audio_set_sample_rate(uint64_t stream_id, uint32_t sample_rate);
hal_status_t hal_audio_get_playback_status(uint64_t stream_id, 
                                            hal_audio_playback_status_t *status);
hal_status_t hal_audio_seek(uint64_t stream_id, uint64_t sample_position);

hal_status_t hal_audio_register_callback(uint64_t stream_id, hal_audio_callback_t callback);
hal_status_t hal_audio_unregister_callback(uint64_t stream_id);

hal_status_t hal_audio_apply_effect(uint64_t stream_id, uint32_t effect_id, 
                                     const void *effect_params, uint32_t params_size);
hal_status_t hal_audio_remove_effect(uint64_t stream_id, uint32_t effect_id);

hal_status_t hal_audio_get_device_stats(uint8_t device_id, uint64_t *underruns, 
                                         uint64_t *overruns, uint32_t *latency_ms);

hal_status_t hal_audio_request_permission_token(uint32_t container_id, const uint8_t *app_id, 
                                                 bool capture_required, bool playback_required,
                                                 uint64_t *token_id);
hal_status_t hal_audio_release_permission_token(uint64_t token_id);
hal_status_t hal_audio_verify_token(uint64_t token_id, hal_audio_permission_token_t *token);
hal_status_t hal_audio_has_capture_permission(uint64_t token_id, bool *allowed);

hal_status_t hal_audio_set_stream_priority(uint64_t stream_id, hal_audio_priority_t priority);
hal_status_t hal_audio_get_stream_priority(uint64_t stream_id, hal_audio_priority_t *priority);

hal_status_t hal_audio_register_hotplug_callback(hal_audio_hotplug_callback_t callback, 
                                                  void *context);
hal_status_t hal_audio_unregister_hotplug_callback(void);

hal_status_t hal_audio_get_ring_buffer_stats(uint64_t stream_id, 
                                              hal_audio_ring_buffer_stats_t *stats);
hal_status_t hal_audio_enable_capture_indicator(uint8_t device_id);
hal_status_t hal_audio_disable_capture_indicator(uint8_t device_id);

#endif
