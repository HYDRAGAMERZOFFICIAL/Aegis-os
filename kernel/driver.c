#include <kernel/driver.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * AegisOS Driver Subsystem & Hardware Device Enumerator
 * Supports modular driver registration, WASM/VM sandboxing,
 * and PCI configuration space bus scanning.
 */

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#if defined(__x86_64__) && !defined(_WIN32)
static inline void pci_outl(u16 port, u32 val) {
    asm volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}
static inline u32 pci_inl(u16 port) {
    u32 ret;
    asm volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
#endif

typedef struct {
    driver_t *drivers[256];
    u32 driver_count;
    pci_device_t pci_devices[64];
    u32 pci_device_count;
    uint lock;
} driver_mgr_state_t;

static driver_mgr_state_t driver_state = {0};
static u64 next_driver_id = 1;

/* Simulated Hardware PCI Devices for Host Simulation Mode */
static const pci_device_t simulated_pci_table[] = {
    { .vendor_id = 0x8086, .device_id = 0x1237, .class_code = 0x0600 }, // Intel 440FX Host Bridge
    { .vendor_id = 0x8086, .device_id = 0x7000, .class_code = 0x0601 }, // Intel PIIX3 ISA Bridge
    { .vendor_id = 0x8086, .device_id = 0x2922, .class_code = 0x0106 }, // Intel ICH9 AHCI SATA Controller
    { .vendor_id = 0x1B36, .device_id = 0x0010, .class_code = 0x0108 }, // QEMU NVMe PCI Storage
    { .vendor_id = 0x8086, .device_id = 0x100E, .class_code = 0x0200 }, // Intel 82540EM Gigabit Ethernet (e1000)
    { .vendor_id = 0x1AF4, .device_id = 0x1000, .class_code = 0x0200 }, // VirtIO Network Adapter
    { .vendor_id = 0x1234, .device_id = 0x1111, .class_code = 0x0300 }, // Bochs / QEMU Standard VGA Display
    { .vendor_id = 0x1AF4, .device_id = 0x1005, .class_code = 0xFF00 }  // VirtIO Hardware CSPRNG
};

// Generic driver lifecycle operations
static int default_probe(void *dev) { (void)dev; return 0; }
static int default_remove(void *dev) { (void)dev; return 0; }
static int default_suspend(void *dev) { (void)dev; return 0; }
static int default_resume(void *dev) { (void)dev; return 0; }

static driver_ops_t default_ops = {
    .init = NULL,
    .destroy = NULL,
    .open = NULL,
    .close = NULL,
    .read = NULL,
    .write = NULL,
    .ioctl = NULL
};

/* PCI Config Space Read */
u32 pci_read_config32(u8 bus, u8 slot, u8 func, u8 offset)
{
#if defined(__x86_64__) && !defined(_WIN32)
    u32 address = (u32)((1UL << 31) | ((u32)bus << 16) | ((u32)slot << 11) | ((u32)func << 8) | (offset & 0xFC));
    pci_outl(PCI_CONFIG_ADDR, address);
    return pci_inl(PCI_CONFIG_DATA);
#else
    // Host simulation lookup
    for (size_t i = 0; i < sizeof(simulated_pci_table)/sizeof(simulated_pci_table[0]); i++) {
        if (slot == (u8)i && bus == 0 && func == 0) {
            if (offset == 0) {
                return (simulated_pci_table[i].device_id << 16) | simulated_pci_table[i].vendor_id;
            } else if (offset == 0x08) {
                return (simulated_pci_table[i].class_code << 16);
            }
        }
    }
    return 0xFFFFFFFF;
#endif
}

/* Enumerate PCI devices across all buses and slots */
void pci_scan_bus(void)
{
    driver_state.pci_device_count = 0;

    for (u16 bus = 0; bus < 4; bus++) {
        for (u8 slot = 0; slot < 32; slot++) {
            u32 reg0 = pci_read_config32((u8)bus, slot, 0, 0);
            u16 vendor = (u16)(reg0 & 0xFFFF);
            u16 device = (u16)(reg0 >> 16);

            if (vendor == 0xFFFF || vendor == 0x0000) {
                continue; // Device not present
            }

            u32 reg2 = pci_read_config32((u8)bus, slot, 0, 0x08);
            u32 class_code = (reg2 >> 16) & 0xFFFF;

            if (driver_state.pci_device_count < 64) {
                pci_device_t *pdev = &driver_state.pci_devices[driver_state.pci_device_count++];
                pdev->vendor_id = vendor;
                pdev->device_id = device;
                pdev->class_code = class_code;
            }
        }
    }
}

int driver_mgr_init(void)
{
    memset(&driver_state, 0, sizeof(driver_state));
    next_driver_id = 1;

    // Discover hardware PCI topology
    pci_scan_bus();

    // Register built-in standard drivers
    driver_register("storage/ahci", DRIVER_TYPE_STORAGE, &default_ops);
    driver_register("storage/nvme", DRIVER_TYPE_STORAGE, &default_ops);
    driver_register("net/e1000", DRIVER_TYPE_NETWORK, &default_ops);
    driver_register("net/virtio", DRIVER_TYPE_NETWORK, &default_ops);
    driver_register("display/bga", DRIVER_TYPE_DISPLAY, &default_ops);
    driver_register("input/ps2", DRIVER_TYPE_INPUT, &default_ops);
    driver_register("crypto/rng", DRIVER_TYPE_CUSTOM, &default_ops);

    return 0;
}

driver_t *driver_register(const char *name, driver_type_t type, driver_ops_t *ops)
{
    if (!name) return NULL;

    driver_t *drv = (driver_t *)malloc(sizeof(driver_t));
    if (!drv) return NULL;

    drv->id = next_driver_id++;
    drv->name = name;
    drv->type = type;
    drv->state = DRIVER_STATE_LOADED;
    drv->module = (void *)ops;
    drv->sandbox = SANDBOX_MODE_WASM; // Default to WebAssembly safe sandbox
    drv->permission_mask = 0x07;      // Read/Write/Execute permission
    drv->probe = default_probe;
    drv->remove = default_remove;
    drv->suspend = default_suspend;
    drv->resume = default_resume;

    if (driver_state.driver_count < 256) {
        driver_state.drivers[driver_state.driver_count++] = drv;
        return drv;
    }

    free(drv);
    return NULL;
}

int driver_unregister(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            if (driver_state.drivers[i]->state == DRIVER_STATE_RUNNING) {
                driver_state.drivers[i]->state = DRIVER_STATE_UNLOADED;
            }

            free(driver_state.drivers[i]);
            for (u32 j = i; j < driver_state.driver_count - 1; j++) {
                driver_state.drivers[j] = driver_state.drivers[j + 1];
            }
            driver_state.driver_count--;
            return 0;
        }
    }

    return -1;
}

