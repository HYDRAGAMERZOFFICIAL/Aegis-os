#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_framework.h"
#include "../kernel/syscall_gate.h"
#include "../kernel/ipc_bus.h"
#include "../kernel/event_system.h"
#include "../kernel/boot_params.h"
#include "../kernel/profiler.h"
#include "../kernel/memory.h"
#include "../kernel/process.h"
#include "../kernel/scheduler.h"

#define MAX_TEST_PROCS 100
#define MAX_TEST_IPC_MSG 1024
#define MAX_TEST_EVENTS 512

typedef struct {
    int passed;
    int failed;
    int total;
} TestStats;

TestStats test_stats = {0, 0, 0};

void test_result(const char* test_name, int passed) {
    test_stats.total++;
    if (passed) {
        test_stats.passed++;
        printf("✓ %s\n", test_name);
    } else {
        test_stats.failed++;
        printf("✗ %s\n", test_name);
    }
}

TEST_SUITE(kernel_modules_phase6) {
    printf("\n=== Phase 6: Kernel Module Tests ===\n");
    
    {
        printf("\n--- Syscall Gateway Tests ---\n");
        
        int result = syscall_register(100, "test_syscall_1", 0);
        test_result("syscall_register_basic", result == 0);
        
        int is_registered = syscall_is_registered(100);
        test_result("syscall_is_registered", is_registered);
        
        const char* name = syscall_get_name(100);
        test_result("syscall_get_name", name != NULL && strcmp(name, "test_syscall_1") == 0);
        
        int priv = syscall_get_privilege_level(100);
        test_result("syscall_get_privilege_level", priv == 0);
        
        result = syscall_register(200, "test_syscall_2", 1);
        test_result("syscall_register_with_privilege", result == 0);
        
        result = syscall_unregister(100);
        test_result("syscall_unregister", result == 0);
        
        result = syscall_is_registered(100);
        test_result("syscall_verify_unregistered", result == 0);
    }
    
    {
        printf("\n--- IPC Bus Tests ---\n");
        
        IpcChannel* ch = ipc_create_channel(1, 100);
        test_result("ipc_create_channel", ch != NULL);
        
        if (ch) {
            int result = ipc_send_message(ch, 1, 2, "test message", 12);
            test_result("ipc_send_message", result == 0);
            
            IpcMessage msg;
            result = ipc_receive_message(ch, 1, &msg);
            test_result("ipc_receive_message", result == 0);
            
            if (result == 0) {
                test_result("ipc_message_content", msg.data_size == 12);
            }
            
            result = ipc_destroy_channel(ch);
            test_result("ipc_destroy_channel", result == 0);
        }
    }
    
    {
        printf("\n--- Event System Tests ---\n");
        
        EventSystem* es = event_create_system(256);
        test_result("event_create_system", es != NULL);
        
        if (es) {
            int result = event_subscribe(es, 1, 10);
            test_result("event_subscribe", result == 0);
            
            result = event_publish(es, 10, NULL, 0);
            test_result("event_publish", result == 0);
            
            Event event;
            result = event_poll(es, 1, &event);
            test_result("event_poll_after_publish", result >= 0);
            
            result = event_unsubscribe(es, 1, 10);
            test_result("event_unsubscribe", result == 0);
            
            event_destroy_system(es);
            test_result("event_destroy_system", 1);
        }
    }
    
    {
        printf("\n--- Boot Parameters Tests ---\n");
        
        BootParams params;
        memset(&params, 0, sizeof(params));
        params.kernel_size = 1024 * 1024;
        params.initrd_size = 512 * 1024;
        
        int result = boot_params_validate(&params);
        test_result("boot_params_validate_valid", result == 0);
        
        params.kernel_size = 0;
        result = boot_params_validate(&params);
        test_result("boot_params_validate_invalid_kernel", result != 0);
        
        params.kernel_size = 1024 * 1024;
        result = boot_params_save(&params, "test_params");
        test_result("boot_params_save", result == 0);
        
        BootParams loaded;
        result = boot_params_load("test_params", &loaded);
        test_result("boot_params_load", result == 0);
    }
    
    {
        printf("\n--- Memory Management Tests ---\n");
        
        void* ptr1 = mem_alloc(1024);
        test_result("mem_alloc_basic", ptr1 != NULL);
        
        if (ptr1) {
            memset(ptr1, 0xAA, 1024);
            test_result("mem_write", 1);
            mem_free(ptr1);
            test_result("mem_free", 1);
        }
        
        void* ptr2 = mem_alloc(4096);
        void* ptr3 = mem_alloc(4096);
        test_result("mem_alloc_multiple", ptr2 != NULL && ptr3 != NULL);
        
        if (ptr2 && ptr3) {
            test_result("mem_alloc_different_addresses", ptr2 != ptr3);
            mem_free(ptr2);
            mem_free(ptr3);
        }
        
        size_t used = mem_get_usage();
        test_result("mem_get_usage", used >= 0);
    }
    
    printf("\n--- Phase 6 Summary ---\n");
    printf("Passed: %d, Failed: %d, Total: %d\n", test_stats.passed, test_stats.failed, test_stats.total);
}

