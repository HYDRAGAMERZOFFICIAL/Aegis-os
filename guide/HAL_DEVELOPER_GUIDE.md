# Aegis OS HAL Developer Guide

## Overview

The Aegis OS HAL has been restructured with a modular Rust-based architecture featuring a driver model, capability system, and device abstraction layers.

## Architecture Layers

```
┌─────────────────────────────────────┐
│   User Space Applications           │
├─────────────────────────────────────┤
│   Kernel Services & Drivers         │
├─────────────────────────────────────┤
│   Device Node Layer (devfs/sysfs)   │
├─────────────────────────────────────┤
│   HAL Subsystems                    │
│   (CPU, GPU, Audio, Block, USB...)  │
├─────────────────────────────────────┤
│   Driver Model & Capability System  │
├─────────────────────────────────────┤
│   Hardware (ACPI/UEFI/Firmware)     │
└─────────────────────────────────────┘
```

## Driver Registration Pattern

### Example: GPU Driver Registration

```rust
use kernel_hal::driver::{DriverDescriptor, DriverCallbackType};
use kernel_hal::error::Result;

extern "C" fn gpu_driver_init(
    device: *const core::ffi::c_void,
    callback_type: DriverCallbackType,
    callback_data: *const core::ffi::c_void,
) -> i32 {
    // Initialize GPU driver
    0
}

extern "C" fn gpu_driver_probe(
    device: *const core::ffi::c_void,
    callback_type: DriverCallbackType,
    callback_data: *const core::ffi::c_void,
) -> i32 {
    // Probe GPU device
    0
}

fn register_gpu_driver() -> Result<()> {
    let descriptor = DriverDescriptor {
        driver_id: 0x02,
        driver_name: b"amdgpu\0".as_ptr(),
        version: 0x010000,
        flags: 0x00,
        init: Some(gpu_driver_init),
        probe: Some(gpu_driver_probe),
        suspend: None,
        resume: None,
        remove: None,
        priority: 100,
    };

    hal_driver_register(&descriptor)
}
```

## Device Node Patterns

### devfs Implementation

Implement in `/kernel/hal/devfs/mod.rs`:

```rust
pub struct DevfsNode {
    pub path: [u8; 256],
    pub device_id: u32,
    pub permissions: u16,
    pub callbacks: DevfsCallbacks,
}

pub struct DevfsCallbacks {
    pub open: Option<fn(*const c_void) -> Result<()>>,
    pub close: Option<fn(*const c_void) -> Result<()>>,
    pub read: Option<fn(*const c_void, *mut u8, u32) -> Result<u32>>,
    pub write: Option<fn(*const c_void, *const u8, u32) -> Result<u32>>,
    pub ioctl: Option<fn(*const c_void, u32, *const c_void) -> Result<()>>,
}

pub struct DevfsManager {
    nodes: [Option<DevfsNode>; 256],
}

impl DevfsManager {
    pub fn register_device(
        &mut self,
        path: &str,
        device_id: u32,
        callbacks: DevfsCallbacks,
    ) -> Result<()> {
        // Register device node under /dev/
        Ok(())
    }

    pub fn unregister_device(&mut self, path: &str) -> Result<()> {
        Ok(())
    }
}
```

Device node layout:
```
/dev/
├── block/
│   ├── nvme0n1          (NVMe block device)
│   ├── sda              (SATA disk)
│   └── mmcblk0          (SD card)
├── gpu/
│   ├── card0            (Primary GPU)
│   ├── card1            (Secondary GPU)
│   └── renderD128       (GPU render node)
├── audio/
│   ├── pcm0p            (PCM playback)
│   ├── pcm0c            (PCM capture)
│   └── mixer0           (Mixer)
├── usb/
│   ├── bus0             (USB bus 0)
│   ├── bus1             (USB bus 1)
│   └── device1-1.2      (USB device)
└── input/
    ├── event0           (Keyboard)
    ├── event1           (Mouse)
    └── event2           (Touch)
```

### sysfs Implementation

Implement in `/kernel/hal/sysfs/mod.rs`:

```rust
pub struct SysfsEntry {
    pub path: [u8; 256],
    pub value: [u8; 1024],
    pub read_only: bool,
}

pub struct SysfsManager {
    entries: [Option<SysfsEntry>; 512],
}

impl SysfsManager {
    pub fn register_entry(
        &mut self,
        path: &str,
        initial_value: &[u8],
        read_only: bool,
    ) -> Result<()> {
        Ok(())
    }

    pub fn read_entry(&self, path: &str) -> Result<&[u8]> {
        Ok(b"")
    }

    pub fn write_entry(&mut self, path: &str, value: &[u8]) -> Result<()> {
        Ok(())
    }
}
```

sysfs node layout:
```
/sys/
└── hal/
    ├── cpu/
    │   ├── 0/
    │   │   ├── frequency_mhz (RO)
    │   │   ├── temperature   (RO)
    │   │   ├── idle_cycles   (RO)
    │   │   └── power_state   (RW)
    │   ├── 1/
    │   └── ...
    ├── gpu/
    │   ├── card0/
    │   │   ├── available_memory (RO)
    │   │   ├── allocated_memory (RO)
    │   │   └── current_frequency (RW)
    │   └── ...
    ├── power/
    │   ├── battery_status    (RO)
    │   ├── battery_capacity  (RO)
    │   ├── ac_connected      (RO)
    │   └── power_profile     (RW)
    ├── devices/
    │   └── (registry of all hardware devices)
    └── drivers/
        └── (registry of all registered drivers)
```

