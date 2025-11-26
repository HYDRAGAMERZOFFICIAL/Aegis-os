pub mod devfs;
pub mod sysfs;
pub mod node;
pub mod attribute;

pub use devfs::DevFS;
pub use sysfs::SysFS;
pub use node::{DeviceNode, NodeType};
pub use attribute::{Attribute, AttributeValue};

const MAX_NODES: usize = 1024;
const MAX_ATTRIBUTES: usize = 4096;
const PATH_MAX: usize = 256;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FileMode {
    UserRead = 0o400,
    UserWrite = 0o200,
    UserExec = 0o100,
    GroupRead = 0o040,
    GroupWrite = 0o020,
    GroupExec = 0o010,
    OtherRead = 0o004,
    OtherWrite = 0o002,
    OtherExec = 0o001,
}

#[derive(Debug, Clone, Copy)]
pub struct Permissions {
    pub mode: u16,
    pub uid: u32,
    pub gid: u32,
}

impl Permissions {
    pub fn new(mode: u16, uid: u32, gid: u32) -> Self {
        Permissions { mode, uid, gid }
    }

    pub fn default() -> Self {
        Permissions {
            mode: 0o644,
            uid: 0,
            gid: 0,
        }
    }

    pub fn readable(&self) -> bool {
        self.mode & 0o444 != 0
    }

    pub fn writable(&self) -> bool {
        self.mode & 0o222 != 0
    }

    pub fn executable(&self) -> bool {
        self.mode & 0o111 != 0
    }
}
