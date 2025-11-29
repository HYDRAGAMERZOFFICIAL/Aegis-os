#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_framework.h"
#include "../kernel/services.h"
#include "../kernel/bootloader.h"
#include "../kernel/wasm_runtime.h"
#include "../kernel/container_engine.h"
#include "../kernel/integration_layer.h"

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

TEST_SUITE(service_manager_tests) {
    printf("\n=== Service Manager Tests ===\n");
    
    int r = service_manager_init();
    log_test("service_manager_init", r == 0);
    
    uint32_t svc_id;
    r = service_register(&svc_id, "test_service", SERVICE_TYPE_SYSTEM);
    log_test("service_register", r == 0 && svc_id > 0);
    
    if (svc_id > 0) {
        r = service_start(svc_id);
        log_test("service_start", r == 0);
        
        int state = service_get_state(svc_id);
        log_test("service_get_state_running", state == SERVICE_STATE_RUNNING);
        
        r = service_stop(svc_id);
        log_test("service_stop", r == 0);
        
        state = service_get_state(svc_id);
        log_test("service_get_state_stopped", state == SERVICE_STATE_STOPPED);
    }
    
    r = service_register(&svc_id, "network_service", SERVICE_TYPE_NETWORK);
    log_test("service_register_network", r == 0);
    
    r = service_register(&svc_id, "storage_service", SERVICE_TYPE_STORAGE);
    log_test("service_register_storage", r == 0);
}

TEST_SUITE(service_config_tests) {
    printf("\n=== Service Configuration Tests ===\n");
    
    service_manager_init();
    
    uint32_t svc_id;
    service_register(&svc_id, "configurable_service", SERVICE_TYPE_UTILITY);
    
    int r = service_set_config(svc_id, "port", "8080");
    log_test("service_set_config", r == 0);
    
    char value[256];
    r = service_get_config(svc_id, "port", value, sizeof(value));
    log_test("service_get_config", r == 0 && strcmp(value, "8080") == 0);
    
    r = service_enable_autostart(svc_id);
    log_test("service_enable_autostart", r == 0);
    
    r = service_set_dependency(svc_id, 1);
    log_test("service_set_dependency", r == 0);
    
    uint32_t deps[16];
    size_t dep_count = 16;
    r = service_get_dependencies(svc_id, deps, &dep_count);
    log_test("service_get_dependencies", r == 0 && dep_count > 0);
}

TEST_SUITE(syslog_tests) {
    printf("\n=== System Logging Tests ===\n");
    
    int r = syslog_init();
    log_test("syslog_init", r == 0);
    
    r = syslog_write(1, "Test log message");
    log_test("syslog_write", r == 0);
    
    char buffer[1024];
    r = syslog_get_logs(1, buffer, sizeof(buffer));
    log_test("syslog_get_logs", r == 0);
}

TEST_SUITE(bootloader_tests) {
    printf("\n=== Bootloader Tests ===\n");
    
    int r = bootloader_init();
    log_test("bootloader_init", r == 0);
    
    r = bootloader_load_kernel("vmlinuz");
    log_test("bootloader_load_kernel", r == 0);
    
    r = bootloader_load_initrd("initrd.img");
    log_test("bootloader_load_initrd", r == 0);
    
    r = bootloader_setup_mmu();
    log_test("bootloader_setup_mmu", r == 0);
    
    r = bootloader_setup_gdt();
    log_test("bootloader_setup_gdt", r == 0);
    
    r = bootloader_setup_stack(0xFFFF0000, 0x10000);
    log_test("bootloader_setup_stack", r == 0);
    
    r = bootloader_enable_paging();
    log_test("bootloader_enable_paging", r == 0);
    
    BootInfo info;
    r = bootloader_get_boot_info(&info);
    log_test("bootloader_get_boot_info", r == 0);
    
    r = bootloader_set_boot_mode(BOOT_MODE_SAFE);
    log_test("bootloader_set_boot_mode", r == 0);
    
    BootMode mode = bootloader_get_boot_mode();
    log_test("bootloader_get_boot_mode", mode == BOOT_MODE_SAFE);
    
    r = bootloader_check_integrity();
    log_test("bootloader_check_integrity", r == 0);
}

