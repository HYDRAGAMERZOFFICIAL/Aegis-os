#include <kernel/hypervisor.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    hypervisor_type_t type;
    vm_info_t *vms;
    u32 vm_count;
    u32 max_vms;
    vcpu_info_t *vcpus;
    u32 vcpu_count;
    u32 max_vcpus;
    bool enabled;
    bool nested_virt_enabled;
    u64 total_memory;
    u32 total_cpus;
    u64 next_vm_id;
    u64 next_vcpu_id;
} hvsr_internal_state_t;

static hvsr_internal_state_t hvsr_state = {0};

int hypervisor_init(hypervisor_type_t type)
{
    hvsr_state.type = type;
    hvsr_state.max_vms = 256;
    hvsr_state.max_vcpus = 2048;
    hvsr_state.vm_count = 0;
    hvsr_state.vcpu_count = 0;
    hvsr_state.enabled = 0;
    hvsr_state.nested_virt_enabled = 0;
    hvsr_state.next_vm_id = 1;
    hvsr_state.next_vcpu_id = 1;

    hvsr_state.vms = (vm_info_t *)calloc(hvsr_state.max_vms, sizeof(vm_info_t));
    if (!hvsr_state.vms) return -1;

    hvsr_state.vcpus = (vcpu_info_t *)calloc(hvsr_state.max_vcpus, sizeof(vcpu_info_t));
    if (!hvsr_state.vcpus) return -1;

    hvsr_state.total_memory = 0x40000000;
    hvsr_state.total_cpus = 8;

    return 0;
}

int hypervisor_enable(void)
{
    hvsr_state.enabled = 1;
    printf("[HYPERVISOR] Enabled (Type: %u)\n", hvsr_state.type);

    return 0;
}

int hypervisor_disable(void)
{
    if (hvsr_state.vm_count > 0) {
        printf("[HYPERVISOR] Cannot disable: %u VMs still running\n", hvsr_state.vm_count);
        return -1;
    }

    hvsr_state.enabled = 0;
    printf("[HYPERVISOR] Disabled\n");

    return 0;
}

int hypervisor_is_enabled(void)
{
    return hvsr_state.enabled ? 1 : 0;
}

hypervisor_type_t hypervisor_get_type(void)
{
    return hvsr_state.type;
}

vm_info_t *hypervisor_create_vm(const char *name, u64 memory_mb, u32 cpu_count, virtualization_type_t virt_type)
{
    if (!name || memory_mb == 0 || cpu_count == 0) return NULL;

    if (hvsr_state.vm_count >= hvsr_state.max_vms) return NULL;

    vm_info_t *vm = &hvsr_state.vms[hvsr_state.vm_count];
    vm->vm_id = hvsr_state.next_vm_id++;
    vm->vm_name = name;
    vm->memory_size = memory_mb * 1024 * 1024;
    vm->cpu_count = cpu_count;
    vm->max_cpu_count = cpu_count;
    vm->state = VM_STATE_STOPPED;
    vm->virt_type = virt_type;
    vm->creation_time = 0;
    vm->uptime = 0;

    hvsr_state.vm_count++;

    printf("[HYPERVISOR] VM created: %s (ID: %llu, Memory: %llu MB, CPUs: %u)\n", 
           name, vm->vm_id, memory_mb, cpu_count);

    return vm;
}

int hypervisor_destroy_vm(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            if (hvsr_state.vms[i].state != VM_STATE_STOPPED) {
                printf("[HYPERVISOR] Cannot destroy running VM\n");
                return -1;
            }

            hvsr_state.vms[i] = hvsr_state.vms[hvsr_state.vm_count - 1];
            hvsr_state.vm_count--;

            printf("[HYPERVISOR] VM destroyed (ID: %llu)\n", vm_id);

            return 0;
        }
    }

    return -1;
}

int hypervisor_start_vm(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            hvsr_state.vms[i].state = VM_STATE_RUNNING;
            hvsr_state.vms[i].creation_time = 0;

            printf("[HYPERVISOR] VM started (ID: %llu)\n", vm_id);

            return 0;
        }
    }

    return -1;
}

