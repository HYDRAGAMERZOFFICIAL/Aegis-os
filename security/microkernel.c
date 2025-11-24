#include <security/microkernel.h>
#include <string.h>

int mkern_init(void) { return 0; }
int mkern_register_component(mkern_component_t *component) { return 0; }
int mkern_unregister_component(uint32_t component_id) { return 0; }
int mkern_set_privilege_level(uint32_t component_id, mkern_privilege_level_t level) { return 0; }
int mkern_set_memory_limit(uint32_t component_id, uint64_t limit_bytes) { return 0; }
int mkern_set_syscall_mask(uint32_t component_id, uint64_t syscall_mask) { return 0; }
int mkern_allow_hw_access(uint32_t component_id) { return 0; }
int mkern_revoke_hw_access(uint32_t component_id) { return 0; }
int mkern_enforce_isolation(uint32_t component_id1, uint32_t component_id2) { return 0; }
int mkern_allow_ipc(uint32_t component_id1, uint32_t component_id2) { return 0; }
int mkern_verify_syscall_allowed(uint32_t component_id, uint32_t syscall_num) { return 0; }
int mkern_enable_strict_mode(void) { return 0; }
