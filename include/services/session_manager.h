#ifndef AEGIS_SESSION_MANAGER_H
#define AEGIS_SESSION_MANAGER_H

#include <kernel/types.h>

typedef enum {
    AUTH_METHOD_PASSWORD,
    AUTH_METHOD_BIOMETRIC,
    AUTH_METHOD_KEYCARD,
    AUTH_METHOD_PIN,
    AUTH_METHOD_MFA
} auth_method_t;

typedef enum {
    SESSION_STATE_LOCKED,
    SESSION_STATE_UNLOCKING,
    SESSION_STATE_ACTIVE,
    SESSION_STATE_IDLE,
    SESSION_STATE_LOCKING
} session_state_t;

typedef struct {
    u64 session_id;
    u64 user_id;
    u64 start_time;
    u64 idle_time;
    session_state_t state;
    u8 session_token[32];
    u32 permission_mask;
} session_t;

typedef struct {
    u64 user_id;
    char *username;
    char *password_hash;
    bool biometric_enabled;
    u8 *biometric_template;
    u64 template_size;
    bool keycard_enabled;
    u8 *keycard_id;
} user_profile_t;

int session_init(void);
session_t *session_create(u64 user_id, const u8 *session_token);
int session_destroy(u64 session_id);
int session_authenticate(u64 user_id, auth_method_t method, const void *credential);
int session_verify_token(const u8 *token);
int session_lock_screen(u64 session_id);
int session_unlock_screen(u64 session_id, auth_method_t method, const void *credential);
int session_register_user(const char *username, const char *password);
int session_enable_biometric(u64 user_id, const u8 *template, u64 size);
int session_enable_keycard(u64 user_id, const u8 *keycard_id);
session_state_t session_get_state(u64 session_id);
session_t *session_get_active(void);

#endif
