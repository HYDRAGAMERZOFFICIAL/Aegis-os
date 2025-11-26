use super::token::CapabilityToken;
use super::permissions::Permission;
use crate::error::{HalError, Result};

const MAX_TOKENS: usize = 4096;
const DEFAULT_LIFETIME: u64 = 3600;

pub struct CapabilityManager {
    tokens: [Option<CapabilityToken>; MAX_TOKENS],
    token_count: u32,
    next_token_id: u32,
}

impl CapabilityManager {
    pub fn new() -> Self {
        CapabilityManager {
            tokens: unsafe { [const { None }; MAX_TOKENS] },
            token_count: 0,
            next_token_id: 1,
        }
    }

    pub fn create_token(
        &mut self,
        container_id: u32,
        app_id: u32,
        permissions: u32,
    ) -> Result<CapabilityToken> {
        if self.token_count as usize >= MAX_TOKENS {
            return Err(HalError::NoMemory);
        }

        let token_id = self.next_token_id;
        self.next_token_id += 1;

        let token = CapabilityToken::new(token_id, container_id, app_id, permissions, DEFAULT_LIFETIME);

        let mut found_slot = false;
        for slot in self.tokens.iter_mut() {
            if slot.is_none() {
                *slot = Some(token);
                found_slot = true;
                break;
            }
        }

        if !found_slot {
            return Err(HalError::NoMemory);
        }

        self.token_count += 1;
        Ok(token)
    }

    pub fn verify(&self, token: &CapabilityToken) -> Result<()> {
        if !token.is_valid() {
            return Err(HalError::PermissionDenied);
        }

        if !self
            .tokens
            .iter()
            .any(|t| t.as_ref().map_or(false, |tt| tt.token_id == token.token_id))
        {
            return Err(HalError::InvalidArg);
        }

        Ok(())
    }

    pub fn revoke(&mut self, token_id: u32) -> Result<()> {
        if let Some(token) = self.tokens.iter_mut().find_map(|t| {
            t.as_mut()
                .filter(|tt| tt.token_id == token_id)
        }) {
            token.flags.insert(super::token::TokenFlags::REVOKED);
            Ok(())
        } else {
            Err(HalError::InvalidArg)
        }
    }

    pub fn check_permission(&self, token: &CapabilityToken, permission: Permission) -> bool {
        if !token.is_valid() {
            return false;
        }

        (token.permissions & permission.bits()) != 0
    }
}