int driver_load(const char *driver_path)
{
    if (!driver_path) return -1;

    // Check if driver is already registered
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (strcmp(driver_state.drivers[i]->name, driver_path) == 0) {
            driver_state.drivers[i]->state = DRIVER_STATE_RUNNING;
            return 0;
        }
    }

    // Determine driver type from path prefix
    driver_type_t dtype = DRIVER_TYPE_CUSTOM;
    if (strstr(driver_path, "storage")) dtype = DRIVER_TYPE_STORAGE;
    else if (strstr(driver_path, "net")) dtype = DRIVER_TYPE_NETWORK;
    else if (strstr(driver_path, "display") || strstr(driver_path, "gpu")) dtype = DRIVER_TYPE_DISPLAY;
    else if (strstr(driver_path, "input")) dtype = DRIVER_TYPE_INPUT;
    else if (strstr(driver_path, "audio")) dtype = DRIVER_TYPE_AUDIO;

    // Dynamically register and activate driver
    driver_t *drv = driver_register(driver_path, dtype, &default_ops);
    if (!drv) return -1;

    drv->state = DRIVER_STATE_LOADING;
    if (drv->probe) {
        drv->probe(NULL);
    }
    drv->state = DRIVER_STATE_RUNNING;

    return 0;
}

int driver_unload(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_t *drv = driver_state.drivers[i];
            if (drv->state == DRIVER_STATE_RUNNING) {
                if (drv->remove) {
                    drv->remove(NULL);
                }
                drv->state = DRIVER_STATE_UNLOADED;
                return 0;
            }
            return -1;
        }
    }

    return -1;
}

int driver_set_sandbox_mode(u64 driver_id, sandbox_mode_t mode)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->sandbox = mode;
            return 0;
        }
    }

    return -1;
}

int driver_set_permissions(u64 driver_id, u32 permissions)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->permission_mask = permissions;
            return 0;
        }
    }

    return -1;
}

int driver_probe_devices(void)
{
    // Match hardware PCI devices with registered drivers
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        driver_t *drv = driver_state.drivers[i];
        for (u32 d = 0; d < driver_state.pci_device_count; d++) {
            pci_device_t *pdev = &driver_state.pci_devices[d];

            bool match = false;
            if (drv->type == DRIVER_TYPE_STORAGE && ((pdev->class_code >> 8) == 0x01)) match = true;
            if (drv->type == DRIVER_TYPE_NETWORK && ((pdev->class_code >> 8) == 0x02)) match = true;
            if (drv->type == DRIVER_TYPE_DISPLAY && ((pdev->class_code >> 8) == 0x03)) match = true;
            if (drv->type == DRIVER_TYPE_INPUT   && ((pdev->class_code >> 8) == 0x09)) match = true;

            if (match && drv->probe) {
                drv->probe(pdev);
                drv->state = DRIVER_STATE_RUNNING;
            }
        }
    }

    return 0;
}

int driver_enable_isolation(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->sandbox = SANDBOX_MODE_VM;
            return 0;
        }
    }

    return -1;
}

int driver_create_wasm_sandbox(u64 driver_id, const u8 *wasm_module, u64 module_size)
{
    if (!wasm_module || module_size == 0) return -1;

    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_state.drivers[i]->sandbox = SANDBOX_MODE_WASM;
            driver_state.drivers[i]->module = (void *)wasm_module;
            return 0;
        }
    }

    return -1;
}

driver_state_t driver_get_state(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            return driver_state.drivers[i]->state;
        }
    }

    return DRIVER_STATE_ERROR;
}

int driver_suspend(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_t *drv = driver_state.drivers[i];
            if (drv->suspend) {
                drv->suspend(NULL);
            }
            drv->state = DRIVER_STATE_SUSPENDED;
            return 0;
        }
    }

    return -1;
}

int driver_resume(u64 driver_id)
{
    for (u32 i = 0; i < driver_state.driver_count; i++) {
        if (driver_state.drivers[i]->id == driver_id) {
            driver_t *drv = driver_state.drivers[i];
            if (drv->resume) {
                drv->resume(NULL);
            }
            drv->state = DRIVER_STATE_RUNNING;
            return 0;
        }
    }

    return -1;
}