## Capability Token Usage

### Example: Audio Application with Capture Permission

```rust
use kernel_hal::capability::{CapabilitySystem, Permission};

fn main() {
    let mut cap_system = CapabilitySystem::new();

    let container_id = 1;
    let app_id = 42;
    let permissions = Permission::CAPTURE.bits() | Permission::PLAYBACK.bits();

    match cap_system.request_token(container_id, app_id, permissions) {
        Ok(token) => {
            if cap_system.has_permission(&token, Permission::CAPTURE) {
                // Can capture audio
            }
        }
        Err(_) => {
            // Permission denied
        }
    }
}
```

### Example: Storage Device Access with Admin Token

```rust
fn access_storage_device() -> Result<()> {
    let mut cap_system = CapabilitySystem::new();

    let admin_perms = Permission::READ.bits()
        | Permission::WRITE.bits()
        | Permission::ADMIN.bits();

    let token = cap_system.request_token(0, 0, admin_perms)?;
    cap_system.verify_token(&token)?;

    if cap_system.has_permission(&token, Permission::ADMIN) {
        // Can perform admin operations
    }

    Ok(())
}
```

## Subsystem Integration Checklist

For each subsystem module (ACPI, UEFI, GPU, Audio, Block, USB, Power):

- [ ] **Module Structure**
  - [ ] `lib.rs` with public exports
  - [ ] `subsystem.rs` with main struct
  - [ ] Type definitions (data structures)
  - [ ] Error handling

- [ ] **Driver Registration**
  - [ ] Implement `DriverDescriptor`
  - [ ] Register with `DriverRegistry`
  - [ ] Implement all callback functions
  - [ ] Set appropriate priority

- [ ] **Device Management**
  - [ ] Enumerate devices on init
  - [ ] Create DevfsNodes for each device
  - [ ] Register SysfsEntries for device info
  - [ ] Implement device callbacks

- [ ] **Capability Integration**
  - [ ] Define required permissions
  - [ ] Check tokens before operations
  - [ ] Log permission denials
  - [ ] Handle token expiry

- [ ] **Testing**
  - [ ] Unit tests for subsystem
  - [ ] Driver registration tests
  - [ ] Device enumeration tests
  - [ ] Capability token tests

## Common Patterns

### Static Device Registration

```rust
pub struct AudioDevice {
    id: u32,
    name: [u8; 256],
}

pub struct AudioSubsystem {
    devices: [Option<AudioDevice>; 32],
    device_count: usize,
}

impl AudioSubsystem {
    pub fn register_device(&mut self, device: AudioDevice) -> Result<()> {
        if self.device_count >= 32 {
            return Err(HalError::NoMemory);
        }
        self.devices[self.device_count] = Some(device);
        self.device_count += 1;
        Ok(())
    }
}
```

### Callback Wrapper

```rust
pub trait DeviceCallback {
    fn on_connect(&self, device_id: u32) -> Result<()>;
    fn on_disconnect(&self, device_id: u32) -> Result<()>;
    fn on_error(&self, device_id: u32, error: HalError) -> Result<()>;
}
```

### Per-Device State

```rust
pub struct DeviceState {
    pub id: u32,
    pub active: bool,
    pub power_state: u8,
    pub error_count: u32,
    pub last_error: HalError,
}

pub struct DeviceManager {
    states: [Option<DeviceState>; 256],
}
```

## Best Practices

1. **Memory Safety**: Use pre-allocated arrays for embedded systems
2. **No Allocations**: Avoid dynamic allocation in hot paths
3. **Error Handling**: Always return `Result<T>` from public APIs
4. **Type Safety**: Use strong types (`#[newtype]` pattern)
5. **Capabilities**: Always verify tokens before sensitive ops
6. **Logging**: Include debug output for driver transitions
7. **Cleanup**: Implement proper cleanup in remove/shutdown
8. **Atomicity**: Use volatile for hardware register access

## Migration Path for Existing C Drivers

1. Create corresponding Rust module structure
2. Wrap C code with `extern "C"` FFI layer
3. Gradually rewrite modules in Rust
4. Register via new DriverDescriptor system
5. Create DevfsNodes for compatibility
6. Update tests to use new interfaces

## Testing Strategy

### Unit Tests

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_driver_registration() {
        let mut registry = DriverRegistry::new();
        let descriptor = create_test_descriptor();
        assert!(registry.register(&descriptor).is_ok());
    }

    #[test]
    fn test_capability_token_creation() {
        let mut cap_system = CapabilitySystem::new();
        let token = cap_system.request_token(1, 1, 0x01);
        assert!(token.is_ok());
    }
}
```

### Integration Tests

```bash
# Test driver probe
./test_driver_probe

# Test device enumeration
./test_device_enumeration

# Test capability system
./test_capability_tokens

# Test devfs/sysfs operations
./test_device_nodes
```

## Performance Considerations

- **Device Enumeration**: O(n) linear search acceptable for <256 devices
- **Capability Verification**: O(1) token lookup via hash (future)
- **Driver Priority**: Sort at registration time, O(1) probe dispatch
- **Hot Path**: Minimize register accesses in callbacks

## Future Enhancements

1. **Device Tree Support**: Parse device tree for device discovery
2. **Hotplug Events**: Async device add/remove notifications
3. **Power Management**: Integrated suspend/resume coordination
4. **Dynamic Modules**: Load drivers at runtime
5. **Telemetry**: Device health monitoring
6. **Formal Verification**: Prove safety properties of critical sections
