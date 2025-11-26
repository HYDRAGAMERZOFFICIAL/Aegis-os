pub mod descriptor;
pub mod registry;
pub mod callback;
pub mod device;

pub use descriptor::{DriverDescriptor, DriverCallbackType};
pub use registry::DriverRegistry;
pub use callback::DriverCallback;
pub use device::Device;

use crate::error::{HalError, Result};

const MAX_DRIVERS: usize = 256;
const MAX_DEVICES: usize = 1024;

pub struct DriverManager {
    registry: DriverRegistry,
    devices: [Option<Device>; MAX_DEVICES],
    device_count: usize,
}

impl DriverManager {
    pub fn new() -> Self {
        DriverManager {
            registry: DriverRegistry::new(),
            devices: unsafe {
                [const { None }; MAX_DEVICES]
            },
            device_count: 0,
        }
    }

    pub fn register_driver(&mut self, descriptor: &DriverDescriptor) -> Result<()> {
        self.registry.register(descriptor)
    }

    pub fn probe_device(&mut self, device: &Device) -> Result<()> {
        if self.device_count >= MAX_DEVICES {
            return Err(HalError::NoMemory);
        }

        self.devices[self.device_count] = Some(*device);
        self.device_count += 1;
        Ok(())
    }

    pub fn get_device(&self, device_id: u32) -> Option<&Device> {
        self.devices
            .iter()
            .find_map(|d| d.as_ref().filter(|dev| dev.id == device_id))
    }
}
