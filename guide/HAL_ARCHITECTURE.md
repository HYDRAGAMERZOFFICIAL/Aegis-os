# Aegis OS Hardware Abstraction Layer (HAL) Architecture

## Overview

The Hardware Abstraction Layer (HAL) is a critical component that sits between the Aegis OS kernel core and device drivers, providing stable, platform-agnostic APIs while encapsulating platform-specific details. The HAL normalizes hardware differences and presents a consistent interface to the kernel subsystems.

## Architecture Principles

1. **Platform Abstraction**: Normalize hardware differences across x86_64, x86_32, ARM64, and ARM32
2. **Consistent API Design**: All modules follow the same naming conventions and error handling patterns
3. **Static State Management**: Each module maintains a static structure containing all state and configuration
4. **Fixed-Size Allocations**: Use arrays with maximum limits for embedded system compatibility
5. **Error Handling**: Return -1/HAL_OK for errors, NULL for allocation failures
6. **Type Safety**: Proper use of typedef structs and enums throughout

## Directory Structure

```
hal/
├── hal.c                    # Main HAL coordinator
├── hal_cpu.c               # CPU abstraction
├── hal_firmware.c          # ACPI/UEFI support
├── hal_gpu.c               # GPU abstraction
├── hal_audio.c             # Audio subsystem
├── hal_storage.c           # Storage subsystem
├── hal_usb.c               # USB/HID subsystem
├── hal_power.c             # Power management
└── CMakeLists.txt          # Build configuration

include/hal/
├── hal.h                   # Main HAL header
├── hal_cpu.h              # CPU API definitions
├── hal_firmware.h         # Firmware API definitions
├── hal_gpu.h              # GPU API definitions
├── hal_audio.h            # Audio API definitions
├── hal_storage.h          # Storage API definitions
├── hal_usb.h              # USB API definitions
└── hal_power.h            # Power API definitions
```

## Core Modules

### 1. CPU Abstraction (hal_cpu.h/hal_cpu.c)

**Purpose**: Portable interface to CPU features, topology, and low-level CPU control.

**Key APIs**:
- `hal_cpu_enumerate()` - Get CPU topology
- `hal_cpu_has_feature()` - Check CPU capabilities
- `hal_cpu_read_msr()` / `hal_cpu_write_msr()` - MSR access
- `hal_cpu_send_ipi()` - Inter-processor interrupts
- `hal_cpu_get/set_pstate()` - Power state control
- `hal_cpu_enable_virtualization()` - VM support

**Features**:
- Multi-package, multi-core, multi-thread topology
- CPUID feature detection
- Per-CPU data storage
- Frequency scaling and Turbo Boost control
- Virtualization feature exposure
- Cache and TLB management

### 2. Firmware Support (hal_firmware.h/hal_firmware.c)

**Purpose**: Abstract UEFI/BIOS, ACPI, SMBIOS, and TPM interfaces.

**Key APIs**:
- `hal_firmware_get_info()` - Firmware information
- `hal_firmware_is_secure_boot_enabled()` - Secure boot status
- `hal_acpi_get_table()` - ACPI table access
- `hal_firmware_validate_signature()` - Signature verification
- `hal_firmware_extend_pcr()` / `hal_firmware_read_pcr()` - TPM PCR operations
- `hal_smbios_get_entry()` - SMBIOS data retrieval

**Features**:
- UEFI variable access
- ACPI method evaluation
- SMBIOS table enumeration
- TPM 2.0 support
- Secure Boot management
- Microcode updates

### 3. GPU Abstraction (hal_gpu.h/hal_gpu.c)

**Purpose**: Vendor-agnostic GPU control interface.

**Supported GPUs**:
- NVIDIA (with compute capability detection)
- AMD RDNA/CDNA
- Intel integrated/discrete
- ARM Mali
- Qualcomm Adreno

**Key APIs**:
- `hal_gpu_enumerate()` - Device discovery
- `hal_gpu_create_context()` - Context management
- `hal_gpu_allocate_memory()` / `hal_gpu_free_memory()` - VRAM management
- `hal_gpu_submit_kernel()` - Kernel execution
- `hal_gpu_copy_to_device()` / `hal_gpu_copy_to_host()` - DMA transfers
- `hal_gpu_enable_ecc()` - ECC control
- `hal_gpu_enable_preemption()` - Preemption support

