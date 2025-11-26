pub struct BlockSubsystem {
    initialized: bool,
}

impl BlockSubsystem {
    pub fn new() -> Self {
        BlockSubsystem {
            initialized: false,
        }
    }

    pub fn init(&mut self) {
        self.initialized = true;
    }
}
