# Aegis OS - Complete System Architecture

## Overview

Aegis OS is a comprehensive, security-first operating system with a complete stack from kernel to user interface. This document provides the complete architecture overview.

```
┌──────────────────────────────────────────────────────────────────────────┐
│                           USER APPLICATIONS                              │
│  (Web browsers, text editors, media players, development tools, etc.)    │
└──────────────────────────┬───────────────────────────────────────────────┘
                           │
┌──────────────────────────┴───────────────────────────────────────────────┐
│                        USERLAND LAYER (5 components)                     │
├─────────────────────────────────────────────────────────────────────────┤
│  AegisUI (GUI)  │ App Runtime  │ Settings  │ AegisShell  │ App Store   │
└──────────────────────────┬───────────────────────────────────────────────┘
                           │
┌──────────────────────────┴───────────────────────────────────────────────┐
│               FILESYSTEM LAYER (6 modules)                              │
├─────────────────────────────────────────────────────────────────────────┤
│ Metadata │ Journaling │ Encryption │ Access Control │ Indexing │ Snapshot│
└──────────────────────────┬───────────────────────────────────────────────┘
                           │
┌──────────────────────────┴───────────────────────────────────────────────┐
│          SECURITY & CRYPTOGRAPHY LAYER (6 modules)                      │
├─────────────────────────────────────────────────────────────────────────┤
│  Crypto │ Zero-Trust AC │ Firewall/IDS/IPS │ Secure Boot │ Permissions│
│  Syscall Gatekeeper                                                    │
└──────────────────────────┬───────────────────────────────────────────────┘
                           │
┌──────────────────────────┴───────────────────────────────────────────────┐
│         SYSTEM SERVICES LAYER (9 services)                             │
├─────────────────────────────────────────────────────────────────────────┤
│ Service Mgr │ Session │ Update │ Network │ Storage │ Package │         │
│ Logging │ Audit │ Sandbox                                             │
└──────────────────────────┬───────────────────────────────────────────────┘
                           │
┌──────────────────────────┴───────────────────────────────────────────────┐
│          KERNEL LAYER (9 core modules)                                  │
├─────────────────────────────────────────────────────────────────────────┤
│ Process Mgr │ Memory Mgr │ Scheduler │ Interrupts │ AegisFS │          │
│ IPC │ Network │ Drivers │ Security Monitor                            │
└──────────────────────────┬───────────────────────────────────────────────┘
                           │
┌──────────────────────────┴───────────────────────────────────────────────┐
│           ARCHITECTURE-SPECIFIC LAYER                                   │
├─────────────────────────────────────────────────────────────────────────┤
│  x86_64: GDT/IDT │ Paging │ NXE │ SMAP │ SMEP │ Context Switch        │
│  ARM:    MMU │ GIC │ Exception Vectors │ Context Switch │ BTAC        │
└──────────────────────────┬───────────────────────────────────────────────┘
                           │
                    ┌──────┴──────┐
                    │   Hardware   │
                    │ CPU / Memory │
                    └──────────────┘
```

## Complete Module Inventory

### Total Modules: 26

#### Kernel Modules (9)
1. **Process Manager (PMGR)** - Process/thread lifecycle
2. **Memory Manager (MMGR)** - Virtual memory, ASLR, encryption
3. **Scheduler** - CFS/Priority/Deadline scheduling
4. **Interrupt & Event Dispatcher (IED)** - IRQ management
5. **AegisFS** - Transaction-based filesystem
6. **IPC Module** - Message passing, pipes, shared memory
7. **Network Stack** - Layer 2-7 networking
8. **Device Driver Manager** - Driver isolation, WASM sandboxing
9. **Security & Integrity Monitor** - Syscall monitoring, anomaly detection

#### System Services (9)
10. **Service Manager (ASM)** - Service lifecycle
11. **Session & Login Manager** - Authentication, sessions
12. **Update & Patch Manager** - Delta updates, KPATCH
13. **Network Manager** - Connection management
14. **Storage Manager** - Disk/volume management
15. **Package Manager (AegisPKG)** - Software installation
16. **Logging & Telemetry** - Log aggregation, zero telemetry by default
17. **Event Audit Service** - Compliance-aware audit logging
18. **Sandbox Manager** - App sandboxing with permission control