**Features**:
- Multi-GPU support
- Per-context memory isolation
- Async compute support
- Cache management
- Temperature monitoring
- Preemptive kernels

### 4. Audio Subsystem (hal_audio.h/hal_audio.c)

**Purpose**: Unified audio device and stream management.

**Supported Formats**:
- PCM (S16, S24, S32, Float)
- Compressed audio
- Multi-channel (Mono, Stereo, 5.1, 7.1)

**Key APIs**:
- `hal_audio_enumerate_devices()` - Device discovery
- `hal_audio_create_stream()` - Stream creation
- `hal_audio_play()` / `hal_audio_record()` - Audio I/O
- `hal_audio_set_volume()` - Volume control
- `hal_audio_apply_effect()` - Audio effects
- `hal_audio_register_callback()` - Event callbacks

**Supported Devices**:
- Speakers, Headphones
- Microphones
- Line In/Out
- HDMI audio
- S/PDIF

**Features**:
- Multi-stream support
- Volume per-device
- Sample rate conversion
- Audio effects framework
- Latency measurement

### 5. Storage Subsystem (hal_storage.h/hal_storage.c)

**Purpose**: Unified interface for multiple storage types.

**Supported Interfaces**:
- SATA (AHCI)
- NVMe (with admin command support)
- eMMC/SD cards
- USB Mass Storage
- RAM disks

**Key APIs**:
- `hal_storage_enumerate_devices()` - Device discovery
- `hal_storage_read_sync()` / `hal_storage_write_sync()` - Synchronous I/O
- `hal_storage_read_async()` / `hal_storage_write_async()` - Asynchronous I/O
- `hal_storage_get_smart_info()` - SMART data
- `hal_storage_trim()` - TRIM support
- `hal_storage_nvme_submit_admin_command()` - NVMe admin
- `hal_storage_lock_device()` / `hal_storage_unlock_device()` - Security

**Features**:
- Multi-protocol support
- Async I/O with callbacks
- SMART monitoring
- ATA/NVMe specific commands
- Device health reporting
- Sector-level locking

### 6. USB/HID Subsystem (hal_usb.h/hal_usb.c)

**Purpose**: Unified USB device and HID interface.

**Supported Speeds**:
- Low Speed (1.5 Mbps)
- Full Speed (12 Mbps)
- High Speed (480 Mbps)
- Super Speed (5 Gbps)
- Super Speed+ (10 Gbps)

**Transfer Types**:
- Control transfers
- Bulk transfers
- Interrupt transfers
- Isochronous transfers (async)

**Key APIs**:
- `hal_usb_enumerate_devices()` - Device discovery
- `hal_usb_control_transfer()` - Control transfers
- `hal_usb_bulk_transfer()` / `hal_usb_bulk_transfer_async()` - Bulk I/O
- `hal_usb_interrupt_transfer()` - Interrupt transfers
- `hal_hid_get_report()` / `hal_hid_set_report()` - HID protocol
- `hal_usb_reset_device()` - Device reset

**HID Features**:
- Report ID support
- Idle rate management
- Protocol switching (boot/report)
- Stall detection and recovery

**Features**:
- Multi-bus support
- Device enumeration with callbacks
- Configuration/interface selection
- Endpoint management
- Power management per device

### 7. Power Management (hal_power.h/hal_power.c)

**Purpose**: System-wide power management and thermal control.

**Power States**:
- S0: Running
- S1-S3: Sleep states
- S4: Hibernation
- S5: Shutdown
- G3: Off

**CPU Idle States**:
- C0: Active
- C1-C10: Increasingly deep sleep states

**Key APIs**:
- `hal_power_get_system_state()` / `hal_power_set_system_state()` - System state
- `hal_power_set_pstate()` - P-state control (frequency scaling)
- `hal_power_set_cpu_idle_state()` - C-state control
- `hal_power_enable_turbo()` / `hal_power_disable_turbo()` - Turbo Boost
- `hal_battery_get_info()` - Battery information
- `hal_battery_set_charge_threshold()` - Charging limits
- `hal_thermal_get_zone_info()` - Thermal data
- `hal_fan_set_speed()` - Fan control
- `hal_power_get_metrics()` - Real-time power metrics

