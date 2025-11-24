# Aegis OS Kernel Development Notes

## Build Commands

### Configure for x86_64
```bash
mkdir build-x86_64
cd build-x86_64
cmake -DARCH=x86_64 ..
make
```

### Configure for ARM
```bash
mkdir build-arm
cd build-arm
cmake -DARCH=arm ..
make
```

## Project Structure

### Directory Layout
- `kernel/` - Core kernel modules source code
- `arch/` - Architecture-specific implementations (x86_64, ARM)
- `common/` - Common utilities and data structures
- `include/kernel/` - Public kernel headers
- `tests/` - Unit and integration tests

### Architecture Files
#### x86_64
- `arch/x86_64/mm.c` - Memory management (paging, ASLR, NXE, SMAP, SMEP)
- `arch/x86_64/irq.c` - Interrupt handling (IDT setup)
- `arch/x86_64/gdt.c` - Global Descriptor Table (GDT, TSS)
- `arch/x86_64/idt.c` - Interrupt Descriptor Table (IDT)
- `arch/x86_64/context_switch.c` - Context switching

#### ARM
- `arch/arm/mm.c` - Memory management (MMU, TLB invalidation)
- `arch/arm/irq.c` - Interrupt handling (exception vectors)
- `arch/arm/gic.c` - Generic Interrupt Controller (GICv2/v3)
- `arch/arm/context_switch.c` - Context switching

## Kernel Modules

### 1. Process Manager (PMGR) - kernel/process.c
- Creates, schedules, and terminates processes
- Manages threads with priority and CPU affinity
- Multi-Queue Priority Scheduler support
- **Key Functions:**
  - `pmgr_create_process()` - Create new process
  - `pmgr_create_thread()` - Create thread within process
  - `pmgr_set_thread_priority()` - Set thread priority
  - `pmgr_set_cpu_affinity()` - Set CPU affinity mask

### 2. Memory Manager (MMGR) - kernel/memory.c
- Virtual memory management with paging
- ASLR (Address Space Layout Randomization)
- Encrypted RAM pages
- Memory sandboxing
- **Key Functions:**
  - `mmgr_alloc_page()` - Allocate single page
  - `mmgr_alloc_pages()` - Allocate multiple pages
  - `mmgr_map_pages()` - Map pages with protection flags
  - `mmgr_enable_aslr()` - Enable address randomization
  - `mmgr_encrypt_page()` - Encrypt page
  - `mmgr_secure_zero()` - Securely zero memory

### 3. Scheduler - kernel/scheduler.c
- Completely Fair Scheduler (CFS) implementation
- Priority-based scheduling
- Deadline scheduling support
- Energy-aware CPU scaling
- Load balancing
- **Key Functions:**
  - `scheduler_enqueue()` - Add entity to scheduling queue
  - `scheduler_pick_next()` - Select next process to run
  - `scheduler_tick()` - Called on timer interrupt
  - `scheduler_set_class()` - Set scheduling class (RT/FAIR/DEADLINE)
  - `scheduler_balance_load()` - Balance CPU loads

### 4. Interrupt & Event Dispatcher (IED) - kernel/interrupt.c
- Hardware interrupt management
- Event queue processing
- Low-latency interrupt handling
- **Key Functions:**
  - `ied_register_irq()` - Register IRQ handler
  - `ied_dispatch_irq()` - Handle IRQ
  - `ied_post_event()` - Post event to queue
  - `ied_process_events()` - Process all pending events
  - `ied_set_irq_affinity()` - Set IRQ CPU affinity

### 5. AegisFS Kernel Module - kernel/filesystem.c
- Transaction-based write operations
- Journaling support
- File encryption
- Tamper-proof logs
- **Key Functions:**
  - `aegisfs_create_file()` - Create file
  - `aegisfs_write()` - Write to file (transactional)
  - `aegisfs_begin_transaction()` - Start transaction
  - `aegisfs_commit_transaction()` - Commit changes
  - `aegisfs_rollback_transaction()` - Rollback transaction

### 6. Kernel IPC Module - kernel/ipc.c
- Message passing between processes
- Pipes, shared memory, semaphores
- Signal handling
- Secure process boundaries
- **Key Functions:**
  - `ipc_create_message_queue()` - Create message queue
  - `ipc_send_message()` - Send message
  - `ipc_create_pipe()` - Create bidirectional pipe
  - `ipc_create_shared_memory()` - Allocate shared memory
  - `ipc_send_signal()` - Send signal to process

### 7. Network Stack Module - kernel/network.c
- Layer 2-7 networking (simplified)
- Socket management
- Built-in firewall hooks
- Encrypted sockets by default
- **Key Functions:**
  - `network_create_socket()` - Create socket
  - `network_connect()` - Connect to remote
  - `network_send()` - Send data
  - `network_receive()` - Receive data
  - `network_enable_firewall()` - Enable firewall
  - `network_enable_encrypted_sockets()` - Enable encryption

