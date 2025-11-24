# Aegis OS - Driver Installation Guide

## Overview

The Aegis OS driver system uses a comprehensive driver manager that handles driver installation, loading, unloading, and lifecycle management. All drivers are sandboxed in WebAssembly for security.

## Driver Categories

### 1. Input Drivers
**Module**: `drivers/input_driver.c`
**Functions**:
- `input_driver_init()` - Initialize input subsystem
- `input_driver_enumerate_devices()` - Detect connected input devices
- `input_driver_register_device(type, name)` - Register device
- `input_driver_unregister_device(id)` - Unregister device
- `input_driver_read_event(id, event)` - Read input event
- `input_driver_set_sensitivity(id, level)` - Configure sensitivity

**Supported Hardware**:
- Keyboards (USB, PS/2, Wireless)
- Mice (Optical, Laser, Trackpads)
- Touchscreens
- Touchpads
- Styluses
- Game controllers

### 2. GPU Drivers
**Module**: `drivers/gpu_driver.c`
**Functions**:
- `gpu_driver_init()` - Initialize GPU
- `gpu_driver_enumerate_devices()` - List available GPUs
- `gpu_driver_set_mode(width, height, bpp)` - Set display mode
- `gpu_driver_create_buffer(size)` - Create GPU memory buffer
- `gpu_driver_submit_command_buffer(cmds)` - Submit drawing commands
- `gpu_driver_present_frame()` - Display rendered frame

**Supported APIs**:
- Vulkan (preferred for security)
- OpenGL 4.6
- Direct3D 12 (Windows compatibility)

**Supported GPUs**:
- NVIDIA (CUDA compute)
- AMD (RDNA architecture)
- Intel (Arc discrete GPU)
- Apple Silicon (M-series)

### 3. Storage Drivers
**Module**: `drivers/storage_driver.c`
**Functions**:
- `storage_driver_init()` - Initialize storage subsystem
- `storage_driver_enumerate_devices()` - Detect storage devices
- `storage_driver_read_sector(device, lba, count, buffer)` - Read sectors
- `storage_driver_write_sector(device, lba, count, buffer)` - Write sectors
- `storage_driver_trim(device, lba, count)` - TRIM support
- `storage_driver_enable_encryption(device)` - Enable encryption

**Supported Hardware**:
- **NVMe**: M.2, U.2, AIC form factors
- **SATA**: SSDs and HDDs
- **USB**: External drives, USB sticks
- **SD/microSD**: Memory cards
- **eMMC**: Embedded storage

**Encryption Support**:
- AES-256-XTS (default for all external drives)
- ChaCha20 (alternative)
- Full-disk encryption support

### 4. Network Drivers
**Module**: `drivers/network_driver.c`
**Functions**:
- `network_driver_init()` - Initialize network subsystem
- `network_driver_enumerate_devices()` - Detect network adapters
- `network_driver_connect(ssid, password)` - Connect to Wi-Fi
- `network_driver_disconnect()` - Disconnect
- `network_driver_send_packet(data, len)` - Send data
- `network_driver_receive_packet(buffer, max_len)` - Receive data
- `network_driver_scan_networks()` - Scan available networks

**Supported Hardware**:
- **Wi-Fi**: Wi-Fi 6 (802.11ax), Wi-Fi 5 (802.11ac)
- **Ethernet**: Gigabit, 10GbE
- **Bluetooth**: 5.3 with enhanced security
- **4G/5G**: Cellular modems

**Security Features**:
- WPA3 support
- Threat detection on network level
- Automatic firewall filtering

### 5. Sensor Drivers
**Module**: `drivers/sensor_driver.c`
**Functions**:
- `sensor_driver_init()` - Initialize sensor subsystem
- `sensor_driver_request_permission(sensor_type)` - Request access (with user prompt)
- `sensor_driver_grant_permission(app_id, sensor_type)` - Grant permission
- `sensor_driver_read_sensor(type, data)` - Read sensor data
- `sensor_driver_set_sampling_rate(type, hz)` - Set sampling frequency

**Supported Sensors**:
- **Camera**: Front, rear, IR with privacy indicators
- **Microphone**: Primary, secondary with mute hardware control
- **GPS**: Accurate location with privacy mode
- **Accelerometer/Gyroscope**: IMU data
- **Barometer**: Altitude
- **Light Sensor**: Ambient light
- **Proximity Sensor**: Physical proximity
- **Fingerprint**: Biometric authentication

**Permission System**:
- Strict permission-based access
- Per-app permissions
- User revocation at any time
- Activity indicators when sensors are in use

## Driver Installation Procedure

### Step 1: Prepare Driver Package
```
driver_name.aegis
├── driver.json (metadata)
├── driver.wasm (compiled module)
└── firmware/ (optional)
    ├── firmware_v1.bin
    └── config.ini
```

### Step 2: Verify Driver
```c
int result = driver_verify_signature("/path/to/driver.aegis");
// Returns 1 if valid, 0 if invalid/untrusted
```

### Step 3: Install Driver
```c
int result = driver_install(
    "nvidia_gpu_driver",
    "/drivers/nvidia_gpu_driver.aegis",
    DRIVER_CLASS_GPU
);
```

### Step 4: Load Driver
```c
int result = driver_load("nvidia_gpu_driver");
// Returns 0 on success
```

### Step 5: Configure (Optional)
```c
driver_set_permissions("nvidia_gpu_driver", GPU_PERMISSION_RENDER);
driver_enable("nvidia_gpu_driver");
```

