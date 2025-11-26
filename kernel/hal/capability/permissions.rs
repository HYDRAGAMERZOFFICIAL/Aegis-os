use bitflags::bitflags;

bitflags! {
    #[repr(transparent)]
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub struct Permission: u32 {
        const READ = 0x01;
        const WRITE = 0x02;
        const EXECUTE = 0x04;
        const ADMIN = 0x08;
        const CAPTURE = 0x10;
        const PLAYBACK = 0x20;
        const MIXER = 0x40;
        const REALTIME = 0x80;
        const PASSTHROUGH = 0x100;
    }
}

impl Permission {
    pub fn name(&self) -> &'static str {
        match *self {
            Self::READ => "READ",
            Self::WRITE => "WRITE",
            Self::EXECUTE => "EXECUTE",
            Self::ADMIN => "ADMIN",
            Self::CAPTURE => "CAPTURE",
            Self::PLAYBACK => "PLAYBACK",
            Self::MIXER => "MIXER",
            Self::REALTIME => "REALTIME",
            Self::PASSTHROUGH => "PASSTHROUGH",
            _ => "UNKNOWN",
        }
    }
}
