#include "test_framework.h"
#include <kernel/process.h>
#include <kernel/memory.h>
#include <kernel/scheduler.h>
#include <kernel/interrupt.h>

TEST_SUITE(kernel) {
    printf("\n=== Kernel Module Tests ===\n");
    mmgr_init();

    TEST_CASE(process_creation) {
        process_t *proc = pmgr_create_process("test_process", NULL, 10);
        ASSERT_TRUE(proc != NULL);
    } TEST_END();

    TEST_CASE(thread_creation) {
        process_t *proc = pmgr_create_process("thread_test", NULL, 10);
        thread_t *th = pmgr_create_thread(proc ? proc->pid : 1, NULL, NULL);
        ASSERT_TRUE(th != NULL);
    } TEST_END();

    TEST_CASE(thread_priority) {
        process_t *proc = pmgr_create_process("priority_test", NULL, 10);
        thread_t *th = pmgr_create_thread(proc ? proc->pid : 1, NULL, NULL);
        int result = pmgr_set_thread_priority(th ? th->tid : 1, 5);
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
        address_space_t *as = mmgr_create_address_space();
        int result = mmgr_enable_aslr(as);
        ASSERT_EQUAL(result, 0);
        mmgr_destroy_address_space(as);
    } TEST_END();

    TEST_CASE(scheduler_initialization) {
        int result = scheduler_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(irq_registration) {
        int result = ied_register_irq(0, NULL, NULL, IRQ_TYPE_EDGE);
        ASSERT_NOT_EQUAL(result, -1);
    } TEST_END();
}
