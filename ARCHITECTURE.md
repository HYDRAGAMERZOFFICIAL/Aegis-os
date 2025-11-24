# Aegis OS Kernel Architecture

## Overview

Aegis OS is a full-featured operating system kernel with support for multiple CPU architectures (x86-64, ARM) and advanced security features. The kernel implements a modular architecture with 9 core subsystems.

## Core Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    KERNEL LAYER                             │
├──────────┬──────────┬──────────┬──────────┬───────────────┤
│ Process  │ Memory   │ Schedule │ Interrupt│ Security &   │
│ Manager  │ Manager  │  (CFS)   │ Dispatch │  Integrity   │
├──────────┴──────────┴──────────┴──────────┴───────────────┤
│                                                             │
│  Filesystem (AegisFS) │ IPC │ Network Stack │ Drivers     │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│            ARCHITECTURE LAYER (x86_64 / ARM)               │
│  MMU │ GDT/IDT │ Interrupts │ Context Switch │ CPU Control
├─────────────────────────────────────────────────────────────┤
│            COMMON UTILITIES LAYER                          │
│  String │ Math │ Bitmap │ RBTree │ List │ Crypto         │
└─────────────────────────────────────────────────────────────┘
```

## Module Responsibilities

### 1. Process Manager (PMGR)
**Location:** `kernel/process.c`

Manages process and thread lifecycle:
- Process creation with isolated address spaces
- Thread creation with configurable priorities
- Thread scheduling state machine
- CPU affinity management
- Multi-queue run queues per CPU

**Key Structures:**
```c
process_t {
    u64 pid;
    thread_t *threads;  // Thread list
    void *page_table;   // Address space
    u32 priority;
    u32 thread_count;
}

thread_t {
    u64 tid;
    u64 pid;
    process_state_t state;
    u32 priority;
    u32 cpu_affinity;
}
```

### 2. Memory Manager (MMGR)
**Location:** `kernel/memory.c`

Provides virtual memory management:
- Physical page allocation/deallocation
- Virtual memory mapping with protection flags
- Address Space Layout Randomization (ASLR)
- Page encryption support
- Secure memory zeroing

**Key Features:**
- 4KB page size
- Bitmap-based page tracking
- Multiple protection levels (R/W/X)
- Memory sandboxing per process

### 3. Aegis Scheduler
**Location:** `kernel/scheduler.c`

Implements hybrid scheduling algorithm:
- **Completely Fair Scheduler (CFS)** - For normal processes
- **Priority Scheduling** - For real-time processes
- **Deadline Scheduling** - For deterministic workloads
- Energy-aware CPU scaling
- Per-CPU run queue load balancing

**Scheduling Classes:**
- `SCHED_CLASS_RT` - Real-time
- `SCHED_CLASS_FAIR` - Fair share (CFS)
- `SCHED_CLASS_DEADLINE` - Hard deadlines

### 4. Interrupt & Event Dispatcher (IED)
**Location:** `kernel/interrupt.c`

Manages hardware/software interrupts and events:
- IRQ registration and routing
- Event queue for async event processing
- Low-latency interrupt handling
- IRQ affinity control
- Interrupt statistics tracking

**Event Types:**
- Timer events
- I/O events
- Signal events
- IPC events

### 5. AegisFS Kernel Module
**Location:** `kernel/filesystem.c`

Transaction-based secure filesystem:
- ACID transaction support
- Journaling for crash consistency
- File encryption capability
- Inode-based file management
- Tamper-proof logging

**Transaction States:**
```
BEGIN → COMMIT (success)
     → ROLLBACK (failure)