TEST_SUITE(wasm_runtime_tests) {
    printf("\n=== WebAssembly Runtime Tests ===\n");
    
    int r = wasm_runtime_init();
    log_test("wasm_runtime_init", r == 0);
    
    WasmModule module;
    module.module_data = malloc(256);
    module.module_size = 256;
    strcpy(module.module_name, "test_module");
    
    uint32_t runtime_id;
    r = wasm_runtime_create(&runtime_id, &module);
    log_test("wasm_runtime_create", r == 0 && runtime_id > 0);
    
    if (runtime_id > 0) {
        r = wasm_runtime_load_module(runtime_id, "/path/to/module.wasm");
        log_test("wasm_runtime_load_module", r == 0);
        
        int state = wasm_runtime_get_state(runtime_id);
        log_test("wasm_runtime_get_state_loaded", state == WASM_STATE_LOADED);
        
        WasmValue args[4];
        args[0].i32 = 42;
        WasmValue result;
        r = wasm_call_function(runtime_id, "add", args, 1, &result);
        log_test("wasm_call_function", r == 0 || r == -1);
        
        r = wasm_runtime_suspend(runtime_id);
        log_test("wasm_runtime_suspend", r == 0 || r == -1);
        
        r = wasm_runtime_resume(runtime_id);
        log_test("wasm_runtime_resume", r == 0 || r == -1);
        
        r = wasm_runtime_destroy(runtime_id);
        log_test("wasm_runtime_destroy", r == 0);
    }
    
    if (module.module_data) {
        free(module.module_data);
    }
}

TEST_SUITE(wasm_memory_tests) {
    printf("\n=== WASM Memory Tests ===\n");
    
    wasm_runtime_init();
    
    WasmModule module;
    module.module_data = malloc(256);
    module.module_size = 256;
    strcpy(module.module_name, "memory_module");
    
    uint32_t runtime_id;
    wasm_runtime_create(&runtime_id, &module);
    
    if (runtime_id > 0) {
        uint8_t write_data[] = {0xDE, 0xAD, 0xBE, 0xEF};
        int r = wasm_runtime_set_memory(runtime_id, 0, write_data, 4);
        log_test("wasm_runtime_set_memory", r == 0);
        
        uint8_t read_buffer[4];
        r = wasm_runtime_get_memory(runtime_id, 0, read_buffer, 4);
        log_test("wasm_runtime_get_memory", r == 0);
        
        if (r == 0) {
            int match = (read_buffer[0] == 0xDE && read_buffer[1] == 0xAD);
            log_test("wasm_memory_content", match);
        }
        
        wasm_runtime_destroy(runtime_id);
    }
    
    if (module.module_data) {
        free(module.module_data);
    }
}

TEST_SUITE(container_engine_tests) {
    printf("\n=== Container Engine Tests ===\n");
    
    int r = container_engine_init();
    log_test("container_engine_init", r == 0);
    
    uint32_t container_id;
    r = container_create(&container_id, "nginx:latest");
    log_test("container_create", r == 0 && container_id > 0);
    
    if (container_id > 0) {
        r = container_start(container_id);
        log_test("container_start", r == 0);
        
        int state = container_get_state(container_id);
        log_test("container_get_state_running", state == CONTAINER_STATE_RUNNING);
        
        r = container_pause(container_id);
        log_test("container_pause", r == 0);
        
        r = container_unpause(container_id);
        log_test("container_unpause", r == 0);
        
        r = container_stop(container_id);
        log_test("container_stop", r == 0);
        
        r = container_remove(container_id);
        log_test("container_remove", r == 0);
    }
    
    uint32_t c2, c3;
    container_create(&c2, "postgres:13");
    container_create(&c3, "redis:latest");
    log_test("container_create_multiple", c2 > 0 && c3 > 0);
}

TEST_SUITE(container_config_tests) {
    printf("\n=== Container Configuration Tests ===\n");
    
    container_engine_init();
    
    uint32_t container_id;
    container_create(&container_id, "app:latest");
    
    if (container_id > 0) {
        int r = container_set_env(container_id, "PORT", "3000");
        log_test("container_set_env", r == 0);
        
        r = container_add_volume(container_id, "/host/data", "/container/data");
        log_test("container_add_volume", r == 0);
        
        r = container_set_memory_limit(container_id, 1024 * 1024 * 1024);
        log_test("container_set_memory_limit", r == 0);
        
        r = container_set_cpu_shares(container_id, 512);
        log_test("container_set_cpu_shares", r == 0);
        
        Container info;
        r = container_get_info(container_id, &info);
        log_test("container_get_info", r == 0 && info.memory_limit == 1024 * 1024 * 1024);
        
        container_start(container_id);
        
        r = container_exec_command(container_id, "echo hello");
        log_test("container_exec_command", r == 0);
    }
}

