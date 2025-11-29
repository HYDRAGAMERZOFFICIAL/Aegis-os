#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_framework.h"
#include "../kernel/security_hardening.h"

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

TEST_SUITE(capability_tests) {
    printf("\n=== Phase 7: Capability Tests ===\n");
    
    int r = cap_grant(0, CAP_SYS_ADMIN);
    log_test("cap_grant_admin", r == 0);
    
    int has = cap_has(0, CAP_SYS_ADMIN);
    log_test("cap_has_admin", has);
    
    r = cap_grant(1, CAP_NET_ADMIN);
    log_test("cap_grant_net_admin", r == 0);
    
    has = cap_has(1, CAP_NET_ADMIN);
    log_test("cap_has_net_admin", has);
    
    r = cap_revoke(0, CAP_SYS_ADMIN);
    log_test("cap_revoke", r == 0);
    
    has = cap_has(0, CAP_SYS_ADMIN);
    log_test("cap_revoked", !has);
    
    r = cap_grant(2, CAP_SETUID);
    log_test("cap_grant_setuid", r == 0);
    
    r = cap_grant(2, CAP_SETGID);
    log_test("cap_grant_setgid", r == 0);
    
    has = cap_has(2, CAP_SETUID);
    log_test("cap_has_setuid", has);
    
    has = cap_has(2, CAP_SETGID);
    log_test("cap_has_setgid", has);
}

TEST_SUITE(security_policy_tests) {
    printf("\n=== Phase 7: Security Policy Tests ===\n");
    
    int r = sec_create_policy(1, "test_policy");
    log_test("sec_create_policy", r == 0);
    
    r = sec_policy_allow(1, 100, 200);
    log_test("sec_policy_allow", r == 0);
    
    int allowed = sec_is_allowed(1, 100, 200);
    log_test("sec_is_allowed_true", allowed);
    
    allowed = sec_is_allowed(1, 100, 300);
    log_test("sec_is_allowed_false", !allowed);
    
    r = sec_policy_deny(1, 300, 400);
    log_test("sec_policy_deny", r == 0);
    
    allowed = sec_is_allowed(1, 300, 400);
    log_test("sec_is_allowed_after_deny", !allowed);
    
    r = sec_create_policy(2, "test_policy_2");
    log_test("sec_create_policy_2", r == 0);
    
    r = sec_policy_allow(2, 500, 600);
    log_test("sec_policy_allow_2", r == 0);
    
    r = sec_delete_policy(1);
    log_test("sec_delete_policy", r == 0);
}

TEST_SUITE(selinux_tests) {
    printf("\n=== Phase 7: SELinux Tests ===\n");
    
    int r = selinux_set_mode(SELINUX_PERMISSIVE);
    log_test("selinux_set_permissive", r == 0);
    
    SELinuxMode mode = selinux_get_mode();
    log_test("selinux_get_mode_permissive", mode == SELINUX_PERMISSIVE);
    
    r = selinux_set_mode(SELINUX_ENFORCING);
    log_test("selinux_set_enforcing", r == 0);
    
    mode = selinux_get_mode();
    log_test("selinux_get_mode_enforcing", mode == SELINUX_ENFORCING);
    
    r = selinux_load_policy("test_policy.bin");
    log_test("selinux_load_policy", r == 0);
    
    r = selinux_set_mode(SELINUX_DISABLED);
    log_test("selinux_set_disabled", r == 0);
}

TEST_SUITE(aslr_tests) {
    printf("\n=== Phase 7: ASLR Tests ===\n");
    
    int r = aslr_enable(ASLR_PARTIAL);
    log_test("aslr_enable_partial", r == 0);
    
    ASLRLevel level = aslr_get_level();
    log_test("aslr_get_level_partial", level == ASLR_PARTIAL);
    
    r = aslr_enable(ASLR_FULL);
    log_test("aslr_enable_full", r == 0);
    
    level = aslr_get_level();
    log_test("aslr_get_level_full", level == ASLR_FULL);
    
    uint64_t addr = 0x400000;
    uint64_t randomized = aslr_randomize_address(addr);
    log_test("aslr_randomize", randomized != 0);
    
    r = aslr_disable();
    log_test("aslr_disable", r == 0);
    
    level = aslr_get_level();
    log_test("aslr_disabled", level == ASLR_DISABLED);
}

TEST_SUITE(stack_canary_tests) {
    printf("\n=== Phase 7: Stack Canary Tests ===\n");
    
    int r = stack_canary_init();
    log_test("stack_canary_init", r == 0);
    
    r = stack_canary_check(1);
    log_test("stack_canary_check", r == 0);
    
    StackCanary canary;
    canary.canary = 0x12345678;
    canary.expected = 0x12345678;
    canary.process_id = 1;
    
    r = stack_canary_verify(&canary);
    log_test("stack_canary_verify_match", r == 0);
    
    canary.expected = 0x87654321;
    r = stack_canary_verify(&canary);
    log_test("stack_canary_verify_mismatch", r != 0);
}

