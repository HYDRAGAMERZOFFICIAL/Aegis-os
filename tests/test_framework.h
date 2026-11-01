#ifndef __TEST_FRAMEWORK_H__
#define __TEST_FRAMEWORK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    int passed;
    int failed;
    int total;
    char* current_test;
} test_context_t;

static test_context_t test_ctx = {0, 0, 0, NULL};

#define TEST_SUITE(name) \
    void name(void); \
    void run_##name##_tests(void); \
    void run_##name##_tests(void) { name(); } \
    void name(void)

#define TEST_CASE(name) \
    do { \
        test_ctx.current_test = #name; \
        printf("  [*] Running: %s\n", test_ctx.current_test);

#define TEST_END() \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            printf("    [FAIL] Assertion failed: %s\n", #condition); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if ((condition)) { \
            printf("    [FAIL] Assertion failed: !(%s)\n", #condition); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_EQUAL(a, b) \
    do { \
        if ((a) != (b)) { \
            printf("    [FAIL] Expected %ld, got %ld\n", (long)(a), (long)(b)); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_NOT_EQUAL(a, b) \
    do { \
        if ((a) == (b)) { \
            printf("    [FAIL] Values should not be equal: %ld\n", (long)(a)); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            printf("    [FAIL] Expected NULL pointer\n"); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf("    [FAIL] Pointer should not be NULL\n"); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_EQ(a, b) ASSERT_EQUAL(a, b)

#define ASSERT_STREQ(a, b) \
    do { \
        if (!(a) || !(b) || strcmp((a), (b)) != 0) { \
            printf("    [FAIL] Strings not equal: %s vs %s\n", (a)?(a):"null", (b)?(b):"null"); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_GTE(a, b) \
    do { \
        if ((a) < (b)) { \
            printf("    [FAIL] %ld < %ld\n", (long)(a), (long)(b)); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

#define ASSERT_GT(a, b) \
    do { \
        if ((a) <= (b)) { \
            printf("    [FAIL] %ld <= %ld\n", (long)(a), (long)(b)); \
            test_ctx.failed++; \
        } else { \
            test_ctx.passed++; \
        } \
        test_ctx.total++; \
    } while(0)

typedef struct {
    const char *name;
    int (*fn)(void);
} integration_test_case_t;

#define TEST(fn) { #fn, fn }

static inline void run_integ(const char *name, int (*setup)(void), int (*teardown)(void),
                             integration_test_case_t tests[], size_t count) {
    printf("  [*] Integration Suite: %s\n", name);
    if (setup) setup();
    for (size_t i = 0; i < count; i++) {
        test_ctx.current_test = (char*)tests[i].name;
        printf("    [*] Running: %s\n", tests[i].name);
        tests[i].fn();
    }
    if (teardown) teardown();
}

#define RUN_INTEG_SUITE(name, setup, teardown, ...) \
    do { \
        integration_test_case_t _c[] = { __VA_ARGS__ }; \
        run_integ(name, setup, teardown, _c, sizeof(_c) / sizeof(_c[0])); \
    } while(0)

#define PRINT_TEST_RESULTS() \
    do { \
        printf("\n========================================\n"); \
        printf("Test Results:\n"); \
        printf("  Passed: %d\n", test_ctx.passed); \
        printf("  Failed: %d\n", test_ctx.failed); \
        printf("  Total:  %d\n", test_ctx.total); \
        printf("  Success Rate: %.2f%%\n", (test_ctx.total > 0) ? ((float)test_ctx.passed / test_ctx.total * 100) : 0); \
        printf("========================================\n"); \
    } while(0)

#endif
