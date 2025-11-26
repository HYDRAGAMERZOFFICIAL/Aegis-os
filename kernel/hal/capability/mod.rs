pub mod token;
pub mod manager;
pub mod permissions;

pub use token::{CapabilityToken, TokenFlags};
pub use manager::CapabilityManager;
pub use permissions::Permission;

use crate::error::{HalError, Result};

const MAX_TOKENS: usize = 4096;
const TOKEN_EXPIRY_DEFAULT: u64 = 3600;

pub struct CapabilitySystem {
    manager: CapabilityManager,
}

impl CapabilitySystem {
    pub fn new() -> Self {
        CapabilitySystem {
            manager: CapabilityManager::new(),
        }
    }

    pub fn request_token(
        &mut self,
        container_id: u32,
        app_id: u32,
        permissions: u32,
    ) -> Result<CapabilityToken> {
        self.manager.create_token(container_id, app_id, permissions)
    }

    pub fn verify_token(&self, token: &CapabilityToken) -> Result<()> {
        self.manager.verify(token)
    }

    pub fn revoke_token(&mut self, token_id: u32) -> Result<()> {
        self.manager.revoke(token_id)
    }

    pub fn has_permission(&self, token: &CapabilityToken, permission: Permission) -> bool {
        self.manager.check_permission(token, permission)
    }
}