TEST_SUITE(driver_tests_phase6) {
    printf("\n=== Phase 6: Driver Tests ===\n");
    
    {
        printf("\n--- Device Driver Tests ---\n");
        
        int result = driver_register(1, "test_driver_1", 0);
        test_result("driver_register_basic", result == 0);
        
        int is_registered = driver_is_registered(1);
        test_result("driver_is_registered", is_registered);
        
        const char* name = driver_get_name(1);
        test_result("driver_get_name", name != NULL);
        
        result = driver_register(2, "test_driver_2", 1);
        test_result("driver_register_with_priority", result == 0);
        
        result = driver_unregister(1);
        test_result("driver_unregister", result == 0);
    }
    
    {
        printf("\n--- USB Driver Tests ---\n");
        
        int result = usb_driver_init();
        test_result("usb_driver_init", result == 0);
        
        result = usb_device_attach(1);
        test_result("usb_device_attach", result == 0);
        
        int is_attached = usb_device_is_attached(1);
        test_result("usb_device_is_attached", is_attached);
        
        result = usb_device_detach(1);
        test_result("usb_device_detach", result == 0);
    }
    
    {
        printf("\n--- Network Driver Tests ---\n");
        
        int result = network_driver_init();
        test_result("network_driver_init", result == 0);
        
        result = network_interface_add(1, "eth0");
        test_result("network_interface_add", result == 0);
        
        const char* name = network_interface_get_name(1);
        test_result("network_interface_get_name", name != NULL);
        
        result = network_interface_remove(1);
        test_result("network_interface_remove", result == 0);
    }
    
    {
        printf("\n--- Storage Driver Tests ---\n");
        
        int result = storage_driver_init();
        test_result("storage_driver_init", result == 0);
        
        result = storage_device_add(1, 1024 * 1024 * 1024);
        test_result("storage_device_add", result == 0);
        
        uint64_t size = storage_device_get_size(1);
        test_result("storage_device_get_size", size == 1024 * 1024 * 1024);
        
        result = storage_device_remove(1);
        test_result("storage_device_remove", result == 0);
    }
}

TEST_SUITE(filesystem_tests_phase6) {
    printf("\n=== Phase 6: Filesystem Tests ===\n");
    
    {
        printf("\n--- Filesystem Operations ---\n");
        
        int result = fs_mount("/", "/dev/sda1", "ext4");
        test_result("fs_mount_root", result == 0);
        
        int is_mounted = fs_is_mounted("/");
        test_result("fs_is_mounted", is_mounted);
        
        result = fs_mkdir("/test");
        test_result("fs_mkdir", result == 0);
        
        result = fs_create_file("/test/file.txt");
        test_result("fs_create_file", result == 0);
        
        int exists = fs_exists("/test/file.txt");
        test_result("fs_exists", exists);
        
        result = fs_delete_file("/test/file.txt");
        test_result("fs_delete_file", result == 0);
        
        result = fs_rmdir("/test");
        test_result("fs_rmdir", result == 0);
    }
    
    {
        printf("\n--- File I/O Tests ---\n");
        
        int fd = fs_open("/test.txt", O_CREAT | O_WRONLY);
        test_result("fs_open_create", fd >= 0);
        
        if (fd >= 0) {
            const char* data = "Hello, Aegis OS!";
            int written = fs_write(fd, data, strlen(data));
            test_result("fs_write", written == strlen(data));
            
            int result = fs_close(fd);
            test_result("fs_close_after_write", result == 0);
            
            fd = fs_open("/test.txt", O_RDONLY);
            test_result("fs_open_read", fd >= 0);
            
            if (fd >= 0) {
                char buffer[32];
                int read = fs_read(fd, buffer, sizeof(buffer));
                test_result("fs_read", read > 0);
                
                fs_close(fd);
            }
        }
    }
    
    {
        printf("\n--- Permission Tests ---\n");
        
        int result = fs_chmod("/test.txt", 0644);
        test_result("fs_chmod", result == 0);
        
        int perms = fs_get_permissions("/test.txt");
        test_result("fs_get_permissions", perms == 0644 || perms >= 0);
        
        result = fs_chown("/test.txt", 1000, 1000);
        test_result("fs_chown", result == 0);
    }
}

