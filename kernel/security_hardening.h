#ifndef SECURITY_HARDENING_H
#define SECURITY_HARDENING_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    CAP_SYS_ADMIN = 0,
    CAP_NET_ADMIN = 1,
    CAP_DAC_OVERRIDE = 2,
    CAP_SETUID = 3,
    CAP_SETGID = 4,
    CAP_NET_RAW = 5,
    CAP_SYS_BOOT = 6,
    CAP_SYS_MODULE = 7,
    CAP_SYS_PTRACE = 8,
    CAP_SYS_RESOURCE = 9,
} Capability;

typedef enum {
    SELINUX_ENFORCING = 0,
    SELINUX_PERMISSIVE = 1,
    SELINUX_DISABLED = 2,
} SELinuxMode;

typedef enum {
    ASLR_DISABLED = 0,
    ASLR_PARTIAL = 1,
    ASLR_FULL = 2,
} ASLRLevel;

typedef struct {
    uint64_t capabilities;
    int uid;
    int gid;
    int is_restricted;
} Credentials;

typedef struct {
    int policy_id;
    char name[64];
    uint32_t rules_count;
    uint64_t created_at;
} SecurityPolicy;

typedef struct {
    uint32_t seed;
    ASLRLevel level;
    uint64_t code_base;
    uint64_t stack_base;
    uint64_t heap_base;
} ASLRConfig;

typedef struct {
    uint32_t canary;
    uint32_t expected;
    int process_id;
} StackCanary;

typedef struct {
    void (*on_violation)(int violation_type);
    void (*on_unauthorized_access)(int access_type);
    void (*on_policy_breach)(int policy_id);
} SecurityCallbacks;

int cap_grant(int uid, Capability cap);
int cap_revoke(int uid, Capability cap);
int cap_has(int uid, Capability cap);

int sec_create_policy(int policy_id, const char* name);
int sec_policy_allow(int policy_id, int src, int dst);
int sec_policy_deny(int policy_id, int src, int dst);
int sec_is_allowed(int policy_id, int src, int dst);
int sec_delete_policy(int policy_id);

int selinux_set_mode(SELinuxMode mode);
SELinuxMode selinux_get_mode(void);
int selinux_load_policy(const char* policy_path);

int aslr_enable(ASLRLevel level);
int aslr_disable(void);
ASLRLevel aslr_get_level(void);
uint64_t aslr_randomize_address(uint64_t addr);

int stack_canary_init(void);
int stack_canary_check(int pid);
int stack_canary_verify(StackCanary* canary);

int cfg_enable(void);
int cfg_disable(void);
int cfg_validate_return(uint64_t return_addr);

int secure_boot_verify(const void* image, size_t size);
int secure_boot_measure(const void* image, size_t size, uint8_t* hash);

int auth_create_user(const char* username, const char* password);
int auth_delete_user(const char* username);
int auth_verify(const char* username, const char* password);
int auth_change_password(const char* username, const char* old_pwd, const char* new_pwd);

int sec_set_callbacks(SecurityCallbacks* callbacks);
int sec_audit_log(int event_type, const char* description);

#endif