#### Security & Cryptography (6)
19. **Cryptography Engine** - AES, ChaCha20, RSA, ECC, Post-Quantum
20. **Zero-Trust Access Controller (ZTAC)** - Default-deny access control
21. **Aegis Shield** - Firewall + IDS + IPS
22. **Secure Boot & Attestation** - Signature verification, TPM
23. **Application Permission Manager** - Android-style but stricter
24. **System Call Gatekeeper** - ROP/JOP/privilege escalation detection

#### Filesystem Extensions (6)
25. **Metadata Engine** - Merkle tree integrity
26. **Journaling Module** - Crash-safe transactions
27. **File Encryption** - Per-file encryption with unique keys
28. **Access Control Layer** - ACLs, roles, RBAC
29. **File Indexing & Search** - Global fast search with tagging
30. **Snapshot & Versioning** - System restore + file versioning

#### Userland (5)
31. **AegisUI** - Desktop environment
32. **App Runtime** - WASM/Container/VM support
33. **Settings Center** - Unified configuration
34. **AegisShell** - Terminal with AI suggestions
35. **Aegis App Store** - Verified app distribution

#### Common Utilities (6)
36. **String** - String manipulation
37. **Math** - Math utilities
38. **Bitmap** - Bit manipulation
39. **Red-Black Tree** - Efficient searching
40. **Linked List** - Data structure
41. **Crypto Stubs** - SHA, AES, random

#### Architecture-Specific (Multiple)
- **x86_64**: GDT, IDT, MMU, context switching, paging
- **ARM**: MMU, GIC, exception handling, context switching

## Key Security Features

### Defense in Depth

1. **Kernel Level**
   - ASLR (Address Space Layout Randomization)
   - NXE (No-Execute) bit enforcement
   - SMAP/SMEP (kernel memory protection)
   - Secure memory wiping
   - Page encryption

2. **Process Level**
   - Process isolation
   - Virtual address spaces
   - Capability-based access control
   - Resource quotas

3. **Filesystem Level**
   - Per-file encryption
   - Merkle tree integrity verification
   - Access control lists
   - Tamper-proof audit logs
   - Transactional consistency

4. **Network Level**
   - Default encryption
   - Firewall with stateful inspection
   - Intrusion detection/prevention
   - DDoS protection
   - DoH (DNS-over-HTTPS)

5. **Application Level**
   - Sandboxing with WASM/VM
   - Permission system (app-level)
   - Zero-trust access control
   - Syscall gatekeeper
   - Real-time behavior analysis

6. **Cryptographic Level**
   - Hardware acceleration support
   - Post-quantum algorithms
   - Secure key derivation
   - Random number generation
   - Code signing & verification

## Component Dependencies

```
Applications
    ↓
AegisUI + AegisShell + App Runtime + App Store + Settings
    ↓
Services (9) + Filesystem (6)
    ↓
Security (6)
    ↓
Kernel (9)
    ↓
Architecture-specific (x86_64/ARM)
    ↓
Hardware
```

## File Hierarchy

```
/AegisOS
├── include/
│   ├── kernel/         # Kernel APIs
│   ├── services/       # Service APIs
│   ├── security/       # Security APIs
│   ├── filesystem/     # Filesystem APIs
│   └── userland/       # Userland APIs
├── kernel/             # Kernel implementations
├── services/           # System services
├── security/           # Security modules
├── filesystem/         # Filesystem modules
├── userland/           # Userland components
├── arch/               # Architecture-specific
│   ├── x86_64/
│   └── arm/
├── common/             # Common utilities
├── tests/              # Test suite
├── CMakeLists.txt      # Build configuration
├── CLAUDE.md           # Build instructions
├── ARCHITECTURE.md     # Kernel architecture
└── USERSPACE_MODULES.md # Complete userspace docs
```

## Build System

**Build Tool:** CMake 3.20+

### Configuration