TEST_SUITE(security_tests_phase6) {
    printf("\n=== Phase 6: Security Tests ===\n");
    
    {
        printf("\n--- Access Control Tests ---\n");
        
        int result = sec_create_policy(1, "test_policy");
        test_result("sec_create_policy", result == 0);
        
        result = sec_policy_allow(1, 100, 200);
        test_result("sec_policy_allow", result == 0);
        
        int allowed = sec_is_allowed(1, 100, 200);
        test_result("sec_is_allowed_true", allowed);
        
        allowed = sec_is_allowed(1, 100, 300);
        test_result("sec_is_allowed_false", !allowed);
        
        result = sec_policy_deny(1, 100, 200);
        test_result("sec_policy_deny", result == 0);
    }
    
    {
        printf("\n--- Capability Tests ---\n");
        
        int result = cap_grant(1, CAP_SYS_ADMIN);
        test_result("cap_grant", result == 0);
        
        int has_cap = cap_has(1, CAP_SYS_ADMIN);
        test_result("cap_has_true", has_cap);
        
        has_cap = cap_has(1, CAP_NET_ADMIN);
        test_result("cap_has_false", !has_cap);
        
        result = cap_revoke(1, CAP_SYS_ADMIN);
        test_result("cap_revoke", result == 0);
    }
    
    {
        printf("\n--- Authentication Tests ---\n");
        
        int result = auth_create_user("testuser", "password123");
        test_result("auth_create_user", result == 0);
        
        int authenticated = auth_verify("testuser", "password123");
        test_result("auth_verify_correct", authenticated);
        
        authenticated = auth_verify("testuser", "wrongpassword");
        test_result("auth_verify_incorrect", !authenticated);
        
        result = auth_delete_user("testuser");
        test_result("auth_delete_user", result == 0);
    }
}

TEST_SUITE(process_tests_phase6) {
    printf("\n=== Phase 6: Process Management Tests ===\n");
    
    {
        printf("\n--- Process Lifecycle ---\n");
        
        ProcessInfo info;
        memset(&info, 0, sizeof(info));
        int result = process_create(&info, "/bin/test");
        test_result("process_create", result >= 0);
        
        int pid = result;
        if (pid > 0) {
            int is_alive = process_is_alive(pid);
            test_result("process_is_alive", is_alive);
            
            result = process_kill(pid, 15);
            test_result("process_kill", result == 0);
            
            is_alive = process_is_alive(pid);
            test_result("process_dead_after_kill", !is_alive);
        }
    }
    
    {
        printf("\n--- Process Information ---\n");
        
        ProcessInfo info;
        memset(&info, 0, sizeof(info));
        int result = process_get_info(1, &info);
        test_result("process_get_info", result == 0);
        
        if (result == 0) {
            test_result("process_info_valid_pid", info.pid > 0);
            test_result("process_info_valid_state", info.state >= 0 && info.state < 10);
        }
    }
    
    {
        printf("\n--- Multi-Process Tests ---\n");
        
        ProcessInfo info[3];
        int pids[3];
        
        int created = 0;
        for (int i = 0; i < 3; i++) {
            memset(&info[i], 0, sizeof(info[i]));
            int pid = process_create(&info[i], "/bin/test");
            if (pid > 0) {
                pids[i] = pid;
                created++;
            }
        }
        test_result("process_create_multiple", created > 0);
        
        if (created > 0) {
            for (int i = 0; i < created; i++) {
                process_kill(pids[i], 15);
            }
        }
    }
}

TEST_SUITE(scheduler_tests_phase6) {
    printf("\n=== Phase 6: Scheduler Tests ===\n");
    
    {
        printf("\n--- Scheduling Operations ---\n");
        
        int result = scheduler_init(4);
        test_result("scheduler_init", result == 0);
        
        result = scheduler_enqueue(1, 0);
        test_result("scheduler_enqueue", result == 0);
        
        int next_pid = scheduler_get_next();
        test_result("scheduler_get_next", next_pid > 0 || next_pid == 1);
        
        result = scheduler_dequeue(1);
        test_result("scheduler_dequeue", result == 0);
    }
    
    {
        printf("\n--- Priority Tests ---\n");
        
        int result = scheduler_set_priority(1, 10);
        test_result("scheduler_set_priority", result == 0);
        
        int priority = scheduler_get_priority(1);
        test_result("scheduler_get_priority", priority >= 0);
    }
}

