use core::fmt;

pub type HalStatus = i32;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum HalError {
    Ok = 0,
    InvalidArg = -1,
    NotSupported = -2,
    DeviceBusy = -3,
    NoMemory = -4,
    Timeout = -5,
    DeviceFailed = -6,
    PermissionDenied = -7,
}

impl From<HalError> for HalStatus {
    fn from(err: HalError) -> Self {
        err as i32
    }
}

impl From<HalStatus> for HalError {
    fn from(status: HalStatus) -> Self {
        match status {
            0 => HalError::Ok,
            -1 => HalError::InvalidArg,
            -2 => HalError::NotSupported,
            -3 => HalError::DeviceBusy,
            -4 => HalError::NoMemory,
            -5 => HalError::Timeout,
            -6 => HalError::DeviceFailed,
            -7 => HalError::PermissionDenied,
            _ => HalError::DeviceFailed,
        }
    }
}

impl fmt::Display for HalError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            HalError::Ok => write!(f, "Success"),
            HalError::InvalidArg => write!(f, "Invalid argument"),
            HalError::NotSupported => write!(f, "Not supported"),
            HalError::DeviceBusy => write!(f, "Device busy"),
            HalError::NoMemory => write!(f, "Out of memory"),
            HalError::Timeout => write!(f, "Timeout"),
            HalError::DeviceFailed => write!(f, "Device failed"),
            HalError::PermissionDenied => write!(f, "Permission denied"),
        }
    }
}

pub type Result<T> = core::result::Result<T, HalError>;