```bash
# x86_64 build
mkdir build-x86_64
cd build-x86_64
cmake -DARCH=x86_64 -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# ARM build
mkdir build-arm
cd build-arm
cmake -DARCH=arm -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Output Libraries

- `libaegis_kernel.a` - Core kernel
- `libaegis_services.a` - System services
- `libaegis_security.a` - Security modules
- `libaegis_filesystem.a` - Filesystem modules
- `libaegis_userland.a` - Userland components
- `libaegis_common.a` - Common utilities

## Configuration Parameters

```c
// Maximum limits (kernel/types.h)
#define MAX_CPUS 256
#define MAX_PROCESSES 4096
#define MAX_THREADS_PER_PROCESS 256
#define MAX_IRQ_HANDLERS 256
#define PAGE_SIZE 4096

// Kernel base addresses
#define KERNEL_BASE 0xFFFF800000000000UL
#define KERNEL_HEAP_BASE 0xFFFF800000100000UL
```

## Scheduling Algorithm

**Type:** Hybrid CFS (Completely Fair Scheduler)

- **FAIR class**: CFS with vruntime tracking
- **RT class**: Priority-based, preemptive
- **DEADLINE class**: Hard deadline enforcement

## Memory Management

- **Paging**: 4KB pages with 64-bit page tables
- **ASLR**: Randomized virtual addresses
- **Encryption**: Optional per-page encryption
- **Sandboxing**: Isolated address spaces per process

## IPC Mechanisms

1. **Message Queues** - Async messaging
2. **Pipes** - Stream communication
3. **Shared Memory** - Fast data sharing
4. **Semaphores** - Synchronization
5. **Signals** - Async notifications

## Compliance Support

- **HIPAA** - Healthcare data protection
- **GDPR** - Privacy regulation
- **ISO27001** - Information security
- **SOC2** - Trust and security

## Attack Surface Mitigation

### Kernel-Level
- ROP chain detection
- Stack pivot prevention
- JOP gadget detection
- Heap spray prevention
- Privilege escalation blocking

### Application-Level
- Sandbox escape prevention
- Permission enforcement
- Syscall filtering
- Resource exhaustion prevention

### Network-Level
- DDoS protection
- ARP spoofing prevention
- DNS poisoning prevention
- Man-in-the-middle prevention

### Filesystem-Level
- Corruption detection
- Tamper detection
- Unauthorized modification prevention
- Privilege escalation via files

## Performance Optimizations

1. **Per-CPU data structures** - Reduce cache invalidation
2. **Lock-free algorithms** - Where applicable
3. **Hardware acceleration** - Crypto, memory protection
4. **Lazy initialization** - Defer setup until needed
5. **Caching** - File metadata, page tables

## Testing Strategy

1. **Unit Tests** - Per module
2. **Integration Tests** - Cross-module
3. **Security Tests** - Exploit mitigations
4. **Performance Tests** - Benchmark critical paths
5. **Stress Tests** - High load scenarios

## Documentation

- **CLAUDE.md** - Build instructions and quick reference
- **ARCHITECTURE.md** - Kernel architecture details
- **USERSPACE_MODULES.md** - Userspace module specifications
- **COMPLETE_ARCHITECTURE.md** - This file
- **Header files** - API documentation with comments

## Future Roadmap

### Phase 1 (Current)
- Core kernel with 9 modules
- System services (9 modules)
- Security layer (6 modules)
- Filesystem extensions (6 modules)
- Userland basics (5 components)

### Phase 2
- Bootloader implementation
- SMP (multi-processor) support
- WebAssembly runtime
- Container support
- Virtual machine support

### Phase 3
- Distributed filesystem
- Cluster management
- Advanced ML-based anomaly detection
- Blockchain-based audit logs
- Quantum-resistant crypto migration

## Licensing & Contributing

- **License**: To be determined (likely GPL/MIT hybrid)
- **Repository**: To be hosted on GitHub
- **Contributing**: Community contributions welcome

## Contact & Support

For questions or contributions:
- Documentation: See included .md files
- Code References: Use line numbers in format `file:line`
- Build Issues: Check CLAUDE.md or create issue

---

**Last Updated:** 2025-11-24
**Total Lines of Code:** ~15,000+ lines
**Modules:** 41 total (9 kernel + 9 services + 6 security + 6 filesystem + 5 userland + 6 common)
**Architectures Supported:** x86_64, ARM (v8-A)
**Status:** Foundation Complete - Ready for Implementation
