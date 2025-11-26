#ifndef AEGIS_KERNEL_HYPERVISOR_H
#define AEGIS_KERNEL_HYPERVISOR_H

#include <kernel/types.h>

typedef enum {
    HYPERVISOR_TYPE_KVM,
    HYPERVISOR_TYPE_XEN,
    HYPERVISOR_TYPE_HYPER_V,
    HYPERVISOR_TYPE_CUSTOM
} hypervisor_type_t;

typedef enum {
    VM_STATE_STOPPED,
    VM_STATE_PAUSED,
    VM_STATE_RUNNING,
    VM_STATE_HALTED,
    VM_STATE_ERROR
} vm_state_t;

typedef enum {
    VIRT_TYPE_FULL,
    VIRT_TYPE_PARA,
    VIRT_TYPE_HVM
} virtualization_type_t;

typedef struct {
    u64 vm_id;
    const char *vm_name;
    u64 memory_size;
    u32 cpu_count;
    u32 max_cpu_count;
    vm_state_t state;
    virtualization_type_t virt_type;
    u64 creation_time;
    u64 uptime;
} vm_info_t;

typedef struct {
    u64 vcpu_id;
    u64 vm_id;
    bool enabled;
    u64 cycles;
    u64 context_switches;
} vcpu_info_t;

typedef struct {
    u64 phys_addr;
    u64 virt_addr;
    u32 size;
    bool readonly;
    bool executable;
} memory_mapping_t;

typedef struct {
    hypervisor_type_t type;
    vm_info_t *vms;
    u32 vm_count;
    u32 max_vms;
    vcpu_info_t *vcpus;
    u32 vcpu_count;
    bool enabled;
    bool nested_virt_enabled;
    u64 total_memory;
    u32 total_cpus;
} hypervisor_state_t;

int hypervisor_init(hypervisor_type_t type);
int hypervisor_enable(void);
int hypervisor_disable(void);
int hypervisor_is_enabled(void);
hypervisor_type_t hypervisor_get_type(void);

vm_info_t *hypervisor_create_vm(const char *name, u64 memory_mb, u32 cpu_count, virtualization_type_t virt_type);
int hypervisor_destroy_vm(u64 vm_id);
int hypervisor_start_vm(u64 vm_id);
int hypervisor_stop_vm(u64 vm_id);
int hypervisor_pause_vm(u64 vm_id);
int hypervisor_resume_vm(u64 vm_id);
int hypervisor_reboot_vm(u64 vm_id);

vm_info_t *hypervisor_get_vm_info(u64 vm_id);
vm_info_t **hypervisor_list_vms(u32 *count);
vm_state_t hypervisor_get_vm_state(u64 vm_id);

int hypervisor_allocate_memory(u64 vm_id, u64 memory_size);
int hypervisor_deallocate_memory(u64 vm_id, u64 memory_size);
int hypervisor_map_memory(u64 vm_id, u64 guest_addr, u64 host_addr, u32 size);
int hypervisor_unmap_memory(u64 vm_id, u64 guest_addr, u32 size);

int hypervisor_attach_disk(u64 vm_id, const char *image_path, const char *device_name);
int hypervisor_detach_disk(u64 vm_id, const char *device_name);
int hypervisor_attach_network(u64 vm_id, const char *network_name);
int hypervisor_detach_network(u64 vm_id, const char *network_name);

int hypervisor_add_vcpu(u64 vm_id);
int hypervisor_remove_vcpu(u64 vm_id);
int hypervisor_set_vcpu_affinity(u64 vcpu_id, u64 cpu_mask);
vcpu_info_t *hypervisor_get_vcpu_info(u64 vcpu_id);

int hypervisor_save_vm_state(u64 vm_id, const char *snapshot_file);
int hypervisor_restore_vm_state(u64 vm_id, const char *snapshot_file);
int hypervisor_live_migrate_vm(u64 vm_id, const char *target_host);

int hypervisor_enable_nested_virtualization(void);
int hypervisor_disable_nested_virtualization(void);
int hypervisor_set_cpu_pinning(u64 vm_id, u32 vcpu, u32 phys_cpu);

u64 hypervisor_get_total_memory(void);
u32 hypervisor_get_total_cpus(void);
int hypervisor_get_performance_stats(u64 vm_id, char *stats_buf, u32 buf_size);

#endif