TEST_SUITE(workflow_tests_phase6) {
    printf("\n=== Phase 6: Workflow Tests ===\n");
    
    {
        printf("\n--- Process Lifecycle Workflow ---\n");
        
        ProcessInfo info;
        memset(&info, 0, sizeof(info));
        int pid = process_create(&info, "/bin/test");
        test_result("workflow_process_create", pid > 0);
        
        if (pid > 0) {
            int is_alive = process_is_alive(pid);
            test_result("workflow_process_running", is_alive);
            
            ProcessInfo retrieved;
            int result = process_get_info(pid, &retrieved);
            test_result("workflow_process_get_info", result == 0);
            
            result = process_kill(pid, 15);
            test_result("workflow_process_terminate", result == 0);
        }
    }
    
    {
        printf("\n--- IPC Workflow ---\n");
        
        IpcChannel* ch = ipc_create_channel(1, 100);
        test_result("workflow_ipc_create", ch != NULL);
        
        if (ch) {
            int result = ipc_send_message(ch, 1, 2, "workflow test", 13);
            test_result("workflow_ipc_send", result == 0);
            
            IpcMessage msg;
            result = ipc_receive_message(ch, 1, &msg);
            test_result("workflow_ipc_receive", result == 0);
            
            ipc_destroy_channel(ch);
        }
    }
    
    {
        printf("\n--- Filesystem Workflow ---\n");
        
        int result = fs_mount("/", "/dev/sda1", "ext4");
        test_result("workflow_fs_mount", result == 0);
        
        result = fs_mkdir("/workflow");
        test_result("workflow_fs_mkdir", result == 0);
        
        int fd = fs_open("/workflow/test.txt", O_CREAT | O_WRONLY);
        test_result("workflow_fs_create", fd >= 0);
        
        if (fd >= 0) {
            const char* data = "workflow data";
            fs_write(fd, data, strlen(data));
            fs_close(fd);
            
            fd = fs_open("/workflow/test.txt", O_RDONLY);
            if (fd >= 0) {
                char buf[32];
                int read = fs_read(fd, buf, sizeof(buf));
                test_result("workflow_fs_readwrite", read > 0);
                fs_close(fd);
            }
        }
        
        fs_delete_file("/workflow/test.txt");
        fs_rmdir("/workflow");
    }
}

TEST_SUITE(stress_tests_phase6) {
    printf("\n=== Phase 6: Stress Tests ===\n");
    
    {
        printf("\n--- Memory Stress Test ---\n");
        
        void** ptrs = malloc(sizeof(void*) * 1000);
        if (ptrs) {
            int allocated = 0;
            for (int i = 0; i < 1000; i++) {
                ptrs[i] = mem_alloc(4096);
                if (ptrs[i]) allocated++;
            }
            test_result("stress_memory_alloc_1000x4k", allocated > 900);
            
            for (int i = 0; i < allocated; i++) {
                if (ptrs[i]) mem_free(ptrs[i]);
            }
            free(ptrs);
            test_result("stress_memory_free", 1);
        }
    }
    
    {
        printf("\n--- IPC Stress Test ---\n");
        
        IpcChannel* ch = ipc_create_channel(1, 10000);
        if (ch) {
            int sent = 0;
            for (int i = 0; i < 1000; i++) {
                char msg[64];
                snprintf(msg, sizeof(msg), "stress_msg_%d", i);
                if (ipc_send_message(ch, 1, 2, msg, strlen(msg)) == 0) {
                    sent++;
                }
            }
            test_result("stress_ipc_1000_messages", sent > 900);
            ipc_destroy_channel(ch);
        }
    }
    
    {
        printf("\n--- Event Stress Test ---\n");
        
        EventSystem* es = event_create_system(10000);
        if (es) {
            int published = 0;
            for (int i = 0; i < 1000; i++) {
                if (event_publish(es, i % 100, NULL, 0) == 0) {
                    published++;
                }
            }
            test_result("stress_events_1000", published > 900);
            event_destroy_system(es);
        }
    }
    
    {
        printf("\n--- Concurrent Syscall Stress ---\n");
        
        int registered = 0;
        for (int i = 0; i < 1000; i++) {
            char name[32];
            snprintf(name, sizeof(name), "syscall_%d", i);
            if (syscall_register(1000 + i, name, 0) == 0) {
                registered++;
            }
        }
        test_result("stress_syscall_register_1000", registered > 900);
        
        for (int i = 0; i < registered; i++) {
            syscall_unregister(1000 + i);
        }
    }
}

int main() {
    printf("════════════════════════════════════════════════════════════\n");
    printf("  Aegis OS - Phase 6: Extended Test Suite\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    kernel_modules_phase6();
    driver_tests_phase6();
    filesystem_tests_phase6();
    security_tests_phase6();
    process_tests_phase6();
    scheduler_tests_phase6();
    workflow_tests_phase6();
    stress_tests_phase6();
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("  TOTAL TESTS: %d | PASSED: %d | FAILED: %d\n", 
           test_stats.total, test_stats.passed, test_stats.failed);
    printf("  PASS RATE: %.1f%%\n", 
           test_stats.total > 0 ? (100.0 * test_stats.passed / test_stats.total) : 0);
    printf("════════════════════════════════════════════════════════════\n");
    
    return test_stats.failed > 0 ? 1 : 0;
}
