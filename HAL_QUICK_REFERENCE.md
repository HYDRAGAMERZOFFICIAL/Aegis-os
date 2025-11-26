# HAL Quick Reference Guide

## Module Header Files

| Module | Header File | Purpose |
|--------|------------|---------|
| Main | `hal.h` | System initialization, IOMMU control |
| CPU | `hal_cpu.h` | CPU topology, MSR access, IPI, frequency control |
| Firmware | `hal_firmware.h` | UEFI/ACPI/TPM, Secure Boot, microcode |
| GPU | `hal_gpu.h` | GPU device management, CUDA/HIP equivalent |
| Audio | `hal_audio.h` | Audio device and stream management |
| Storage | `hal_storage.h` | SATA/NVMe/eMMC/USB storage devices |
| USB | `hal_usb.h` | USB device control and HID protocol |
| Power | `hal_power.h` | Power states, frequency scaling, thermal management |

## Initialization Sequence

```c
// Initialize HAL
hal_status_t status = hal_init();
if (status != HAL_OK) {
    printf("HAL initialization failed: %s\n", hal_get_error_str(status));
    return;
}

// Get system info
hal_system_info_t info;
hal_get_system_info(&info);
printf("Platform: %d, CPUs: %d\n", info.platform, info.cpu_count);

// Use HAL modules...

// Cleanup
hal_fini();
```

## Common Tasks

### CPU Operations

```c
// Get CPU topology
hal_cpu_topology_t topo;
hal_cpu_enumerate(&topo);
printf("CPUs: %d (cores: %d, threads: %d)\n", 
       topo.logical_cpus, topo.cores_per_package, topo.threads_per_core);

// Check CPU feature
if (hal_cpu_has_feature(0, HAL_CPU_FEATURE_AVX2) == HAL_OK) {
    printf("AVX2 supported\n");
}

// Read/Write MSR
uint64_t msr_value;
hal_cpu_read_msr(0, 0x198, &msr_value);  // IA32_PERF_CTL

// Send IPI to CPU 1
hal_cpu_send_ipi(1 << 1, 32);  // Send vector 32 to CPU 1

// Frequency scaling
hal_cpu_set_frequency(0, 3400);  // Set CPU 0 to 3.4 GHz
hal_cpu_enable_turbo(0);
```

### Firmware Operations

```c
// Get firmware info
hal_firmware_info_t fw_info;
hal_firmware_get_info(&fw_info);
if (fw_info.type == HAL_FIRMWARE_UEFI_64) {
    printf("UEFI 64-bit detected\n");
}

// Check Secure Boot
bool sb_enabled;
hal_firmware_is_secure_boot_enabled(&sb_enabled);

// Get ACPI table
hal_acpi_table_t table;
if (hal_acpi_get_table((uint8_t *)"DSDT", 0, &table) == HAL_OK) {
    printf("Found DSDT, size: %d\n", table.table_size);
}

// TPM operations
hal_firmware_extend_pcr(0, data, data_len);
uint32_t pcr_size = 32;
hal_firmware_read_pcr(0, pcr_value, &pcr_size);
```

### GPU Operations

```c
// Enumerate GPUs
uint32_t gpu_count;
hal_gpu_enumerate(&gpu_count);
printf("Found %d GPUs\n", gpu_count);

// Get GPU info
hal_gpu_info_t gpu_info;
hal_gpu_get_info(0, &gpu_info);
printf("GPU: %s, VRAM: %lld MB\n", gpu_info.model, gpu_info.vram_size / (1024*1024));

// Create GPU context
uint64_t context;
hal_gpu_create_context(0, getpid(), &context);

// Allocate GPU memory
uint64_t gpu_addr;
hal_gpu_allocate_memory(context, 1024*1024, &gpu_addr);

// Copy to device
hal_gpu_copy_to_device(context, host_buffer, gpu_addr, buffer_size);

// Submit kernel
hal_gpu_submit_kernel(context, kernel_code, code_size, 
                      16, 16, 1,  // block dimensions
                      128, 128, 1); // grid dimensions

// Wait for completion
hal_gpu_wait_for_completion(context, 5000);  // 5 second timeout
```

### Audio Operations

```c
// Enumerate audio devices
uint32_t dev_count;
hal_audio_enumerate_devices(&dev_count);

// Get device info
hal_audio_device_t dev_info;
hal_audio_get_device_info(0, &dev_info);
printf("Device: %s, Volume: %d dB\n", dev_info.device_name, dev_info.volume_db);

// Create audio stream
hal_audio_config_t config = {
    .format = HAL_AUDIO_FORMAT_PCM_S16,
    .sample_rate = 48000,
    .channels = HAL_AUDIO_CHANNELS_STEREO
};
uint64_t stream_id;
hal_audio_create_stream(0, &config, &stream_id);

// Playback
hal_audio_play(stream_id, audio_buffer, buffer_size, 1000);

// Volume control
hal_audio_set_volume(0, -3);  // -3 dB
```

### Storage Operations

