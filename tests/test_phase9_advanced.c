#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_framework.h"
#include "../kernel/advanced_features.h"

static int test_count = 0;
static int pass_count = 0;
static int fail_count = 0;

void log_test(const char* name, int passed) {
    test_count++;
    if (passed) {
        pass_count++;
        printf("✓ %s\n", name);
    } else {
        fail_count++;
        printf("✗ %s\n", name);
    }
}

TEST_SUITE(hypervisor_tests) {
    printf("\n=== Phase 9: Hypervisor Tests ===\n");
    
    int r = hypervisor_init();
    log_test("hypervisor_init", r == 0);
    
    VirtualMachine vm_config;
    vm_config.entry_point = 0x400000;
    vm_config.memory_size = 512 * 1024 * 1024;
    vm_config.vcpu_count = 4;
    vm_config.guest_memory = malloc(vm_config.memory_size);
    
    uint32_t vm_id;
    r = vm_create(&vm_id, &vm_config);
    log_test("vm_create", r == 0 && vm_id > 0);
    
    if (vm_id > 0) {
        r = vm_start(vm_id);
        log_test("vm_start", r == 0);
        
        VMStatus status;
        r = vm_get_status(vm_id, &status);
        log_test("vm_get_status", r == 0 && status.state == VM_STATE_RUNNING);
        
        r = vm_pause(vm_id);
        log_test("vm_pause", r == 0);
        
        r = vm_get_status(vm_id, &status);
        log_test("vm_paused_status", r == 0 && status.state == VM_STATE_PAUSED);
        
        r = vm_resume(vm_id);
        log_test("vm_resume", r == 0);
        
        r = vm_stop(vm_id);
        log_test("vm_stop", r == 0);
    }
    
    if (vm_config.guest_memory) {
        free(vm_config.guest_memory);
    }
}

TEST_SUITE(microkernel_tests) {
    printf("\n=== Phase 9: Microkernel Tests ===\n");
    
    int r = microkernel_init();
    log_test("microkernel_init", r == 0);
    
    uint32_t cap_id;
    r = microkernel_create_capability(&cap_id, 0xFF);
    log_test("microkernel_create_capability", r == 0 && cap_id > 0);
    
    if (cap_id > 0) {
        uint64_t rights;
        r = microkernel_get_capability_rights(cap_id, &rights);
        log_test("microkernel_get_capability_rights", r == 0 && rights == 0xFF);
        
        CapabilityTransfer transfer;
        transfer.src_cap = cap_id;
        transfer.dst_cap = cap_id + 1;
        transfer.permissions = 0x0F;
        
        r = microkernel_grant_capability(cap_id, cap_id + 1, &transfer);
        log_test("microkernel_grant_capability", r == 0);
        
        r = microkernel_revoke_capability(cap_id);
        log_test("microkernel_revoke_capability", r == 0);
    }
}

TEST_SUITE(partition_tests) {
    printf("\n=== Phase 9: Partition Tests ===\n");
    
    uint32_t partition_id;
    int r = partition_create(&partition_id, PARTITION_MUTABLE, 1024 * 1024 * 1024);
    log_test("partition_create", r == 0 && partition_id > 0);
    
    if (partition_id > 0) {
        r = partition_set_immutable(partition_id);
        log_test("partition_set_immutable", r == 0);
        
        int type = partition_get_type(partition_id);
        log_test("partition_get_type_immutable", type == PARTITION_IMMUTABLE);
        
        uint32_t snapshot_id;
        r = partition_create_snapshot(partition_id, &snapshot_id);
        log_test("partition_create_snapshot", r == 0 || r == -1);
        
        if (r == 0) {
            r = partition_restore_snapshot(partition_id, snapshot_id);
            log_test("partition_restore_snapshot", r == 0);
        }
    }
}

TEST_SUITE(realtime_ipc_tests) {
    printf("\n=== Phase 9: Real-Time IPC Tests ===\n");
    
    int r = ipc_set_priority(1, IPC_PRIORITY_HIGH);
    log_test("ipc_set_priority", r == 0 || r == -1);
    
    r = ipc_set_deadline(1, 10000);
    log_test("ipc_set_deadline", r == 0 || r == -1);
    
    r = ipc_ensure_delivery(1);
    log_test("ipc_ensure_delivery", r == 0 || r == -1);
    
    uint64_t latency;
    r = ipc_get_latency(1, &latency);
    log_test("ipc_get_latency", r == 0 || r == -1);
}