TEST_SUITE(control_flow_guard_tests) {
    printf("\n=== Phase 7: Control Flow Guard Tests ===\n");
    
    int r = cfg_enable();
    log_test("cfg_enable", r == 0);
    
    r = cfg_validate_return(0x400000);
    log_test("cfg_validate_return_valid", r == 0);
    
    r = cfg_validate_return(0);
    log_test("cfg_validate_return_invalid", r != 0);
    
    r = cfg_disable();
    log_test("cfg_disable", r == 0);
}

TEST_SUITE(secure_boot_tests) {
    printf("\n=== Phase 7: Secure Boot Tests ===\n");
    
    unsigned char image[] = {0x42, 0x4F, 0x4F, 0x54};
    
    int r = secure_boot_verify(image, sizeof(image));
    log_test("secure_boot_verify", r == 0);
    
    uint8_t hash[32];
    r = secure_boot_measure(image, sizeof(image), hash);
    log_test("secure_boot_measure", r == 0);
}

TEST_SUITE(authentication_tests) {
    printf("\n=== Phase 7: Authentication Tests ===\n");
    
    int r = auth_create_user("testuser", "password123");
    log_test("auth_create_user", r == 0);
    
    int verified = auth_verify("testuser", "password123");
    log_test("auth_verify_correct", verified);
    
    verified = auth_verify("testuser", "wrongpassword");
    log_test("auth_verify_incorrect", !verified);
    
    r = auth_change_password("testuser", "password123", "newpassword");
    log_test("auth_change_password", r == 0);
    
    verified = auth_verify("testuser", "newpassword");
    log_test("auth_verify_new_password", verified);
    
    r = auth_delete_user("testuser");
    log_test("auth_delete_user", r == 0);
}

TEST_SUITE(audit_logging_tests) {
    printf("\n=== Phase 7: Audit Logging Tests ===\n");
    
    int r = sec_audit_log(1, "Test audit event");
    log_test("sec_audit_log", r == 0);
    
    r = sec_audit_log(2, "Security violation");
    log_test("sec_audit_log_violation", r == 0);
    
    r = sec_audit_log(3, "Access denied");
    log_test("sec_audit_log_denial", r == 0);
}

TEST_SUITE(security_callback_tests) {
    printf("\n=== Phase 7: Security Callback Tests ===\n");
    
    SecurityCallbacks callbacks = {0};
    callbacks.on_violation = NULL;
    callbacks.on_unauthorized_access = NULL;
    callbacks.on_policy_breach = NULL;
    
    int r = sec_set_callbacks(&callbacks);
    log_test("sec_set_callbacks", r == 0);
}

TEST_SUITE(multi_user_security_tests) {
    printf("\n=== Phase 7: Multi-User Security Tests ===\n");
    
    int r = auth_create_user("user1", "pass1");
    log_test("auth_create_user1", r == 0);
    
    r = auth_create_user("user2", "pass2");
    log_test("auth_create_user2", r == 0);
    
    int verified1 = auth_verify("user1", "pass1");
    int verified2 = auth_verify("user2", "pass2");
    log_test("auth_verify_both", verified1 && verified2);
    
    int cross_verify = auth_verify("user1", "pass2");
    log_test("auth_cross_verify_fail", !cross_verify);
    
    cap_grant(1, CAP_SYS_ADMIN);
    cap_grant(2, CAP_NET_ADMIN);
    
    int has1 = cap_has(1, CAP_SYS_ADMIN);
    int has2 = cap_has(2, CAP_NET_ADMIN);
    log_test("multi_user_caps", has1 && has2);
    
    int wrong1 = cap_has(1, CAP_NET_ADMIN);
    int wrong2 = cap_has(2, CAP_SYS_ADMIN);
    log_test("multi_user_caps_separate", !wrong1 && !wrong2);
}

int main() {
    printf("════════════════════════════════════════════════════════════\n");
    printf("  Aegis OS - Phase 7: Security Hardening Tests\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    capability_tests();
    security_policy_tests();
    selinux_tests();
    aslr_tests();
    stack_canary_tests();
    control_flow_guard_tests();
    secure_boot_tests();
    authentication_tests();
    audit_logging_tests();
    security_callback_tests();
    multi_user_security_tests();
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("  TOTAL: %d | PASSED: %d | FAILED: %d\n", test_count, pass_count, fail_count);
    printf("  PASS RATE: %.1f%%\n", test_count > 0 ? (100.0 * pass_count / test_count) : 0);
    printf("════════════════════════════════════════════════════════════\n");
    
    return fail_count > 0 ? 1 : 0;
}
