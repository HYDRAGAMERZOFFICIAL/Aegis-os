#include <assert.h>
#include <services/service_manager.h>
#include <stdio.h>
#include <string.h>

// Mock types if needed, but we should include the real header
// We might need to point the include path correctly during compilation

int main() {
  printf("Testing Service Manager...\n");

  // 1. Init
  assert(asm_init() == 0);
  printf("[PASS] asm_init\n");

  // 2. Register Service
  const char *deps[] = {"NetworkService"};
  service_t *svc = asm_register_service("TestService", deps, 1);
  assert(svc != NULL);
  assert(strcmp(svc->name, "TestService") == 0);
  assert(svc->service_id == 1);
  assert(svc->state == SERVICE_STATE_STOPPED);
  printf("[PASS] asm_register_service\n");

  // 3. Start Service (Should fail due to missing dependency)
  int res = asm_start_service(svc->service_id);
  assert(res == -3); // Dependency missing
  printf("[PASS] asm_start_service (missing dependency)\n");

  // 4. Register Dependency
  service_t *net_svc = asm_register_service("NetworkService", NULL, 0);
  assert(net_svc != NULL);

  // 5. Start Dependency
  res = asm_start_service(net_svc->service_id);
  assert(res == 0);
  assert(net_svc->state == SERVICE_STATE_RUNNING);
  printf("[PASS] asm_start_service (dependency)\n");

  // 6. Start Service (Should succeed now)
  res = asm_start_service(svc->service_id);
  assert(res == 0);
  assert(svc->state == SERVICE_STATE_RUNNING);
  printf("[PASS] asm_start_service (success)\n");

  // 7. Stop Service
  res = asm_stop_service(svc->service_id);
  assert(res == 0);
  assert(svc->state == SERVICE_STATE_STOPPED);
  printf("[PASS] asm_stop_service\n");

  // 8. Restart Service
  res = asm_restart_service(svc->service_id);
  assert(res == 0);
  assert(svc->state == SERVICE_STATE_RUNNING);
  assert(svc->restart_count == 1);
  printf("[PASS] asm_restart_service\n");

  printf("All tests passed!\n");
  return 0;
}
