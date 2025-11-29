#ifndef SERVICES_H
#define SERVICES_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    SERVICE_STATE_STOPPED = 0,
    SERVICE_STATE_STARTING = 1,
    SERVICE_STATE_RUNNING = 2,
    SERVICE_STATE_STOPPING = 3,
    SERVICE_STATE_FAILED = 4,
} ServiceState;

typedef enum {
    SERVICE_TYPE_SYSTEM = 0,
    SERVICE_TYPE_NETWORK = 1,
    SERVICE_TYPE_STORAGE = 2,
    SERVICE_TYPE_SECURITY = 3,
    SERVICE_TYPE_UTILITY = 4,
} ServiceType;

typedef struct {
    uint32_t service_id;
    char name[64];
    ServiceType type;
    ServiceState state;
    uint32_t pid;
    uint64_t uptime;
    uint64_t memory_usage;
    void (*on_start)(void);
    void (*on_stop)(void);
} Service;

typedef struct {
    char key[64];
    char value[256];
} ServiceConfig;

typedef struct {
    uint32_t service_id;
    uint64_t timestamp;
    int severity;
    char message[256];
} ServiceEvent;

int service_manager_init(void);
int service_register(uint32_t* service_id, const char* name, ServiceType type);
int service_start(uint32_t service_id);
int service_stop(uint32_t service_id);
int service_restart(uint32_t service_id);
int service_get_state(uint32_t service_id);
int service_get_info(uint32_t service_id, Service* info);
int service_set_config(uint32_t service_id, const char* key, const char* value);
int service_get_config(uint32_t service_id, const char* key, char* value, size_t size);
int service_enable_autostart(uint32_t service_id);
int service_disable_autostart(uint32_t service_id);
int service_get_dependencies(uint32_t service_id, uint32_t* deps, size_t* count);
int service_set_dependency(uint32_t service_id, uint32_t dep_id);

int syslog_init(void);
int syslog_write(int level, const char* message);
int syslog_get_logs(int level, char* buffer, size_t size);

int network_service_init(void);
int network_service_start(void);
int network_service_stop(void);
int network_get_ip(const char* interface, char* ip, size_t size);

int storage_service_init(void);
int storage_service_mount_all(void);
int storage_service_unmount_all(void);

int security_service_init(void);
int security_service_verify_integrity(void);

#endif
