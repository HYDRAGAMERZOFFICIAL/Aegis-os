#include <stdio.h>
#include <string.h>
#include <kernel/syscalls.h>
#include <kernel/ipc_bus.h>
#include <kernel/event_system.h>
#include <kernel/boot_params.h>
#include "test_framework.h"

static int setup_syscall_test(void)
{
    syscall_gate_init();
    return 0;
}

static int teardown_syscall_test(void)
{
    return 0;
}

static int test_syscall_register(void)
{
    int dummy_handler = 42;
    int result = syscall_register(0, (syscall_handler_t)(intptr_t)dummy_handler, 
                                  "test_syscall", 0, 2);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(syscall_is_registered(0), 1);
    ASSERT_STREQ(syscall_get_name(0), "test_syscall");
    ASSERT_EQ(syscall_get_privilege_level(0), 0);
    return 0;
}

static int test_syscall_unregister(void)
{
    int dummy_handler = 42;
    syscall_register(1, (syscall_handler_t)(intptr_t)dummy_handler, "test", 0, 1);
    int result = syscall_unregister(1);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(syscall_is_registered(1), 0);
    return 0;
}

static int test_syscall_validate_privilege(void)
{
    int dummy_handler = 42;
    syscall_register(2, (syscall_handler_t)(intptr_t)dummy_handler, "admin_call", 2, 1);
    
    int result_user = syscall_validate_privilege(2, 0);
    int result_admin = syscall_validate_privilege(2, 2);
    
    ASSERT_EQ(result_user, 0);
    ASSERT_EQ(result_admin, 1);
    return 0;
}

static int test_syscall_get_count(void)
{
    int dummy_handler = 42;
    syscall_register(10, (syscall_handler_t)(intptr_t)dummy_handler, "test1", 0, 1);
    syscall_register(11, (syscall_handler_t)(intptr_t)dummy_handler, "test2", 0, 1);
    
    int count = syscall_get_count();
    ASSERT_GTE(count, 2);
    return 0;
}

static int setup_ipc_test(void)
{
    ipc_bus_init();
    return 0;
}

static int teardown_ipc_test(void)
{
    return 0;
}

static int test_ipc_register_route(void)
{
    int result = ipc_bus_register_route(0, 1, 5);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(ipc_bus_is_route_available(0, 1), 1);
    return 0;
}

static int test_ipc_send_receive_message(void)
{
    ipc_bus_register_route(0, 1, 5);
    
    ipc_message_t msg = {
        .source_id = 0,
        .dest_id = 1,
        .priority = 5,
        .msg_type = 1,
        .msg_id = 100,
        .payload_size = 10
    };
    
    int result = ipc_bus_send_message(&msg);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(ipc_bus_get_queue_size(1), 1);
    
    ipc_message_t recv_msg;
    int received = ipc_bus_receive_message(1, &recv_msg);
    ASSERT_EQ(received, 1);
    ASSERT_EQ(recv_msg.msg_id, 100);
    ASSERT_EQ(ipc_bus_get_queue_size(1), 0);
    return 0;
}

static int test_ipc_queue_overflow(void)
{
    ipc_bus_register_route(2, 3, 5);
    
    ipc_message_t msg;
    msg.source_id = 2;
    msg.dest_id = 3;
    
    int sent = 0;
    for (int i = 0; i < IPC_MAX_QUEUE_SIZE + 10; i++) {
        msg.msg_id = i;
        if (ipc_bus_send_message(&msg) == 0) {
            sent++;
        }
    }
    
    ASSERT_EQ(sent, IPC_MAX_QUEUE_SIZE);
    ASSERT_GT(ipc_bus_get_dropped_messages(3), 0);
    return 0;
}

static int test_ipc_clear_queue(void)
{
    ipc_bus_register_route(4, 5, 5);
    
    ipc_message_t msg;
    msg.source_id = 4;
    msg.dest_id = 5;
    
    for (int i = 0; i < 5; i++) {
        msg.msg_id = i;
        ipc_bus_send_message(&msg);
    }
    
    ASSERT_EQ(ipc_bus_get_queue_size(5), 5);
    
    ipc_bus_clear_queue(5);
    ASSERT_EQ(ipc_bus_get_queue_size(5), 0);
    return 0;
}

static int event_callback_called = 0;

static int test_event_callback(const kernel_event_t *event, void *context)
{
    event_callback_called = 1;
    return 0;
}

static int setup_event_test(void)
{
    event_system_init();
    event_callback_called = 0;
    return 0;
}