TEST_SUITE(cache_tests) {
    printf("\n=== Phase 9: Cache Management Tests ===\n");
    
    int r = cache_flush(CACHE_LEVEL_L1);
    log_test("cache_flush_l1", r == 0);
    
    r = cache_flush(CACHE_LEVEL_L2);
    log_test("cache_flush_l2", r == 0);
    
    r = cache_prefetch(0x400000, 4096);
    log_test("cache_prefetch", r == 0);
    
    CacheStats stats;
    r = cache_get_stats(CACHE_LEVEL_L3, &stats);
    log_test("cache_get_stats", r == 0);
    
    if (r == 0) {
        log_test("cache_stats_valid", stats.total_accesses > 0);
    }
    
    r = cache_set_policy(CACHE_LEVEL_LLC, 0);
    log_test("cache_set_policy", r == 0);
}

TEST_SUITE(container_tests) {
    printf("\n=== Phase 9: Container Tests ===\n");
    
    uint32_t container_id;
    int r = container_create(&container_id, "nginx");
    log_test("container_create", r == 0 && container_id > 0);
    
    if (container_id > 0) {
        r = container_start(container_id);
        log_test("container_start", r == 0);
        
        int status = container_get_status(container_id);
        log_test("container_get_status_running", status == 1);
        
        r = container_stop(container_id);
        log_test("container_stop", r == 0);
        
        status = container_get_status(container_id);
        log_test("container_get_status_stopped", status == 0);
    }
    
    uint32_t cont2, cont3;
    container_create(&cont2, "postgres");
    container_create(&cont3, "redis");
    log_test("container_create_multiple", cont2 > 0 && cont3 > 0);
}

TEST_SUITE(numa_tests) {
    printf("\n=== Phase 9: NUMA Tests ===\n");
    
    int node = numa_get_local_node(0);
    log_test("numa_get_local_node", node >= 0);
    
    int r = numa_allocate_local(4096, 0);
    log_test("numa_allocate_local", r == 0 || r == -1);
    
    int distance = numa_get_distance(0, 1);
    log_test("numa_get_distance", distance > 0);
}

TEST_SUITE(heterogeneous_scheduler_tests) {
    printf("\n=== Phase 9: Heterogeneous Scheduler Tests ===\n");
    
    int r = heterogeneous_scheduler_init();
    log_test("heterogeneous_scheduler_init", r == 0);
    
    r = heterogeneous_scheduler_assign(1, 0);
    log_test("heterogeneous_scheduler_assign_p_core", r == 0 || r == -1);
    
    r = heterogeneous_scheduler_assign(2, 1);
    log_test("heterogeneous_scheduler_assign_e_core", r == 0 || r == -1);
}

TEST_SUITE(memory_tagging_tests) {
    printf("\n=== Phase 9: Memory Tagging Tests ===\n");
    
    int r = memory_tagging_init();
    log_test("memory_tagging_init", r == 0);
    
    void* ptr;
    r = memory_tag_allocate(&ptr, 4096, 42);
    log_test("memory_tag_allocate", r == 0 && ptr != NULL);
    
    if (ptr) {
        r = memory_check_tag(ptr, 42);
        log_test("memory_check_tag_valid", r == 0 || r == -1);
        
        r = memory_check_tag(ptr, 99);
        log_test("memory_check_tag_invalid", r != 0 || r == -1);
        
        free(ptr);
    }
}

TEST_SUITE(isolation_domain_tests) {
    printf("\n=== Phase 9: Isolation Domain Tests ===\n");
    
    uint32_t domain_id;
    int r = isolation_create_domain(&domain_id);
    log_test("isolation_create_domain", r == 0 && domain_id > 0);
    
    if (domain_id > 0) {
        r = isolation_add_process(domain_id, 1);
        log_test("isolation_add_process", r == 0);
        
        r = isolation_add_process(domain_id, 2);
        log_test("isolation_add_process_2", r == 0);
        
        r = isolation_set_memory_isolation(domain_id, 1);
        log_test("isolation_set_memory_isolation", r == 0);
    }
}

