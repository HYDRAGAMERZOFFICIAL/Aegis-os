pub struct GpuSubsystem {
    initialized: bool,
}

impl GpuSubsystem {
    pub fn new() -> Self {
        GpuSubsystem {
            initialized: false,
        }
    }

    pub fn init(&mut self) {
        self.initialized = true;
    }
}
