#ifndef CONTAINER_ENGINE_H
#define CONTAINER_ENGINE_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    CONTAINER_STATE_CREATED = 0,
    CONTAINER_STATE_RUNNING = 1,
    CONTAINER_STATE_PAUSED = 2,
    CONTAINER_STATE_STOPPED = 3,
    CONTAINER_STATE_REMOVED = 4,
} ContainerState;

typedef enum {
    NETWORK_MODE_BRIDGE = 0,
    NETWORK_MODE_HOST = 1,
    NETWORK_MODE_CONTAINER = 2,
    NETWORK_MODE_NONE = 3,
} NetworkMode;

typedef struct {
    uint32_t container_id;
    char name[64];
    char image[128];
    ContainerState state;
    uint32_t pid;
    uint64_t memory_limit;
    uint64_t memory_used;
    uint32_t cpu_shares;
    uint64_t created_at;
    uint64_t started_at;
} Container;

typedef struct {
    char key[64];
    char value[256];
} ContainerEnv;

typedef struct {
    char source[256];
    char target[256];
    int read_only;
} ContainerVolume;

typedef struct {
    uint32_t container_id;
    ContainerEnv envs[64];
    uint32_t env_count;
    ContainerVolume volumes[32];
    uint32_t volume_count;
    NetworkMode network_mode;
    uint64_t memory_limit;
    uint32_t cpu_shares;
    char working_dir[256];
} ContainerConfig;

int container_engine_init(void);
int container_create(uint32_t* container_id, const char* image_name);
int container_start(uint32_t container_id);
int container_pause(uint32_t container_id);
int container_unpause(uint32_t container_id);
int container_stop(uint32_t container_id);
int container_restart(uint32_t container_id);
int container_remove(uint32_t container_id);
int container_get_state(uint32_t container_id);
int container_get_info(uint32_t container_id, Container* info);
int container_exec_command(uint32_t container_id, const char* command);
int container_set_config(uint32_t container_id, ContainerConfig* config);
int container_get_logs(uint32_t container_id, char* buffer, size_t size);
int container_set_env(uint32_t container_id, const char* key, const char* value);
int container_add_volume(uint32_t container_id, const char* source, const char* target);
int container_set_memory_limit(uint32_t container_id, uint64_t limit);
int container_set_cpu_shares(uint32_t container_id, uint32_t shares);
int container_list(uint32_t* ids, size_t* count);
int container_get_stats(uint32_t container_id, uint64_t* memory_used, uint32_t* cpu_usage);

#endif