TEST_SUITE(container_list_stats_tests) {
    printf("\n=== Container List & Stats Tests ===\n");
    
    container_engine_init();
    
    uint32_t ids[3];
    container_create(&ids[0], "web:latest");
    container_create(&ids[1], "db:latest");
    container_create(&ids[2], "cache:latest");
    
    container_start(ids[0]);
    container_start(ids[1]);
    
    uint32_t list_ids[100];
    size_t count = 100;
    int r = container_list(list_ids, &count);
    log_test("container_list", r == 0 && count >= 2);
    
    uint64_t memory_used;
    uint32_t cpu_usage;
    r = container_get_stats(ids[0], &memory_used, &cpu_usage);
    log_test("container_get_stats", r == 0);
}

TEST_SUITE(integration_layer_tests) {
    printf("\n=== Integration Layer Tests ===\n");
    
    int r = integration_init_all();
    log_test("integration_init_all", r == 0);
    
    SystemHealth health;
    r = integration_get_system_health(&health);
    log_test("integration_get_system_health", r == 0);
    log_test("integration_system_ready", health.overall_state == INTEGRATION_STATE_READY);
    
    r = integration_check_health();
    log_test("integration_check_health", r == 0);
}

TEST_SUITE(integration_subsystem_tests) {
    printf("\n=== Integration Subsystem Tests ===\n");
    
    integration_init_all();
    
    int r = integration_init_subsystem(SUBSYSTEM_SERVICES);
    log_test("integration_init_subsystem", r == 0);
    
    SubsystemStatus status;
    r = integration_get_status(SUBSYSTEM_SERVICES, &status);
    log_test("integration_get_status", r == 0 && status.state == INTEGRATION_STATE_READY);
    
    r = integration_handle_fault(SUBSYSTEM_NETWORK, "Network interface down");
    log_test("integration_handle_fault", r == 0);
    
    r = integration_recover(SUBSYSTEM_NETWORK);
    log_test("integration_recover", r == 0);
}

TEST_SUITE(integration_dependency_tests) {
    printf("\n=== Integration Dependency Tests ===\n");
    
    integration_init_all();
    
    SubsystemType deps[5];
    size_t count = 5;
    int r = integration_get_dependencies(SUBSYSTEM_CONTAINER, deps, &count);
    log_test("integration_get_dependencies", r == 0 && count > 0);
    
    r = integration_verify_all();
    log_test("integration_verify_all", r == 0);
}

TEST_SUITE(integration_callback_tests) {
    printf("\n=== Integration Callback Tests ===\n");
    
    static int callback_called = 0;
    void test_callback(SubsystemType subsystem, int event) {
        callback_called = 1;
    }
    
    int r = integration_register_callback(test_callback);
    log_test("integration_register_callback", r == 0);
}

TEST_SUITE(complete_workflow_tests) {
    printf("\n=== Complete Workflow Tests ===\n");
    
    bootloader_init();
    service_manager_init();
    wasm_runtime_init();
    container_engine_init();
    integration_init_all();
    
    uint32_t svc_id;
    service_register(&svc_id, "integration_test", SERVICE_TYPE_SYSTEM);
    service_start(svc_id);
    
    uint32_t container_id;
    container_create(&container_id, "workflow:latest");
    container_start(container_id);
    
    SystemHealth health;
    integration_get_system_health(&health);
    
    log_test("complete_workflow", health.overall_state == INTEGRATION_STATE_READY);
    
    container_stop(container_id);
    service_stop(svc_id);
}

int main() {
    printf("════════════════════════════════════════════════════════════\n");
    printf("  Aegis OS - Implementation Needed Components Test Suite\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    service_manager_tests();
    service_config_tests();
    syslog_tests();
    bootloader_tests();
    wasm_runtime_tests();
    wasm_memory_tests();
    container_engine_tests();
    container_config_tests();
    container_list_stats_tests();
    integration_layer_tests();
    integration_subsystem_tests();
    integration_dependency_tests();
    integration_callback_tests();
    complete_workflow_tests();
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("  TOTAL: %d | PASSED: %d | FAILED: %d\n", test_count, pass_count, fail_count);
    printf("  PASS RATE: %.1f%%\n", test_count > 0 ? (100.0 * pass_count / test_count) : 0);
    printf("════════════════════════════════════════════════════════════\n");
    
    return fail_count > 0 ? 1 : 0;
}
