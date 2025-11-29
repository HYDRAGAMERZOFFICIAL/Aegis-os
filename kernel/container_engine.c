#include "container_engine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_CONTAINERS 1024
#define MAX_LOGS_SIZE 8192

static Container g_containers[MAX_CONTAINERS] = {0};
static uint32_t g_container_count = 0;
static uint32_t g_next_container_id = 1;

typedef struct {
    uint32_t container_id;
    ContainerConfig config;
} ContainerConfigSet;

static ContainerConfigSet g_configs[MAX_CONTAINERS] = {0};

typedef struct {
    uint32_t container_id;
    char logs[MAX_LOGS_SIZE];
    uint32_t log_offset;
} ContainerLogs;

static ContainerLogs g_logs[MAX_CONTAINERS] = {0};

int container_engine_init(void) {
    memset(g_containers, 0, sizeof(g_containers));
    memset(g_configs, 0, sizeof(g_configs));
    memset(g_logs, 0, sizeof(g_logs));
    return 0;
}

int container_create(uint32_t* container_id, const char* image_name) {
    if (!container_id || !image_name || g_container_count >= MAX_CONTAINERS) {
        return -1;
    }
    
    *container_id = g_next_container_id++;
    
    Container* cont = &g_containers[g_container_count];
    cont->container_id = *container_id;
    strncpy(cont->name, image_name, sizeof(cont->name) - 1);
    strncpy(cont->image, image_name, sizeof(cont->image) - 1);
    cont->state = CONTAINER_STATE_CREATED;
    cont->pid = 0;
    cont->memory_limit = 512 * 1024 * 1024;
    cont->memory_used = 0;
    cont->cpu_shares = 1024;
    cont->created_at = time(NULL);
    cont->started_at = 0;
    
    ContainerConfigSet* cfg = &g_configs[g_container_count];
    cfg->container_id = *container_id;
    cfg->config.memory_limit = 512 * 1024 * 1024;
    cfg->config.cpu_shares = 1024;
    cfg->config.network_mode = NETWORK_MODE_BRIDGE;
    cfg->config.env_count = 0;
    cfg->config.volume_count = 0;
    
    ContainerLogs* logs = &g_logs[g_container_count];
    logs->container_id = *container_id;
    logs->log_offset = 0;
    
    g_container_count++;
    return 0;
}

int container_start(uint32_t container_id) {
    if (container_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            if (g_containers[i].state == CONTAINER_STATE_RUNNING) {
                return 0;
            }
            
            g_containers[i].state = CONTAINER_STATE_RUNNING;
            g_containers[i].pid = container_id + 2000;
            g_containers[i].started_at = time(NULL);
            
            return 0;
        }
    }
    
    return -1;
}

int container_pause(uint32_t container_id) {
    if (container_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            if (g_containers[i].state == CONTAINER_STATE_RUNNING) {
                g_containers[i].state = CONTAINER_STATE_PAUSED;
                return 0;
            }
        }
    }
    
    return -1;
}

int container_unpause(uint32_t container_id) {
    if (container_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            if (g_containers[i].state == CONTAINER_STATE_PAUSED) {
                g_containers[i].state = CONTAINER_STATE_RUNNING;
                return 0;
            }
        }
    }
    
    return -1;
}

int container_stop(uint32_t container_id) {
    if (container_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            if (g_containers[i].state != CONTAINER_STATE_STOPPED) {
                g_containers[i].state = CONTAINER_STATE_STOPPED;
                g_containers[i].pid = 0;
                return 0;
            }
        }
    }
    
    return -1;
}

int container_restart(uint32_t container_id) {
    int r = container_stop(container_id);
    if (r != 0) return r;
    return container_start(container_id);
}

int container_remove(uint32_t container_id) {
    if (container_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            if (g_containers[i].state != CONTAINER_STATE_STOPPED) {
                return -1;
            }
            
            g_containers[i].state = CONTAINER_STATE_REMOVED;
            g_containers[i].container_id = 0;
            return 0;
        }
    }
    
    return -1;
}

int container_get_state(uint32_t container_id) {
    if (container_id == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            return g_containers[i].state;
        }
    }
    
    return -1;
}

int container_get_info(uint32_t container_id, Container* info) {
    if (container_id == 0 || !info) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            *info = g_containers[i];
            return 0;
        }
    }
    
    return -1;
}

int container_exec_command(uint32_t container_id, const char* command) {
    if (container_id == 0 || !command) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            if (g_containers[i].state != CONTAINER_STATE_RUNNING) {
                return -1;
            }
            
            return 0;
        }
    }
    
    return -1;
}

int container_set_config(uint32_t container_id, ContainerConfig* config) {
    if (container_id == 0 || !config) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_configs[i].container_id == container_id) {
            g_configs[i].config = *config;
            return 0;
        }
    }
    
    return -1;
}

int container_get_logs(uint32_t container_id, char* buffer, size_t size) {
    if (container_id == 0 || !buffer || size == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_logs[i].container_id == container_id) {
            strncpy(buffer, g_logs[i].logs, size - 1);
            return 0;
        }
    }
    
    return -1;
}

int container_set_env(uint32_t container_id, const char* key, const char* value) {
    if (container_id == 0 || !key || !value) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_configs[i].container_id == container_id) {
            if (g_configs[i].config.env_count < 64) {
                strncpy(g_configs[i].config.envs[g_configs[i].config.env_count].key, key, 63);
                strncpy(g_configs[i].config.envs[g_configs[i].config.env_count].value, value, 255);
                g_configs[i].config.env_count++;
                return 0;
            }
        }
    }
    
    return -1;
}

int container_add_volume(uint32_t container_id, const char* source, const char* target) {
    if (container_id == 0 || !source || !target) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_configs[i].container_id == container_id) {
            if (g_configs[i].config.volume_count < 32) {
                strncpy(g_configs[i].config.volumes[g_configs[i].config.volume_count].source, source, 255);
                strncpy(g_configs[i].config.volumes[g_configs[i].config.volume_count].target, target, 255);
                g_configs[i].config.volume_count++;
                return 0;
            }
        }
    }
    
    return -1;
}

int container_set_memory_limit(uint32_t container_id, uint64_t limit) {
    if (container_id == 0 || limit == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            g_containers[i].memory_limit = limit;
            g_configs[i].config.memory_limit = limit;
            return 0;
        }
    }
    
    return -1;
}

int container_set_cpu_shares(uint32_t container_id, uint32_t shares) {
    if (container_id == 0 || shares == 0) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            g_containers[i].cpu_shares = shares;
            g_configs[i].config.cpu_shares = shares;
            return 0;
        }
    }
    
    return -1;
}

int container_list(uint32_t* ids, size_t* count) {
    if (!ids || !count) {
        return -1;
    }
    
    uint32_t found = 0;
    for (uint32_t i = 0; i < g_container_count && found < *count; i++) {
        if (g_containers[i].container_id != 0) {
            ids[found++] = g_containers[i].container_id;
        }
    }
    
    *count = found;
    return 0;
}

int container_get_stats(uint32_t container_id, uint64_t* memory_used, uint32_t* cpu_usage) {
    if (container_id == 0 || !memory_used || !cpu_usage) {
        return -1;
    }
    
    for (uint32_t i = 0; i < g_container_count; i++) {
        if (g_containers[i].container_id == container_id) {
            *memory_used = g_containers[i].memory_used;
            *cpu_usage = 25;
            return 0;
        }
    }
    
    return -1;
}
