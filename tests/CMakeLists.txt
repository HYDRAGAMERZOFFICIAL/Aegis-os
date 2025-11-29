project(AegisOS_Tests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(TEST_FRAMEWORK
    test_framework.h
)

set(TEST_SOURCES
    test_main.c
    test_kernel.c
    test_security.c
    test_drivers.c
    test_filesystem.c
    test_devapi.c
    test_integration.c
    test_profiler.c
    test_phase3.c
    test_phase6_extended.c
    test_phase7_security.c
    test_phase8_ui.c
    test_phase9_advanced.c
    test_implementation_needed.c
)

add_executable(aegis_tests ${TEST_SOURCES})

target_link_libraries(aegis_tests 
    kernel_lib
    security_lib
    drivers_lib
    devapi_lib
    filesystem_lib
    common_lib
    userland_lib
)

add_test(NAME AegisOS_Tests COMMAND aegis_tests)

set_tests_properties(AegisOS_Tests PROPERTIES
    TIMEOUT 60
    PASS_REGULAR_EXPRESSION "Test Suite Complete"
)