```c
// Enumerate storage devices
uint32_t dev_count;
hal_storage_enumerate_devices(&dev_count);

// Get device info
hal_storage_device_t storage;
hal_storage_get_device_info(0, &storage);
printf("Device: %s, Capacity: %lld GB\n", 
       storage.model, storage.capacity_bytes / (1024*1024*1024));

// Synchronous read
uint8_t buffer[4096];
hal_storage_read_sync(0, 0, 8, buffer, 5000);  // Read 8 sectors from LBA 0

// Asynchronous write
uint64_t req_id;
hal_storage_write_async(0, 100, 8, buffer, 
                        my_io_callback, context, &req_id);

// Get SMART info
hal_storage_smart_t smart;
hal_storage_get_smart_info(0, &smart);
printf("Health: %d%%, Temp: %d C\n", smart.is_healthy, smart.temperature);

// TRIM operation
hal_storage_trim(0, 1000, 100);
```

### USB Operations

```c
// Enumerate USB devices
uint32_t dev_count;
hal_usb_enumerate_devices(&dev_count);

// Get device info
hal_usb_device_t usb_dev;
hal_usb_get_device_info(0, 1, &usb_dev);
printf("Device: %s, Speed: %d\n", usb_dev.product, usb_dev.speed);

// Bulk transfer (synchronous)
uint32_t transferred;
hal_usb_bulk_transfer(0, 1, 0x81, buffer, 512, &transferred, 5000);

// Async interrupt transfer
uint64_t transfer_id;
hal_usb_interrupt_transfer_async(0, 1, 0x82, buffer, 64, 
                                  my_usb_callback, context, &transfer_id);

// HID report
uint8_t report[64];
hal_hid_get_report(0, 1, 0, report, 64);
```

### Power Management

```c
// Get current power state
hal_power_state_t state;
hal_power_get_system_state(&state);

// Sleep
hal_power_sleep(HAL_POWER_STATE_S3);  // Suspend to RAM

// Get P-state table
hal_power_pstate_table_t pstates;
hal_power_get_pstate_table(0, &pstates);
for (int i = 0; i < pstates.pstate_count; i++) {
    printf("P%d: %d MHz @ %d mV\n", i, 
           pstates.pstates[i].frequency_mhz, 
           pstates.pstates[i].voltage_mv);
}

// Set P-state
hal_power_set_pstate(0, 5);  // Set CPU 0 to P5

// Battery info
hal_battery_info_t battery;
hal_battery_get_info(&battery);
printf("Battery: %d%%, %d mWh remaining\n", 
       battery.battery_percent, battery.remaining_mwh);

// Set charging threshold
hal_battery_set_charge_threshold(20, 80);

// Thermal control
hal_thermal_zone_t zone;
hal_thermal_get_zone_info(0, &zone);
printf("Temp: %d C, Trip: %d C\n", 
       zone.current_temperature_celsius, zone.trip_point_celsius);

// Fan control
hal_fan_info_t fan;
hal_fan_get_info(0, &fan);
printf("Fan 0: %d RPM\n", fan.current_rpm);
hal_fan_set_speed(0, 75);  // Set to 75%
```

## Error Handling

```c
hal_status_t status = hal_cpu_read_msr(0, 0x198, &value);
if (status != HAL_OK) {
    const char *error = hal_get_error_str(status);
    fprintf(stderr, "Error: %s\n", error);
    
    switch (status) {
        case HAL_ERR_INVALID_ARG:
            printf("Invalid argument\n");
            break;
        case HAL_ERR_NOT_SUPPORTED:
            printf("Operation not supported on this hardware\n");
            break;
        case HAL_ERR_TIMEOUT:
            printf("Operation timed out\n");
            break;
        default:
            printf("Unknown error: %d\n", status);
    }
}
```

## Platform Detection

```c
hal_system_info_t info;
hal_get_system_info(&info);

switch (info.platform) {
    case HAL_PLATFORM_X86_64:
        printf("x86_64\n");
        break;
    case HAL_PLATFORM_ARM64:
        printf("ARM64\n");
        break;
    case HAL_PLATFORM_ARM32:
        printf("ARM32\n");
        break;
    default:
        printf("Unknown platform\n");
}
```

## Maximum Limits

| Item | Limit |
|------|-------|
| CPU Count | 1024 |
| GPU Devices | 8 |
| Audio Devices | 16 |
| Storage Devices | 16 |
| USB Buses | 8 |
| GPU Contexts | 256 |
| Audio Streams | 64 |
| Storage I/O Requests | 256 |
| USB Transfers | 512 |

## Important Notes

1. **Initialization**: Always call `hal_init()` before using any HAL functions
2. **Cleanup**: Call `hal_fini()` before shutdown to release resources
3. **Thread Safety**: Most HAL functions are NOT thread-safe; synchronization is caller's responsibility
4. **Callbacks**: Callbacks may be invoked from interrupt context; keep them short
5. **Memory**: Don't assume specific memory layout for HAL structures; always use provided APIs
6. **Error Codes**: Always check return values and handle errors appropriately
