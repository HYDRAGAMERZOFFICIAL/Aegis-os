pub struct AudioSubsystem {
    initialized: bool,
}

impl AudioSubsystem {
    pub fn new() -> Self {
        AudioSubsystem {
            initialized: false,
        }
    }

    pub fn init(&mut self) {
        self.initialized = true;
    }
}