**Features**:
- Dynamic frequency scaling
- Turbo Boost management
- Battery health monitoring
- Thermal zone management
- Fan speed control
- Power profiling
- Event callbacks for power/thermal/battery events

## Common API Patterns

### Error Handling

All HAL functions return `hal_status_t`:
```c
#define HAL_OK                  0
#define HAL_ERR_INVALID_ARG    -1
#define HAL_ERR_NOT_SUPPORTED  -2
#define HAL_ERR_DEVICE_BUSY    -3
#define HAL_ERR_NO_MEMORY      -4
#define HAL_ERR_TIMEOUT        -5
#define HAL_ERR_DEVICE_FAILED  -6
#define HAL_ERR_PERMISSION_DENIED -7
```

### State Management

Each module maintains static state:
```c
typedef struct {
    // Configuration
    // Device list
    // Counters
    bool initialized;
} module_state_t;

static module_state_t state = {0};
```

### Callbacks

Many modules support callbacks for asynchronous events:
```c
typedef void (*hal_callback_t)(event_type_t event, void *data, void *context);

hal_status_t hal_register_callback(hal_callback_t callback, void *context);
```

## Integration Points with Kernel

### Scheduler
- Uses CPU topology from `hal_cpu_enumerate()`
- Sets CPU affinity based on cache topology
- Manages frequency scaling via `hal_power_set_pstate()`

### Memory Manager
- Integrates with IOMMU via `hal_enable_iommu()`
- Uses hardware page tables from firmware
- Manages NUMA from CPU topology

### Device Driver Manager
- Enumerates all devices (GPU, Audio, Storage, USB)
- Provides device initialization hooks
- Manages device power states

### Security Subsystem
- Validates firmware via TPM/PCR
- Manages Secure Boot settings
- Coordinates with HSK for sealed keys

### Power Manager
- Controls system sleep states
- Manages battery/thermal events
- Collects power metrics

## Security Considerations

1. **MSR Protection**: Only kernel/HSK authorized code can access MSRs
2. **Firmware Validation**: All firmware updates validated via HSK-signed packages
3. **User-Space Isolation**: No direct user-space access to HAL functions
4. **TPM Integration**: Hardware attestation support
5. **IOMMU Support**: Device isolation through I/O virtualization

## Testing and Verification

### Unit Tests
- Feature detection parsing
- Error handling paths
- State machine transitions

### Stress Tests
- Cross-CPU IPI latency
- Context switch correctness
- Cache coherency

### Microbenchmarks
- IPI latency measurement
- MSR read/write performance
- Device enumeration speed

## Build and Integration

### Compilation
```bash
cmake -B build -DARCH=x86_64
cmake --build build
```

### Linking
The `hal` library is automatically linked to the kernel in `kernel/CMakeLists.txt`.

## Extending HAL

To add support for a new device type:

1. Create `include/hal/hal_newdevice.h` with API definitions
2. Create `hal/hal_newdevice.c` with implementations
3. Add module-specific state structure
4. Implement discovery and control APIs
5. Update `hal.c` to initialize the new module
6. Update `hal/CMakeLists.txt` to include new files
7. Update `hal_init()` and `hal_fini()` functions

## Performance Characteristics

- **hal_cpu_send_ipi()**: < 1 microsecond
- **hal_cpu_read_msr()**: < 100 nanoseconds
- **hal_gpu_allocate_memory()**: < 1 microsecond
- **hal_storage_read_sync()**: Device dependent (typically 100 microseconds - milliseconds)
- **hal_usb_bulk_transfer()**: Device dependent (typically 100 microseconds - milliseconds)

## Future Enhancements

1. **Heterogeneous Computing**: Support for asymmetric CPU cores
2. **Advanced Thermal Management**: ML-based thermal prediction
3. **Network Abstraction**: HAL layer for network devices
4. **Memory Tiering**: Support for HBM and other memory types
5. **PCI-E Gen 5 Support**: Latest PCIe topology discovery
