#include "test_framework.h"
#include <kernel/types.h>

void run_kernel_tests(void);
void run_security_tests(void);
void run_drivers_tests(void);
void run_filesystem_tests(void);
void run_devapi_tests(void);
void test_integration_main(void);
void run_profiler_tests(void);

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║     Aegis OS - Comprehensive Test Suite      ║\n");
    printf("╚════════════════════════════════════════╝\n");

    printf("\n[Phase 1] Running Kernel Module Tests...\n");
    run_kernel_tests();

    printf("\n[Phase 2] Running Security Module Tests...\n");
    run_security_tests();

    printf("\n[Phase 3] Running Driver Module Tests...\n");
    run_drivers_tests();

    printf("\n[Phase 4] Running Filesystem Module Tests...\n");
    run_filesystem_tests();

    printf("\n[Phase 5] Running Developer API Tests...\n");
    run_devapi_tests();

    printf("\n[Phase 6] Running Integration Tests...\n");
    test_integration_main();

    printf("\n[Phase 7] Running Profiler Tests...\n");
    run_profiler_tests();

    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║          Test Suite Complete            ║\n");
    printf("╚════════════════════════════════════════╝\n");

    PRINT_TEST_RESULTS();

    return test_ctx.failed > 0 ? 1 : 0;
}
