#ifndef AEGIS_DEVAPI_CONTAINER_H
#define AEGIS_DEVAPI_CONTAINER_H

#include <kernel/types.h>

typedef enum {
    AEGIS_CONTAINER_TYPE_DOCKER = 1,
    AEGIS_CONTAINER_TYPE_PODMAN = 2,
    AEGIS_CONTAINER_TYPE_LXC = 3,
    AEGIS_CONTAINER_TYPE_FIRECRACKER = 4,
} aegis_container_type_t;

typedef enum {
    AEGIS_CONTAINER_STATUS_CREATED = 1,
    AEGIS_CONTAINER_STATUS_RUNNING = 2,
    AEGIS_CONTAINER_STATUS_PAUSED = 3,
    AEGIS_CONTAINER_STATUS_STOPPED = 4,
    AEGIS_CONTAINER_STATUS_EXITED = 5,
} aegis_container_status_t;

typedef enum {
    AEGIS_RESOURCE_TYPE_CPU = 1,
    AEGIS_RESOURCE_TYPE_MEMORY = 2,
    AEGIS_RESOURCE_TYPE_DISK = 3,
    AEGIS_RESOURCE_TYPE_NETWORK = 4,
} aegis_resource_type_t;

typedef struct {
    char id[64];
    char name[128];
    aegis_container_type_t type;
    aegis_container_status_t status;
    char image_name[256];
    uint64_t created_time;
    uint64_t started_time;
    uint64_t memory_usage_bytes;
    uint32_t cpu_usage_percent;
} aegis_container_info_t;

typedef struct {
    char key[64];
    char value[256];
} aegis_container_env_var_t;

typedef struct {
    char host_path[512];
    char container_path[512];
    uint8_t read_only;
} aegis_container_volume_mount_t;

typedef struct {
    char host_port[16];
    char container_port[16];
    char protocol[16];
} aegis_container_port_mapping_t;

typedef struct {
    uint64_t cpu_limit_millicores;
    uint64_t memory_limit_bytes;
    uint64_t disk_limit_bytes;
    uint32_t network_bandwidth_limit_mbps;
} aegis_container_resource_limits_t;

typedef struct {
    char key[64];
    char value[256];
} aegis_container_label_t;

typedef struct {
    uint32_t cpu_percent;
    uint32_t memory_percent;
    uint64_t memory_bytes;
    uint64_t network_rx_bytes;
    uint64_t network_tx_bytes;
    uint64_t disk_read_bytes;
    uint64_t disk_write_bytes;
} aegis_container_stats_t;

typedef struct aegis_container aegis_container_t;

typedef void (*aegis_container_callback_fn)(aegis_container_t *container, uint32_t event, void *ctx);
typedef void (*aegis_container_log_callback_fn)(const char *log_line, void *ctx);

int aegis_container_init(void);
int aegis_container_shutdown(void);

int aegis_container_create(const char *name, const char *image, aegis_container_type_t type,
                           aegis_container_t **container);
int aegis_container_destroy(aegis_container_t *container);

int aegis_container_start(aegis_container_t *container);
int aegis_container_stop(aegis_container_t *container, uint32_t timeout_seconds);
int aegis_container_pause(aegis_container_t *container);
int aegis_container_resume(aegis_container_t *container);
int aegis_container_restart(aegis_container_t *container);
int aegis_container_kill(aegis_container_t *container, int signal);

int aegis_container_set_env_variable(aegis_container_t *container, const char *key, const char *value);
int aegis_container_set_working_directory(aegis_container_t *container, const char *path);
int aegis_container_set_entrypoint(aegis_container_t *container, const char *entrypoint);
int aegis_container_set_command(aegis_container_t *container, const char *command);

int aegis_container_add_volume_mount(aegis_container_t *container, 
                                     const aegis_container_volume_mount_t *mount);
int aegis_container_remove_volume_mount(aegis_container_t *container, const char *container_path);

int aegis_container_add_port_mapping(aegis_container_t *container,
                                     const aegis_container_port_mapping_t *port);
int aegis_container_remove_port_mapping(aegis_container_t *container, const char *container_port);

int aegis_container_set_resource_limits(aegis_container_t *container,
                                        const aegis_container_resource_limits_t *limits);
int aegis_container_get_resource_limits(aegis_container_t *container,
                                        aegis_container_resource_limits_t *limits);

int aegis_container_add_label(aegis_container_t *container, const char *key, const char *value);
int aegis_container_remove_label(aegis_container_t *container, const char *key);
int aegis_container_get_labels(aegis_container_t *container, aegis_container_label_t *labels,
                               uint32_t *count, uint32_t max_labels);

int aegis_container_execute_command(aegis_container_t *container, const char *command,
                                    char *output, uint32_t max_output_len);
int aegis_container_attach_stdin(aegis_container_t *container);
int aegis_container_detach_stdin(aegis_container_t *container);

int aegis_container_get_logs(aegis_container_t *container, char *logs, uint32_t max_len);
int aegis_container_stream_logs(aegis_container_t *container, 
                                aegis_container_log_callback_fn callback, void *ctx);
int aegis_container_stop_log_stream(aegis_container_t *container);

int aegis_container_get_info(aegis_container_t *container, aegis_container_info_t *info);
int aegis_container_get_status(aegis_container_t *container, aegis_container_status_t *status);
int aegis_container_get_stats(aegis_container_t *container, aegis_container_stats_t *stats);

int aegis_container_export_image(aegis_container_t *container, const char *output_path);
int aegis_container_save_checkpoint(aegis_container_t *container, const char *checkpoint_path);
int aegis_container_restore_from_checkpoint(aegis_container_t *container, const char *checkpoint_path);

int aegis_container_create_from_yaml(const char *yaml_path, aegis_container_t **container);
int aegis_container_get_yaml_config(aegis_container_t *container, char *yaml_config, uint32_t max_len);

int aegis_container_list(aegis_container_info_t *containers, uint32_t *count, uint32_t max_containers);
aegis_container_t *aegis_container_get_by_name(const char *name);
aegis_container_t *aegis_container_get_by_id(const char *id);

int aegis_container_register_callback(aegis_container_t *container, uint32_t event_mask,
                                      aegis_container_callback_fn callback, void *ctx);
int aegis_container_unregister_callback(aegis_container_t *container, aegis_container_callback_fn callback);

int aegis_container_enable_security_policy(aegis_container_t *container, const char *policy_file);
int aegis_container_disable_security_policy(aegis_container_t *container);

int aegis_container_get_network_info(aegis_container_t *container, char *network_info, uint32_t max_len);
int aegis_container_connect_to_network(aegis_container_t *container, const char *network_name);
int aegis_container_disconnect_from_network(aegis_container_t *container, const char *network_name);

int aegis_container_health_check(aegis_container_t *container);
int aegis_container_wait_for_ready(aegis_container_t *container, uint32_t timeout_seconds);

#endif
