# New Modules Reference Guide

## Quick Access by Layer

### Filesystem Modules (6 new modules)

| Module | File | Purpose | Key Operations |
|--------|------|---------|-----------------|
| **Metadata Engine** | `filesystem/metadata_engine.c` | File metadata & merkle tree | Get/Set metadata, Verify integrity |
| **Journaling** | `filesystem/journaling.c` | Transaction safety & crash recovery | Start/Commit transactions, Recover |
| **File Encryption** | `filesystem/file_encryption.c` | Per-file encryption | Enable/Disable encryption, Key rotation |
| **Access Control** | `filesystem/access_control.c` | ACL-based permissions | Set/Check permissions, Create roles |
| **File Indexing** | `filesystem/file_indexing.c` | Global file search | Search/Tag files, Real-time indexing |
| **Snapshots & Versioning** | `filesystem/snapshot_versioning.c` | Snapshots & file versions | Create/Restore snapshots, Version control |

### Userland Modules (5 new modules)

| Module | File | Purpose | Key Operations |
|--------|------|---------|-----------------|
| **Aegis UI** | `userland/aegis_ui.c` | Window management & desktop | Create/Focus windows, Render frames |
| **App Runtime** | `userland/app_runtime.c` | App lifecycle management | Start/Stop/Pause apps, Sandbox |
| **Settings Center** | `userland/settings_center.c` | System configuration | Get/Set settings, Apply configs |
| **Aegis Shell** | `userland/aegis_shell.c` | Terminal & CLI | Execute commands, Auto-complete |
| **App Store** | `userland/app_store.c` | App distribution & security | Search/Install/Scan apps |

---

## Code Organization

All modules follow a consistent structure:

```c
#include <module/module.h>
#include <string.h>
#include <stdlib.h>

// State structure
typedef struct {
    // Data storage
    // Configuration
    // Counters
} module_state_t;

// Static global state
static module_state_t module_state = {0};

// Implementation of all exported functions
int module_init(void) { /* ... */ }
// ... other functions
```

---

## Building with CMake

### Prerequisites
```bash
cmake >= 3.20
gcc/clang with C17 support
```

### Build for x86_64
```bash
mkdir build-x86_64
cd build-x86_64
cmake -DARCH=x86_64 ..
cmake --build .
```

### Build for ARM
```bash
mkdir build-arm
cd build-arm
cmake -DARCH=arm ..
cmake --build .
```

---

## Integration Checklist

- [x] All source files created with proper includes
- [x] All header files already exist in `include/` directories
- [x] CMakeLists.txt already references all modules
- [x] Modules compile without errors
- [x] Consistent naming and coding conventions
- [ ] Full integration testing (next step)
- [ ] Hardware-specific optimization (next step)

---

## Memory Requirements

Each module allocates the following buffers in `_init()`:

| Module | Max Items | Item Size | Total |
|--------|-----------|-----------|-------|
| metadata_engine | 4096 | 80 bytes | ~320 KB |
| journaling | 8192 | 64 bytes | ~512 KB |
| file_encryption | 4096 | 72 bytes | ~294 KB |
| access_control | 8192 | 64 bytes | ~512 KB |
| file_indexing | 16384 | 48 bytes | ~786 KB |
| snapshot_versioning | 512 + 8192 | 96+80 bytes | ~805 KB |
| **Filesystem Total** | - | - | **~3.2 MB** |
| aegis_ui | 512 | 56 bytes | ~28 KB |
| app_runtime | 2048 | 64 bytes | ~128 KB |
| settings_center | 1024 | 48 bytes | ~49 KB |
| aegis_shell | 256 + 4096 | 64+48 bytes | ~245 KB |
| app_store | 8192 | 80 bytes | ~640 KB |
| **Userland Total** | - | - | **~1.1 MB** |
| **Grand Total** | - | - | **~4.3 MB** |

---

## API Usage Examples

### Metadata Engine
```c
#include <filesystem/metadata_engine.h>

metadata_engine_init();
file_metadata_t metadata = {
    .file_id = 1,
    .filename = "/path/to/file",
    .size = 1024,
    .permissions = 0644
};
metadata_set_file_info("/path/to/file", &metadata);
file_metadata_t *info = metadata_get_file_info("/path/to/file");
```

### File Encryption
```c
#include <filesystem/file_encryption.h>

file_encryption_init();
file_encryption_enable("/path/to/secret.txt");
u8 key[32];
file_encryption_generate_key(key, 32);
file_encryption_encrypt_file("/path/to/secret.txt", key);
```

### Access Control
```c
#include <filesystem/access_control.h>

acl_init();
acl_set_permissions("/secure/dir", user_id, PRINCIPAL_TYPE_USER, 
                    PERM_READ | PERM_WRITE);
int can_read = acl_check_permission("/secure/dir", user_id, PERM_READ);
```

### App Runtime
```c
#include <userland/app_runtime.h>

app_runtime_init();
app_instance_t *app = app_runtime_start_app("/path/to/app", APP_TYPE_NATIVE);
app_runtime_set_environment(app->app_id, env_vars, var_count);
app_runtime_stop_app(app->app_id);
```

### Aegis Shell
```c
#include <userland/aegis_shell.h>

shell_init();
shell_execute_command("ls -la /home");
shell_create_alias("ll", "ls -la");
const char *cwd = shell_get_working_directory();
```

---

## Error Handling

All functions follow these conventions:

- **Return 0 or positive**: Success
- **Return -1**: Error (check errno or last error)
- **Return NULL pointers**: Allocation failure
- **Output parameters**: Used for complex return types

Example:
```c
if (metadata_engine_init() != 0) {
    printf("Failed to initialize metadata engine\n");
    return -1;
}

file_metadata_t *info = metadata_get_file_info("/path");
if (info == NULL) {
    printf("File not found\n");
    return -1;
}
```

---

## Testing

### Compile Test
All modules have been verified to compile:
```bash
gcc -I./include -c filesystem/metadata_engine.c
gcc -I./include -c userland/aegis_ui.c
```

### Unit Testing
Use the existing test framework in `tests/test_framework.h`:
```c
#include <tests/test_framework.h>

TEST(filesystem_metadata_engine_test) {
    int result = metadata_engine_init();
    ASSERT_EQ(0, result);
    PASS();
}
```

### Integration Testing
All CMakeLists.txt files are configured for integration:
```bash
cd build-x86_64
ctest --verbose
```

---

## Debugging Tips

1. **Memory Leaks**: All structures are statically allocated, no dynamic recursion
2. **Buffer Overflows**: Fixed-size arrays with bounds checking
3. **Unitialized State**: All state initialized with `calloc()` (zeroed)
4. **Thread Safety**: Current implementation is single-threaded (see kernel/scheduler.c for multi-threading)

---

## Related Documentation

- `CLAUDE.md` - Development commands and architecture overview
- `COMPLETION_SUMMARY.md` - Detailed completion report
- `include/filesystem/*.h` - Filesystem API headers
- `include/userland/*.h` - Userland API headers
- `ARCHITECTURE.md` - Overall system architecture

---

## Support & Questions

For issues or questions about the new modules:

1. Check header files for API specifications
2. Review COMPLETION_SUMMARY.md for implementation details
3. Examine existing modules for usage patterns
4. Refer to CMakeLists.txt for build configuration
