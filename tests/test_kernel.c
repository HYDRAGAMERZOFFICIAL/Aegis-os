#include "test_framework.h"
#include <kernel/process.h>
#include <kernel/memory.h>
#include <kernel/scheduler.h>
#include <kernel/interrupt.h>

extern int pmgr_create_process(const char* name, uint32_t flags);
extern int pmgr_create_thread(int pid, void* entry, void* arg);
extern int pmgr_set_thread_priority(int tid, uint8_t priority);
extern void* mmgr_alloc_page(void);
extern void mmgr_free_page(void* page);
extern int mmgr_enable_aslr(void);
extern int scheduler_init(void);
extern int ied_register_irq(int irq, void* handler);

TEST_SUITE(kernel) {
    printf("\n=== Kernel Module Tests ===\n");

    TEST_CASE(process_creation) {
        int pid = pmgr_create_process("test_process", 0);
        ASSERT_NOT_EQUAL(pid, -1);
    } TEST_END();

    TEST_CASE(thread_creation) {
        int pid = pmgr_create_process("thread_test", 0);
        int tid = pmgr_create_thread(pid, NULL, NULL);
        ASSERT_NOT_EQUAL(tid, -1);
    } TEST_END();

    TEST_CASE(thread_priority) {
        int pid = pmgr_create_process("priority_test", 0);
        int tid = pmgr_create_thread(pid, NULL, NULL);
        int result = pmgr_set_thread_priority(tid, 5);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(memory_allocation) {
        void* page = mmgr_alloc_page();
        ASSERT_NOT_NULL(page);
    } TEST_END();

    TEST_CASE(memory_deallocation) {
        void* page = mmgr_alloc_page();
        if (page) {
            mmgr_free_page(page);
            ASSERT_TRUE(1);
        } else {
            ASSERT_FALSE(1);
        }
    } TEST_END();

    TEST_CASE(aslr_enable) {
        int result = mmgr_enable_aslr();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(scheduler_initialization) {
        int result = scheduler_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(irq_registration) {
        int result = ied_register_irq(0, NULL);
        ASSERT_NOT_EQUAL(result, -1);
    } TEST_END();
}