```

### 6. Kernel IPC Module
**Location:** `kernel/ipc.c`

Inter-process communication:
- Message queues (async messaging)
- Pipes (stream communication)
- Shared memory (fast data sharing)
- Semaphores (synchronization)
- Signal handlers

**Secure Features:**
- Process boundary enforcement
- Secure IPC object tagging
- Signal masking support

### 7. Network Stack Module
**Location:** `kernel/network.c`

Layered network implementation:
- Socket abstraction
- TCP/UDP protocol support (simplified)
- Built-in firewall hooks
- Default socket encryption
- Network packet handling

**Protocol Support:**
- Ethernet (Layer 2)
- IP (Layer 3)
- TCP/UDP (Layer 4)
- ICMP (diagnostics)

### 8. Device Driver Manager
**Location:** `kernel/driver.c`

Manages driver loading and isolation:
- Driver registration and lifecycle
- Sandbox modes:
  - `SANDBOX_MODE_NONE` - No sandboxing
  - `SANDBOX_MODE_WASM` - WebAssembly sandbox
  - `SANDBOX_MODE_VM` - Virtual machine sandbox
  - `SANDBOX_MODE_CONTAINER` - Container isolation
- Permission-based access control
- Driver state management

### 9. Kernel Security & Integrity Monitor
**Location:** `kernel/security.c`

Real-time security monitoring:
- Kernel code integrity verification
- System call monitoring and logging
- Process behavior analysis
- Anomaly detection (frequency, pattern, resource, privilege, network)
- Runtime integrity reports

**Anomaly Types:**
- Frequency anomalies (excessive syscalls)
- Pattern anomalies (suspicious call sequences)
- Resource anomalies (excessive allocation)
- Privilege anomalies (unauthorized access)
- Network anomalies (suspicious connections)

## Architecture-Specific Implementation

### x86_64 Architecture
**Location:** `arch/x86_64/`

**Memory Protection:**
- Paging (64-bit page tables)
- NXE - No-Execute bit
- SMAP - Supervisor Mode Access Prevention
- SMEP - Supervisor Mode Execution Prevention
- ASLR - Address Space Layout Randomization

**Interrupt Handling:**
- GDT (Global Descriptor Table) setup
- IDT (Interrupt Descriptor Table) with 256 entries
- TSS (Task State Segment) for ring switching
- Exception handlers for divide-by-zero, GPF, etc.

**Context Switching:**
- 64-bit register save/restore
- Kernel stack management
- User/kernel mode switching

### ARM Architecture
**Location:** `arch/arm/`

**Memory Protection:**
- ARMv8 MMU setup
- TLB invalidation
- Branch Target Address Cache (BTAC)
- Execute-Never (PXN) bit

**Interrupt Handling:**
- GIC (Generic Interrupt Controller) v2/v3
- Exception vector table setup
- SGI (Software Generated Interrupts)
- FIQ/IRQ distinction

**Context Switching:**
- 64-bit register save/restore
- EL1 exception handlers
- Stack pointer management

## Common Utilities

### Data Structures
- **Red-Black Tree** - Efficient O(log n) searching
- **Linked List** - Dynamic data storage
- **Bitmap** - Efficient bit manipulation

### Algorithms
- String operations (memcpy, strcmp, strlen)
- Math utilities (min, max, log2, gcd, lcm)
- Cryptographic stubs (SHA-1, SHA-256, AES)

## Security Features

1. **Memory Protection:**
   - Process isolation through virtual memory
   - ASLR for exploit mitigation
   - Page encryption for sensitive data
   - Secure memory zeroing

2. **Process Isolation:**
   - Independent address spaces
   - Protected inter-process communication
   - Resource quotas

3. **Interrupt Protection:**
   - Privilege level enforcement
   - Exception handling validation

4. **Filesystem Security:**
   - Transaction-based consistency
   - File encryption
   - Tamper-proof logging

5. **Driver Isolation:**
   - WebAssembly sandboxing
   - Permission masks
   - Resource limitations

6. **Runtime Monitoring:**
   - Syscall tracing
   - Behavior analysis
   - Real-time anomaly detection

## Configuration Limits

```c
#define MAX_CPUS 256
#define MAX_PROCESSES 4096
#define MAX_THREADS_PER_PROCESS 256
#define MAX_IRQ_HANDLERS 256
#define PAGE_SIZE 4096
```

## Compilation Targets

**x86_64 Build:**
```bash
cmake -DARCH=x86_64 -DCMAKE_BUILD_TYPE=Release ..
make
```

**ARM Build:**
```bash
cmake -DARCH=arm -DCMAKE_BUILD_TYPE=Release ..
make
```

## Performance Considerations

1. **Scheduling:** CFS provides fair share while allowing priority boost
2. **Memory:** Per-CPU allocation reduces contention
3. **Interrupts:** Per-CPU run queues prevent cache invalidation
4. **IPC:** Shared memory faster than message passing
5. **Filesystem:** Transactions ensure consistency without excessive locking

## Future Enhancements

1. Symmetric Multi-Processing (SMP) with fine-grained locking
2. NUMA support for multi-socket systems
3. Virtual machine support
4. Container orchestration
5. Advanced cgroup resource management
6. Enhanced cryptographic support
7. Device pass-through capabilities
8. Real-time kernel variant (PREEMPT_RT)
