use core::ffi::c_void;
use core::ptr;

use crate::error::{HalError, Result};
use super::topology::{CpuTopology, CpuInfo};
use super::features::{CpuFeature, Cpuid};
use super::pstate::PowerState;
use super::stats::CpuStats;

pub const MAX_CPUS: usize = 256;

pub struct CpuState {
    cpu_mask: u64,
    cpu_count: u8,
    topology: CpuTopology,
    per_cpu_data: [*mut c_void; MAX_CPUS],
    initialized: bool,
}

pub struct CpuSubsystem {
    state: CpuState,
}

impl CpuSubsystem {
    pub fn new() -> Self {
        CpuSubsystem {
            state: CpuState {
                cpu_mask: 0,
                cpu_count: 0,
                topology: CpuTopology::default(),
                per_cpu_data: [ptr::null_mut(); MAX_CPUS],
                initialized: false,
            },
        }
    }

    pub fn init(&mut self) -> Result<()> {
        if self.state.initialized {
            return Ok(());
        }

        self.state.cpu_count = self.state.topology.logical_cpus as u8;
        self.state.cpu_mask = !0u64 & (((1u64) << self.state.cpu_count) - 1);
        self.state.initialized = true;

        Ok(())
    }

    pub fn enumerate(&self) -> Result<CpuTopology> {
        if !self.state.initialized {
            return Err(HalError::DeviceFailed);
        }
        Ok(self.state.topology)
    }

    pub fn get_cpu_count(&self) -> u8 {
        self.state.cpu_count
    }

    pub fn has_feature(&self, cpu_id: u8, _feature: CpuFeature) -> bool {
        if cpu_id >= self.state.cpu_count {
            return false;
        }
        true
    }

    pub fn get_cache_line_size() -> u32 {
        64
    }
}
