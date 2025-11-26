pub struct PowerSubsystem {
    initialized: bool,
}

impl PowerSubsystem {
    pub fn new() -> Self {
        PowerSubsystem {
            initialized: false,
        }
    }

    pub fn init(&mut self) {
        self.initialized = true;
    }
}