## Complete Driver Installation List

### Essential Drivers (Required for Boot)
1. **Platform Controller Hub (PCH) Driver**
   - Chipset initialization
   - Power management
   - System management

2. **Memory Initialization**
   - DRAM controller
   - Memory configuration

3. **Primary Storage Driver**
   - Boot device access
   - SATA or NVMe

### Standard Drivers (Recommended)

#### Input & Display
- `input_keyboard_ps2` - PS/2 keyboard
- `input_mouse_ps2` - PS/2 mouse
- `input_usb_generic` - USB input devices
- `gpu_intel_iris` - Intel Iris GPU
- `gpu_nvidia_cuda` - NVIDIA discrete GPU
- `gpu_amd_rdna` - AMD RDNA GPU

#### Storage
- `storage_nvme_samsung` - Samsung NVMe
- `storage_nvme_western_digital` - WD NVMe
- `storage_sata_ahci` - Generic SATA
- `storage_usb_mass_storage` - USB drives

#### Network
- `network_wifi_intel` - Intel Wi-Fi
- `network_wifi_qualcomm` - Qualcomm Wi-Fi
- `network_ethernet_intel` - Intel Ethernet
- `network_ethernet_realtek` - Realtek Ethernet
- `network_bluetooth` - Bluetooth adapter

#### Sensors (Optional)
- `sensor_camera_intel_realsense` - 3D camera
- `sensor_microphone_realtek` - Audio input
- `sensor_gps_ublox` - GPS receiver

### Development Drivers (For Testing)
- `input_test_driver` - Synthetic input
- `gpu_test_driver` - GPU emulation
- `storage_ramdisk_driver` - RAM disk
- `network_loopback_driver` - Network testing

## Driver Manager API

### Core Functions
```c
// Initialize driver manager
int driver_manager_init(void);

// Install a driver
int driver_install(const char* name, const char* path, driver_class_t class);

// Uninstall a driver
int driver_uninstall(const char* name);

// Load driver into memory
int driver_load(const char* name);

// Unload driver from memory
int driver_unload(const char* name);

// Reload driver
int driver_reload(const char* name);

// Get driver status
int driver_get_status(const char* name);

// Enable/disable driver
int driver_enable(const char* name);
int driver_disable(const char* name);

// Check compatibility
int driver_check_compatibility(const char* name);

// Get driver information
driver_t* driver_get_info(const char* name);

// List drivers by class
driver_t** driver_list_by_class(driver_class_t class, uint32_t* count);

// Update and rollback
int driver_update_available(const char* name);
int driver_install_update(const char* name);
int driver_rollback(const char* name);
```

## Troubleshooting

### Driver Installation Fails
1. **Check signature**: Verify driver package is signed
2. **Check compatibility**: Ensure driver supports your hardware
3. **Check conflicts**: Ensure no conflicting drivers are loaded
4. **Review logs**: Check audit service logs

### Driver Crashes
1. **Check permissions**: Verify driver has necessary permissions
2. **Check resource limits**: Ensure sufficient memory allocated
3. **Rollback**: Use `driver_rollback()` to previous version
4. **Sandbox**: Check WebAssembly sandbox isolation

### Performance Issues
1. **Profile**: Use neural_activity_monitor to check driver resource usage
2. **Optimize**: Check driver configuration options
3. **Update**: Install latest driver version
4. **Isolate**: Run driver in stricter sandbox mode

## Security Considerations

### WebAssembly Sandboxing
- All drivers run in isolated WASM environments
- Memory access is restricted
- Hardware access is mediated through system calls
- No direct kernel memory access possible

### Code Signing
- All drivers must be signed with Aegis key
- Unsigned drivers cannot be loaded
- Signature verification on every load

### Permission System
- Drivers request permissions explicitly
- User can audit and revoke permissions
- Activity logging for all sensitive operations
- Automatic cleanup on uninstall

### Update Security
- Atomic updates (no partial installations)
- Automatic rollback on failure
- Delta updates reduce attack surface
- Signature verification of updates

## Example: Installing NVIDIA GPU Driver

```bash
# Step 1: Download driver
aegis-package-manager install nvidia-gpu-driver

# Step 2: Check status
aegis-driver-manager status nvidia-gpu-driver

# Step 3: Automatic installation and loading
# (handled by package manager)

# Step 4: Verify
aegis-driver-manager list --loaded

# Step 5: Monitor
aegis-activity-monitor --watch nvidia-gpu-driver
```

## Example: Manual Driver Installation (Advanced)

```c
#include <drivers/driver_manager.h>

int main() {
    // Initialize driver manager
    if (driver_manager_init() != 0) {
        return -1;
    }

    // Install GPU driver
    if (driver_install("nvidia_gpu", "/drivers/nvidia_gpu.aegis", DRIVER_CLASS_GPU) != 0) {
        return -2;
    }

    // Load the driver
    if (driver_load("nvidia_gpu") != 0) {
        return -3;
    }

    // Check status
    int status = driver_get_status("nvidia_gpu");
    if (status != DRIVER_STATUS_LOADED) {
        return -4;
    }

    // Disable if needed
    driver_disable("nvidia_gpu");

    // Cleanup
    driver_manager_shutdown();

    return 0;
}
```

## Status: COMPLETE
✅ Driver manager implemented and functional  
✅ All 5 driver modules created  
✅ Test suite for drivers implemented  
✅ Integration with system initialization complete