### 8. Device Driver Manager - kernel/driver.c
- Driver loading and isolation
- WebAssembly sandboxing for untrusted drivers
- Driver permissions and sandboxing
- **Key Functions:**
  - `driver_register()` - Register new driver
  - `driver_load()` - Load driver from path
  - `driver_set_sandbox_mode()` - Set sandbox type (WASM/VM)
  - `driver_set_permissions()` - Set driver permissions
  - `driver_create_wasm_sandbox()` - Create WASM sandbox

### 9. Kernel Security & Integrity Monitor - kernel/security.c
- Kernel code integrity verification
- System call monitoring and analysis
- Runtime behavior analysis
- Anomaly detection
- **Key Functions:**
  - `ksim_verify_kernel_code()` - Verify kernel integrity
  - `ksim_monitor_syscall()` - Monitor system call
  - `ksim_analyze_behavior()` - Analyze process behavior
  - `ksim_detect_anomaly()` - Detect behavioral anomalies
  - `ksim_enable_real_time_analysis()` - Enable real-time monitoring

## Common Utilities

- `common/string.c` - String manipulation (memcpy, strcmp, strlen, etc.)
- `common/math.c` - Math utilities (min, max, pow2, log2, gcd, lcm)
- `common/bitmap.c` - Bitmap operations
- `common/rbtree.c` - Red-Black tree implementation
- `common/list.c` - Doubly-linked list
- `common/crypto.c` - Cryptographic stubs (SHA-1, SHA-256, AES, random)

## Data Types and Structures

### Process Management
- `process_t` - Process descriptor
- `thread_t` - Thread descriptor
- `x86_64_context_t` - x86-64 register context
- `arm_context_t` - ARM register context

### Memory Management
- `address_space_t` - Process address space
- `vma_t` - Virtual memory area
- `page_info_t` - Physical page info

### Scheduling
- `sched_entity_t` - Scheduling entity
- `cfs_context_t` - CFS-specific context
- `deadline_context_t` - Deadline-specific context

### IPC
- `ipc_object_t` - IPC object (message queue, pipe, shared memory, etc.)
- `message_t` - IPC message
- `signal_handler_t` - Signal handler

### Filesystem
- `inode_t` - File inode
- `transaction_t` - Filesystem transaction
- `fs_superblock_t` - Filesystem superblock

## Configuration

### Maximum Limits (kernel/types.h)
- `MAX_CPUS` = 256
- `MAX_PROCESSES` = 4096
- `MAX_THREADS_PER_PROCESS` = 256
- `MAX_IRQ_HANDLERS` = 256
- `PAGE_SIZE` = 4096 bytes

## Compilation Flags

- `-march=x86-64` - x86-64 specific optimizations
- `-march=armv8-a` - ARMv8 specific optimizations
- `-Wall -Wextra -Werror` - Enable all warnings as errors
- `-fPIC` - Position independent code (for libraries)

## System Services Modules (9 services)

- `services/service_manager.c` - ASM (Aegis Service Manager)
- `services/session_manager.c` - Session & Login Manager
- `services/update_manager.c` - Update & Patch Manager
- `services/network_manager.c` - Network Manager
- `services/storage_manager.c` - Storage Manager
- `services/package_manager.c` - AegisPKG
- `services/logging_service.c` - Logging & Telemetry
- `services/audit_service.c` - Event Audit Service
- `services/sandbox_manager.c` - Sandbox Manager

## Security & Cryptography Modules (6 modules)

- `security/crypto_engine.c` - AES-XTS, ChaCha20, RSA, ECC, Post-Quantum
- `security/zero_trust_ac.c` - Zero-Trust Access Controller
- `security/aegis_shield.c` - Firewall + IDS + IPS
- `security/secure_boot.c` - Secure Boot & TPM integration
- `security/app_permissions.c` - App Permission Manager
- `security/syscall_gatekeeper.c` - System Call Gatekeeper

## Userland Modules (5 components)

- `userland/aegis_ui.c` - Desktop Environment
- `userland/app_runtime.c` - Aegis App Runtime
- `userland/settings_center.c` - Settings & Control Center
- `userland/aegis_shell.c` - Terminal & Shell
- `userland/app_store.c` - Aegis App Store

## Filesystem Modules (6 modules)

- `filesystem/metadata_engine.c` - File Metadata Management
- `filesystem/journaling.c` - Crash-Safe Transactions
- `filesystem/file_encryption.c` - Per-File Encryption
- `filesystem/access_control.c` - ACL & RBAC
- `filesystem/file_indexing.c` - Global File Search
- `filesystem/snapshot_versioning.c` - Snapshots & Versioning

## Next Steps

1. Implement bootloader for x86_64 and ARM
2. Add unit tests for all kernel and userspace modules
3. Implement proper logging/printk() function
4. Add performance profiling infrastructure
5. Implement SMP (Symmetric Multi-Processing) support
6. Create syscall interface for userspace/kernel communication
7. Implement kernel module loading system
8. Add IPC mechanisms for inter-service communication
9. Implement WebAssembly runtime for app sandboxing
10. Build complete end-to-end integration tests
