pub struct AcpiSubsystem {
    initialized: bool,
}

impl AcpiSubsystem {
    pub fn new() -> Self {
        AcpiSubsystem {
            initialized: false,
        }
    }

    pub fn init(&mut self) {
        self.initialized = true;
    }
}