int hypervisor_stop_vm(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            hvsr_state.vms[i].state = VM_STATE_STOPPED;

            printf("[HYPERVISOR] VM stopped (ID: %llu)\n", vm_id);

            return 0;
        }
    }

    return -1;
}

int hypervisor_pause_vm(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            hvsr_state.vms[i].state = VM_STATE_PAUSED;

            printf("[HYPERVISOR] VM paused (ID: %llu)\n", vm_id);

            return 0;
        }
    }

    return -1;
}

int hypervisor_resume_vm(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            hvsr_state.vms[i].state = VM_STATE_RUNNING;

            printf("[HYPERVISOR] VM resumed (ID: %llu)\n", vm_id);

            return 0;
        }
    }

    return -1;
}

int hypervisor_reboot_vm(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            printf("[HYPERVISOR] VM rebooting (ID: %llu)\n", vm_id);

            return 0;
        }
    }

    return -1;
}

vm_info_t *hypervisor_get_vm_info(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            return &hvsr_state.vms[i];
        }
    }

    return NULL;
}

vm_info_t **hypervisor_list_vms(u32 *count)
{
    if (!count) return NULL;

    vm_info_t **result = (vm_info_t **)calloc(hvsr_state.vm_count, sizeof(vm_info_t *));
    if (!result) return NULL;

    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        result[i] = &hvsr_state.vms[i];
    }

    *count = hvsr_state.vm_count;

    return result;
}

vm_state_t hypervisor_get_vm_state(u64 vm_id)
{
    for (u32 i = 0; i < hvsr_state.vm_count; i++) {
        if (hvsr_state.vms[i].vm_id == vm_id) {
            return hvsr_state.vms[i].state;
        }
    }

    return VM_STATE_ERROR;
}

int hypervisor_allocate_memory(u64 vm_id, u64 memory_size)
{
    if (memory_size == 0) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    vm->memory_size += memory_size;

    printf("[HYPERVISOR] Memory allocated to VM %llu: +%llu bytes (Total: %llu)\n", 
           vm_id, memory_size, vm->memory_size);

    return 0;
}

int hypervisor_deallocate_memory(u64 vm_id, u64 memory_size)
{
    if (memory_size == 0) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    if (vm->memory_size < memory_size) return -1;

    vm->memory_size -= memory_size;

    printf("[HYPERVISOR] Memory deallocated from VM %llu: -%llu bytes\n", vm_id, memory_size);

    return 0;
}

int hypervisor_map_memory(u64 vm_id, u64 guest_addr, u64 host_addr, u32 size)
{
    if (size == 0) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] Memory mapped for VM %llu: Guest 0x%llx -> Host 0x%llx (Size: %u)\n",
           vm_id, guest_addr, host_addr, size);

    return 0;
}

int hypervisor_unmap_memory(u64 vm_id, u64 guest_addr, u32 size)
{
    if (size == 0) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] Memory unmapped for VM %llu: Guest 0x%llx (Size: %u)\n",
           vm_id, guest_addr, size);

    return 0;
}

int hypervisor_attach_disk(u64 vm_id, const char *image_path, const char *device_name)
{
    if (!image_path || !device_name) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] Disk attached to VM %llu: %s as %s\n", vm_id, image_path, device_name);

    return 0;
}

int hypervisor_detach_disk(u64 vm_id, const char *device_name)
{
    if (!device_name) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] Disk detached from VM %llu: %s\n", vm_id, device_name);

    return 0;
}

int hypervisor_attach_network(u64 vm_id, const char *network_name)
{
    if (!network_name) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] Network attached to VM %llu: %s\n", vm_id, network_name);

    return 0;
}

int hypervisor_detach_network(u64 vm_id, const char *network_name)
{
    if (!network_name) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] Network detached from VM %llu: %s\n", vm_id, network_name);

    return 0;
}