static int test_event_subscribe_publish(void)
{
    int result = event_subscribe(1, EVENT_PROCESS_CREATED, test_event_callback, NULL);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(event_get_subscriber_count(EVENT_PROCESS_CREATED), 1);
    
    kernel_event_t event;
    event.event_type = EVENT_PROCESS_CREATED;
    event.event_id = 1;
    event.timestamp = 12345;
    event.source_id = 100;
    event.data_size = 0;
    
    int handled = event_publish(&event);
    ASSERT_GT(handled, 0);
    ASSERT_EQ(event_callback_called, 1);
    return 0;
}

static int test_event_unsubscribe(void)
{
    event_subscribe(2, EVENT_PROCESS_DESTROYED, test_event_callback, NULL);
    ASSERT_EQ(event_get_subscriber_count(EVENT_PROCESS_DESTROYED), 1);
    
    int result = event_unsubscribe(2, EVENT_PROCESS_DESTROYED);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(event_get_subscriber_count(EVENT_PROCESS_DESTROYED), 0);
    return 0;
}

static int test_event_multiple_subscribers(void)
{
    event_subscribe(10, EVENT_MEMORY_ALLOCATED, test_event_callback, NULL);
    event_subscribe(11, EVENT_MEMORY_ALLOCATED, test_event_callback, NULL);
    event_subscribe(12, EVENT_MEMORY_ALLOCATED, test_event_callback, NULL);
    
    ASSERT_EQ(event_get_subscriber_count(EVENT_MEMORY_ALLOCATED), 3);
    return 0;
}

static int test_event_get_count(void)
{
    kernel_event_t event;
    event.event_type = EVENT_IRQ_RECEIVED;
    event.timestamp = 0;
    
    for (int i = 0; i < 10; i++) {
        event_publish(&event);
    }
    
    ASSERT_EQ(event_get_count(EVENT_IRQ_RECEIVED), 10);
    return 0;
}

static int test_boot_params_memory(void)
{
    multiboot_info_t info;
    memset(&info, 0, sizeof(info));
    
    info.flags = MULTIBOOT_INFO_MEMORY;
    info.mem_lower = 512;
    info.mem_upper = 8192;
    
    boot_params_init(&info);
    
    ASSERT_EQ(boot_get_mem_lower(), 512);
    ASSERT_EQ(boot_get_mem_upper(), 8192);
    ASSERT_EQ(boot_get_total_memory(), (512 + 8192) * 1024);
    return 0;
}

static int test_boot_params_cmdline(void)
{
    multiboot_info_t info;
    memset(&info, 0, sizeof(info));
    
    info.flags = MULTIBOOT_INFO_CMDLINE;
    info.cmdline = (uint32_t)"root=/dev/sda1 debug=1";
    
    boot_params_init(&info);
    
    const char *cmdline = boot_get_cmdline();
    ASSERT_STREQ(cmdline, "root=/dev/sda1 debug=1");
    ASSERT_EQ(boot_has_param("debug"), 1);
    ASSERT_EQ(boot_has_param("verbose"), 0);
    return 0;
}

static int test_boot_params_bootloader(void)
{
    multiboot_info_t info;
    memset(&info, 0, sizeof(info));
    
    info.flags = MULTIBOOT_INFO_BOOTLOADER_NAME;
    info.boot_loader_name = (uint32_t)"GRUB 2.0";
    
    boot_params_init(&info);
    
    const char *loader = boot_get_bootloader_name();
    ASSERT_STREQ(loader, "GRUB 2.0");
    return 0;
}

void test_integration_main(void)
{
    printf("\n=== Kernel Integration Tests ===\n");
    
    TEST_SUITE("Syscall Gate", setup_syscall_test, teardown_syscall_test,
        TEST(test_syscall_register),
        TEST(test_syscall_unregister),
        TEST(test_syscall_validate_privilege),
        TEST(test_syscall_get_count)
    );
    
    TEST_SUITE("IPC Bus", setup_ipc_test, teardown_ipc_test,
        TEST(test_ipc_register_route),
        TEST(test_ipc_send_receive_message),
        TEST(test_ipc_queue_overflow),
        TEST(test_ipc_clear_queue)
    );
    
    TEST_SUITE("Event System", setup_event_test, NULL,
        TEST(test_event_subscribe_publish),
        TEST(test_event_unsubscribe),
        TEST(test_event_multiple_subscribers),
        TEST(test_event_get_count)
    );
    
    TEST_SUITE("Boot Parameters", NULL, NULL,
        TEST(test_boot_params_memory),
        TEST(test_boot_params_cmdline),
        TEST(test_boot_params_bootloader)
    );
}