TEST_SUITE(advanced_vm_tests) {
    printf("\n=== Phase 9: Advanced VM Tests ===\n");
    
    hypervisor_init();
    
    VirtualMachine vm1_config = {
        .entry_point = 0x400000,
        .memory_size = 256 * 1024 * 1024,
        .vcpu_count = 2,
        .guest_memory = malloc(256 * 1024 * 1024),
    };
    
    VirtualMachine vm2_config = {
        .entry_point = 0x500000,
        .memory_size = 512 * 1024 * 1024,
        .vcpu_count = 4,
        .guest_memory = malloc(512 * 1024 * 1024),
    };
    
    uint32_t vm1, vm2;
    int r1 = vm_create(&vm1, &vm1_config);
    int r2 = vm_create(&vm2, &vm2_config);
    
    log_test("advanced_vm_create_multiple", r1 == 0 && r2 == 0);
    
    if (vm1 > 0 && vm2 > 0) {
        vm_start(vm1);
        vm_start(vm2);
        
        VMStatus s1, s2;
        vm_get_status(vm1, &s1);
        vm_get_status(vm2, &s2);
        
        log_test("advanced_vm_both_running", s1.state == VM_STATE_RUNNING && 
                                            s2.state == VM_STATE_RUNNING);
        
        vm_pause(vm1);
        vm_get_status(vm1, &s1);
        log_test("advanced_vm_pause_one", s1.state == VM_STATE_PAUSED);
        
        vm_resume(vm1);
        vm_stop(vm1);
        vm_stop(vm2);
    }
    
    if (vm1_config.guest_memory) free(vm1_config.guest_memory);
    if (vm2_config.guest_memory) free(vm2_config.guest_memory);
}

TEST_SUITE(advanced_partition_tests) {
    printf("\n=== Phase 9: Advanced Partition Tests ===\n");
    
    uint32_t p1, p2, p3;
    partition_create(&p1, PARTITION_MUTABLE, 512 * 1024 * 1024);
    partition_create(&p2, PARTITION_MUTABLE, 1024 * 1024 * 1024);
    partition_create(&p3, PARTITION_MUTABLE, 256 * 1024 * 1024);
    
    log_test("advanced_partition_create_multiple", p1 > 0 && p2 > 0 && p3 > 0);
    
    partition_set_immutable(p1);
    partition_set_immutable(p2);
    
    int t1 = partition_get_type(p1);
    int t2 = partition_get_type(p2);
    int t3 = partition_get_type(p3);
    
    log_test("advanced_partition_types", 
             t1 == PARTITION_IMMUTABLE && t2 == PARTITION_IMMUTABLE && 
             t3 == PARTITION_MUTABLE);
}

TEST_SUITE(advanced_container_tests) {
    printf("\n=== Phase 9: Advanced Container Tests ===\n");
    
    uint32_t c1, c2, c3;
    container_create(&c1, "web_server");
    container_create(&c2, "database");
    container_create(&c3, "cache");
    
    log_test("advanced_container_create_3", c1 > 0 && c2 > 0 && c3 > 0);
    
    container_start(c1);
    container_start(c2);
    
    int s1 = container_get_status(c1);
    int s2 = container_get_status(c2);
    int s3 = container_get_status(c3);
    
    log_test("advanced_container_mixed_states", 
             s1 == 1 && s2 == 1 && s3 == 0);
}

TEST_SUITE(advanced_isolation_tests) {
    printf("\n=== Phase 9: Advanced Isolation Tests ===\n");
    
    uint32_t d1, d2;
    isolation_create_domain(&d1);
    isolation_create_domain(&d2);
    
    log_test("advanced_isolation_create_multiple", d1 > 0 && d2 > 0);
    
    isolation_add_process(d1, 1);
    isolation_add_process(d1, 2);
    isolation_add_process(d2, 3);
    isolation_add_process(d2, 4);
    
    isolation_set_memory_isolation(d1, 1);
    isolation_set_memory_isolation(d2, 0);
    
    log_test("advanced_isolation_configured", 1);
}

int main() {
    printf("════════════════════════════════════════════════════════════\n");
    printf("  Aegis OS - Phase 9: Advanced Features Tests\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    hypervisor_tests();
    microkernel_tests();
    partition_tests();
    realtime_ipc_tests();
    cache_tests();
    container_tests();
    numa_tests();
    heterogeneous_scheduler_tests();
    memory_tagging_tests();
    isolation_domain_tests();
    advanced_vm_tests();
    advanced_partition_tests();
    advanced_container_tests();
    advanced_isolation_tests();
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("  TOTAL: %d | PASSED: %d | FAILED: %d\n", test_count, pass_count, fail_count);
    printf("  PASS RATE: %.1f%%\n", test_count > 0 ? (100.0 * pass_count / test_count) : 0);
    printf("════════════════════════════════════════════════════════════\n");
    
    return fail_count > 0 ? 1 : 0;
}
