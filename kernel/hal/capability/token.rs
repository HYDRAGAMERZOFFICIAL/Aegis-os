use bitflags::bitflags;

bitflags! {
    #[repr(transparent)]
    #[derive(Debug, Clone, Copy)]
    pub struct TokenFlags: u32 {
        const SIGNED = 0x01;
        const REVOKED = 0x02;
        const EXPIRED = 0x04;
        const RESTRICTED = 0x08;
    }
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct CapabilityToken {
    pub token_id: u32,
    pub container_id: u32,
    pub app_id: u32,
    pub permissions: u32,
    pub flags: TokenFlags,
    pub created_time: u64,
    pub expiry_time: u64,
    pub signature: [u8; 64],
}

impl CapabilityToken {
    pub fn new(
        token_id: u32,
        container_id: u32,
        app_id: u32,
        permissions: u32,
        lifetime_secs: u64,
    ) -> Self {
        let now = 0u64;
        
        CapabilityToken {
            token_id,
            container_id,
            app_id,
            permissions,
            flags: TokenFlags::empty(),
            created_time: now,
            expiry_time: now + lifetime_secs,
            signature: [0u8; 64],
        }
    }

    pub fn is_expired(&self) -> bool {
        self.flags.contains(TokenFlags::EXPIRED)
    }

    pub fn is_revoked(&self) -> bool {
        self.flags.contains(TokenFlags::REVOKED)
    }

    pub fn is_valid(&self) -> bool {
        !self.is_expired() && !self.is_revoked()
    }
}
