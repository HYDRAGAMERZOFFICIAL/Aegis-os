use super::descriptor::DriverDescriptor;
use crate::error::{HalError, Result};

const MAX_DRIVERS: usize = 256;

pub struct DriverRegistry {
    drivers: [Option<DriverDescriptor>; MAX_DRIVERS],
    driver_count: usize,
}

impl DriverRegistry {
    pub fn new() -> Self {
        DriverRegistry {
            drivers: unsafe {
                [const { None }; MAX_DRIVERS]
            },
            driver_count: 0,
        }
    }

    pub fn register(&mut self, descriptor: &DriverDescriptor) -> Result<()> {
        descriptor.validate()?;

        if self.driver_count >= MAX_DRIVERS {
            return Err(HalError::NoMemory);
        }

        if self.drivers.iter().any(|d| {
            d.as_ref()
                .map_or(false, |dd| dd.driver_id == descriptor.driver_id)
        }) {
            return Err(HalError::DeviceBusy);
        }

        self.drivers[self.driver_count] = Some(*descriptor);
        self.driver_count += 1;

        Ok(())
    }

    pub fn unregister(&mut self, driver_id: u32) -> Result<()> {
        if let Some(pos) = self
            .drivers
            .iter()
            .position(|d| d.as_ref().map_or(false, |dd| dd.driver_id == driver_id))
        {
            self.drivers.remove(pos);
            self.driver_count -= 1;
            Ok(())
        } else {
            Err(HalError::InvalidArg)
        }
    }

    pub fn find(&self, driver_id: u32) -> Option<&DriverDescriptor> {
        self.drivers
            .iter()
            .find_map(|d| d.as_ref().filter(|dd| dd.driver_id == driver_id))
    }

    pub fn get_by_priority(&self, min_priority: u32) -> Vec<&DriverDescriptor> {
        self.drivers
            .iter()
            .filter_map(|d| d.as_ref().filter(|dd| dd.priority >= min_priority))
            .collect()
    }
}
