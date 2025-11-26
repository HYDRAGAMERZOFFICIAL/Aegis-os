pub struct UefiSubsystem {
    initialized: bool,
}

impl UefiSubsystem {
    pub fn new() -> Self {
        UefiSubsystem {
            initialized: false,
        }
    }

    pub fn init(&mut self) {
        self.initialized = true;
    }
}
