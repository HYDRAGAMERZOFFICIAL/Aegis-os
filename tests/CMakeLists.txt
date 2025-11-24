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
