#include <stdio.h>
#include <kernel/profiler.h>
#include "test_framework.h"

static int setup_profiler_test(void)
{
    profiler_init();
    profiler_clear();
    return 0;
}

static int test_profiler_record_event(void)
{
    profiler_record_event("syscall", 500);
    profiler_record_event("context_switch", 1000);
    profiler_record_event("page_fault", 2000);
    
    ASSERT_EQ(profiler_sample_count(), 3);
    return 0;
}

static int test_profiler_get_average(void)
{
    profiler_record_event("operation", 100);
    profiler_record_event("operation", 200);
    profiler_record_event("operation", 300);
    
    uint64_t avg = profiler_get_average_duration("operation");
    ASSERT_EQ(avg, 200);
    return 0;
}

static int test_profiler_get_max_min(void)
{
    profiler_record_event("test_op", 150);
    profiler_record_event("test_op", 500);
    profiler_record_event("test_op", 250);
    
    uint32_t max = profiler_get_max_duration("test_op");
    uint32_t min = profiler_get_min_duration("test_op");
    
    ASSERT_EQ(max, 500);
    ASSERT_EQ(min, 150);
    return 0;
}

static int test_profiler_start_end(void)
{
    profiler_start("timed_operation");
    
    for (volatile int i = 0; i < 1000; i++) {
        __asm__ __volatile__ ("nop");
    }
    
    profiler_end("timed_operation");
    
    uint64_t avg = profiler_get_average_duration("timed_operation");
    ASSERT_GT(avg, 0);
    return 0;
}

void run_profiler_tests(void)
{
    printf("\n=== Profiler Tests ===\n");
    
    TEST_SUITE("Profiler", setup_profiler_test, NULL,
        TEST(test_profiler_record_event),
        TEST(test_profiler_get_average),
        TEST(test_profiler_get_max_min),
        TEST(test_profiler_start_end)
    );
}
