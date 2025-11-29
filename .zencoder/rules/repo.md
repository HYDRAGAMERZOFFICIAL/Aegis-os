---
description: Repository Information Overview
alwaysApply: true
---

# Aegis OS Kernel Information

## Summary
Aegis OS is a comprehensive, security-focused kernel operating system written in C with Rust components. It features a modular architecture with 53+ modules across 9 layers: core kernel, hardware abstraction (HAL), drivers, filesystem, security, services, developer APIs, userland, and common utilities. Designed for multi-architecture support (x86_64, ARM) with advanced security features including zero-trust access control, microkernel architecture, and AI-powered intrusion detection.

## Repository Structure
- **kernel/** - Core kernel modules (scheduler, memory, IPC, interrupt handling, security monitoring)
- **arch/** - Architecture-specific implementations (x86_64, ARM with CPU-specific optimizations)
- **hal/** - Hardware Abstraction Layer (Rust-based, covering CPU, storage, GPU, USB, power management)
- **drivers/** - Device drivers (input, GPU, storage, network, sensors) with WebAssembly sandboxing
- **filesystem/** - AegisFS implementation with journaling, encryption, ACL, indexing, snapshots, self-healing
- **security/** - Security modules (crypto engine, zero-trust AC, firewall/IDS/IPS, secure boot, hypervisor protection)
- **services/** - System services (15 modules: service manager, session manager, update manager, network manager, logging, audit, sandbox, UI components, task optimizer)
- **devapi/** - Developer APIs (core, UI, filesystem, network, crypto, sensors, containers)
- **userland/** - User-facing components (desktop UI, app runtime, shell, settings, app store)
- **common/** - Shared utilities (string ops, math, bitmap, red-black trees, linked lists, crypto stubs)
- **tests/** - Comprehensive test suite (43+ test cases across kernel, security, drivers, filesystem, devapi)

## Language & Runtime
**Languages**: C (C17 standard), C++, Assembly (x86-64, ARM), Rust (HAL module)  
**Build System**: CMake 3.20+  
**Target Architectures**: x86_64, ARM32, ARM64  
**API Standard**: POSIX-compatible kernel interfaces  

## Dependencies
**Build Dependencies**:
- CMake 3.20 or higher
- C17/C++ compiler (GCC or Clang with x86-64/ARM support)
- Rustup (for HAL module compilation)

**Runtime Dependencies**:
- Architecture-specific CPU features (x86: ASLR, NXE, SMAP, SMEP; ARM: MMU, TLB)
- Firmware support (UEFI/BIOS for x86_64, device tree for ARM)

**Key Internal Libraries**:
- Red-Black tree implementation (common/rbtree.c)
- Doubly-linked list (common/list.c)
- Bitmap operations (common/bitmap.c)
- Cryptographic primitives (SHA-1, SHA-256, AES, RSA, ECC, Kyber, Dilithium)

## Build & Installation

### Configure and Build (x86_64)
```bash
mkdir build-x86_64
cd build-x86_64
cmake -DARCH=x86_64 ..
cmake --build .
```

### Configure and Build (ARM)
```bash
mkdir build-arm
cd build-arm
cmake -DARCH=arm ..
cmake --build .
```

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### HAL Module (Rust)
```bash
cd hal
cargo build --release
```

## Testing

**Framework**: Custom C test framework (test_framework.h)  
**Test Location**: tests/ directory  
**Test Suites**: 6 modules (kernel, security, drivers, filesystem, devapi, integration)  
**Test Coverage**: 43+ test cases  

**Run All Tests**:
```bash
cd build-x86_64
ctest --verbose
```

**Run Direct Test Executable**:
```bash
./aegis_tests
```

**Test Configuration**: CMakeLists.txt (tests/) - uses enable_testing() and add_test() directives

## Main Entry Points
- **Kernel**: kernel/main.c → kernel_main()
- **Bootloader**: arch/boot.c (architecture-specific entry)
- **HAL Init**: hal/hal.c → hal_initialize()
- **System Init**: kernel/system_init.c → system_init()

## Key Modules

### Kernel Core (kernel/)
- **process.c**: Process and thread management, priority scheduling
- **memory.c**: Virtual memory, paging, ASLR, encrypted RAM pages
- **scheduler.c**: CFS scheduler, deadline scheduling, load balancing
- **interrupt.c**: Hardware interrupt management and event queue processing
- **ipc.c**: Message passing, pipes, shared memory, signals
- **filesystem.c**: Transaction-based file operations, journaling
- **network.c**: Layer 2-7 networking, encrypted sockets, firewall hooks
- **driver.c**: Driver lifecycle management, WebAssembly sandboxing
- **security.c**: Kernel code integrity verification, syscall monitoring
- **hypervisor.c**: Hypervisor-level memory protection and isolation

### Security (security/)
- **crypto_engine.c**: AES-XTS, ChaCha20, RSA, ECC, post-quantum (Kyber/Dilithium)
- **zero_trust_ac.c**: Zero-trust access control model
- **aegis_shield.c**: Firewall, IDS, IPS functionality
- **secure_boot.c**: Secure boot and TPM integration
- **app_permissions.c**: Application permission management
- **aegis_guard.c**: AI-powered intrusion detection
- **microkernel.c**: Hybrid microkernel architecture
- **hypervisor_protection.c**: Hypervisor-level protection

### Services (services/)
Manager modules for: system initialization, sessions, updates, network, storage, package management, logging, audit, sandboxing, command palette, troubleshooting, task optimization, neural monitoring, voice commands, checkpoint/rollback

### Hardware Abstraction Layer (hal/)
- **hal_cpu.c**: CPU operations, context switching
- **hal_storage.c**: NVMe, SATA, USB, SD card interfaces
- **hal_gpu.c**: GPU initialization and management
- **hal_gpu_display.c**: Display output and rendering
- **hal_usb.c**: USB device enumeration and control
- **hal_power.c**: Power management, sleep states
- **hal_audio.c**: Audio subsystem
- **hal_acpi_uefi.c**: ACPI and UEFI firmware interfaces

### Developer APIs (devapi/)
- **core_api.c**: Process, thread, memory management APIs
- **ui_api.c**: Native UI components and windows
- **fs_api.c**: File operations with encryption and snapshots
- **network_api.c**: Socket and networking APIs
- **crypto_api.c**: Cryptographic function exports
- **sensor_api.c**: Hardware sensor access control
- **container_api.c**: Container runtime support (Docker/Podman/LXC/Firecracker)

## Configuration Files
- **CMakeLists.txt** (root): Project configuration, version 1.0.0
- **CMakeLists.txt** (per subdirectory): Module-specific build rules
- **hal/Cargo.toml**: Rust HAL module dependencies
- **include/** headers: Public API definitions for all modules

## System Limits (kernel/types.h)
- MAX_CPUS: 256
- MAX_PROCESSES: 4096
- MAX_THREADS_PER_PROCESS: 256
- MAX_IRQ_HANDLERS: 256
- PAGE_SIZE: 4096 bytes

## Architecture Support
**x86_64 Features**: Paging, ASLR, NXE, SMAP, SMEP, GDT/TSS, IDT, context switching  
**ARM Features**: MMU, TLB invalidation, exception vectors, GICv2/v3 interrupt controller, context switching

## Performance & Advanced Features
- **Adaptive Scheduler**: Detects user behavior patterns (30-40% responsiveness boost)
- **RAM Compression**: Transparent memory compression with reduced swapping
- **GPU Acceleration**: Full UI rendering on GPU
- **AI Pre-fetch**: Pattern learning for faster app opening (50-70% improvement)
- **Universal Caching**: Multi-layer caching (RAM, SSD, network)
- **Tiered Storage**: Intelligent storage hierarchy management
- **Self-Healing Filesystem**: Automatic corruption repair with Merkle tree validation
- **Checkpoint System**: Full/delta snapshots with atomic 10-second rollback
