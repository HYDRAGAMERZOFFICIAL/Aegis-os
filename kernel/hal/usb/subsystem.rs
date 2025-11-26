pub struct UsbSubsystem {
    initialized: bool,
}

impl UsbSubsystem {
    pub fn new() -> Self {
        UsbSubsystem {
            initialized: false,
        }
    }

    pub fn init(&mut self) {
        self.initialized = true;
    }
}