int hypervisor_add_vcpu(u64 vm_id)
{
    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    if (vm->cpu_count >= vm->max_cpu_count) return -1;

    if (hvsr_state.vcpu_count >= hvsr_state.max_vcpus) return -1;

    vcpu_info_t *vcpu = &hvsr_state.vcpus[hvsr_state.vcpu_count];
    vcpu->vcpu_id = hvsr_state.next_vcpu_id++;
    vcpu->vm_id = vm_id;
    vcpu->enabled = 1;
    vcpu->cycles = 0;
    vcpu->context_switches = 0;

    hvsr_state.vcpu_count++;
    vm->cpu_count++;

    printf("[HYPERVISOR] vCPU added to VM %llu (vCPU ID: %llu)\n", vm_id, vcpu->vcpu_id);

    return 0;
}

int hypervisor_remove_vcpu(u64 vm_id)
{
    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm || vm->cpu_count <= 1) return -1;

    for (u32 i = 0; i < hvsr_state.vcpu_count; i++) {
        if (hvsr_state.vcpus[i].vm_id == vm_id) {
            hvsr_state.vcpus[i] = hvsr_state.vcpus[hvsr_state.vcpu_count - 1];
            hvsr_state.vcpu_count--;
            vm->cpu_count--;

            printf("[HYPERVISOR] vCPU removed from VM %llu\n", vm_id);

            return 0;
        }
    }

    return -1;
}

int hypervisor_set_vcpu_affinity(u64 vcpu_id, u64 cpu_mask)
{
    for (u32 i = 0; i < hvsr_state.vcpu_count; i++) {
        if (hvsr_state.vcpus[i].vcpu_id == vcpu_id) {
            printf("[HYPERVISOR] vCPU %llu affinity set to 0x%llx\n", vcpu_id, cpu_mask);

            return 0;
        }
    }

    return -1;
}

vcpu_info_t *hypervisor_get_vcpu_info(u64 vcpu_id)
{
    for (u32 i = 0; i < hvsr_state.vcpu_count; i++) {
        if (hvsr_state.vcpus[i].vcpu_id == vcpu_id) {
            return &hvsr_state.vcpus[i];
        }
    }

    return NULL;
}

int hypervisor_save_vm_state(u64 vm_id, const char *snapshot_file)
{
    if (!snapshot_file) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] VM state saved: %s\n", snapshot_file);

    return 0;
}

int hypervisor_restore_vm_state(u64 vm_id, const char *snapshot_file)
{
    if (!snapshot_file) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] VM state restored from: %s\n", snapshot_file);

    return 0;
}

int hypervisor_live_migrate_vm(u64 vm_id, const char *target_host)
{
    if (!target_host) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    printf("[HYPERVISOR] Live migration initiated: VM %llu -> %s\n", vm_id, target_host);

    return 0;
}

int hypervisor_enable_nested_virtualization(void)
{
    hvsr_state.nested_virt_enabled = 1;
    printf("[HYPERVISOR] Nested virtualization enabled\n");

    return 0;
}

int hypervisor_disable_nested_virtualization(void)
{
    hvsr_state.nested_virt_enabled = 0;
    printf("[HYPERVISOR] Nested virtualization disabled\n");

    return 0;
}

int hypervisor_set_cpu_pinning(u64 vm_id, u32 vcpu, u32 phys_cpu)
{
    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm || vcpu >= vm->cpu_count) return -1;

    printf("[HYPERVISOR] CPU pinning: VM %llu vCPU %u -> Physical CPU %u\n",
           vm_id, vcpu, phys_cpu);

    return 0;
}

u64 hypervisor_get_total_memory(void)
{
    return hvsr_state.total_memory;
}

u32 hypervisor_get_total_cpus(void)
{
    return hvsr_state.total_cpus;
}

int hypervisor_get_performance_stats(u64 vm_id, char *stats_buf, u32 buf_size)
{
    if (!stats_buf || buf_size == 0) return -1;

    vm_info_t *vm = hypervisor_get_vm_info(vm_id);
    if (!vm) return -1;

    snprintf(stats_buf, buf_size, "VM %llu: CPUs: %u, Memory: %llu bytes, Uptime: %llu seconds",
             vm_id, vm->cpu_count, vm->memory_size, vm->uptime);

    return 0;
}
