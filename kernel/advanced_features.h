#ifndef ADVANCED_FEATURES_H
#define ADVANCED_FEATURES_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    VM_STATE_STOPPED = 0,
    VM_STATE_RUNNING = 1,
    VM_STATE_PAUSED = 2,
} VirtualMachineState;

typedef enum {
    PARTITION_MUTABLE = 0,
    PARTITION_IMMUTABLE = 1,
    PARTITION_SNAPSHOT = 2,
} PartitionType;

typedef enum {
    IPC_PRIORITY_LOW = 0,
    IPC_PRIORITY_NORMAL = 1,
    IPC_PRIORITY_HIGH = 2,
    IPC_PRIORITY_CRITICAL = 3,
} IPCPriority;

typedef enum {
    CACHE_LEVEL_L1 = 0,
    CACHE_LEVEL_L2 = 1,
    CACHE_LEVEL_L3 = 2,
    CACHE_LEVEL_LLC = 3,
} CacheLevel;

typedef struct {
    uint64_t entry_point;
    uint64_t memory_size;
    uint32_t vcpu_count;
    void* guest_memory;
} VirtualMachine;

typedef struct {
    uint32_t vm_id;
    VirtualMachineState state;
    uint64_t cycles_executed;
    uint32_t active_vcpus;
} VMStatus;

typedef struct {
    uint32_t partition_id;
    PartitionType type;
    uint64_t start_lba;
    uint64_t size;
    uint32_t version;
} Partition;

typedef struct {
    uint32_t src_cap;
    uint32_t dst_cap;
    uint64_t permissions;
} CapabilityTransfer;

typedef struct {
    uint64_t hit_count;
    uint64_t miss_count;
    uint64_t eviction_count;
    uint64_t total_accesses;
} CacheStats;

typedef struct {
    uint32_t msg_id;
    IPCPriority priority;
    uint64_t deadline;
    uint32_t size;
} RealTimeMessage;

int hypervisor_init(void);
int vm_create(uint32_t* vm_id, VirtualMachine* config);
int vm_start(uint32_t vm_id);
int vm_pause(uint32_t vm_id);
int vm_resume(uint32_t vm_id);
int vm_stop(uint32_t vm_id);
int vm_get_status(uint32_t vm_id, VMStatus* status);

int microkernel_init(void);
int microkernel_create_capability(uint32_t* cap_id, uint64_t rights);
int microkernel_grant_capability(uint32_t src, uint32_t dst, CapabilityTransfer* transfer);
int microkernel_revoke_capability(uint32_t cap_id);
int microkernel_get_capability_rights(uint32_t cap_id, uint64_t* rights);

int partition_create(uint32_t* partition_id, PartitionType type, uint64_t size);
int partition_set_immutable(uint32_t partition_id);
int partition_create_snapshot(uint32_t partition_id, uint32_t* snapshot_id);
int partition_restore_snapshot(uint32_t partition_id, uint32_t snapshot_id);
int partition_get_type(uint32_t partition_id);

int ipc_set_priority(uint32_t msg_id, IPCPriority priority);
int ipc_set_deadline(uint32_t msg_id, uint64_t deadline);
int ipc_ensure_delivery(uint32_t msg_id);
int ipc_get_latency(uint32_t msg_id, uint64_t* latency);

int cache_flush(CacheLevel level);
int cache_prefetch(uint64_t address, size_t size);
int cache_get_stats(CacheLevel level, CacheStats* stats);
int cache_set_policy(CacheLevel level, int policy);

int container_create(uint32_t* container_id, const char* name);
int container_start(uint32_t container_id);
int container_stop(uint32_t container_id);
int container_get_status(uint32_t container_id);

int numa_get_local_node(int cpu_id);
int numa_allocate_local(size_t size, int node);
int numa_get_distance(int node1, int node2);

int heterogeneous_scheduler_init(void);
int heterogeneous_scheduler_assign(int pid, int core_type);

int memory_tagging_init(void);
int memory_tag_allocate(void** ptr, size_t size, uint8_t tag);
int memory_check_tag(void* ptr, uint8_t expected_tag);

int isolation_create_domain(uint32_t* domain_id);
int isolation_add_process(uint32_t domain_id, int pid);
int isolation_set_memory_isolation(uint32_t domain_id, int enabled);

#endif
