# Aegis OS Userspace Modules

This document describes all the high-level services, security modules, userland components, and filesystem services for Aegis OS.

## Overview

Aegis OS is structured as a layered architecture:
- **Kernel Layer** - Core OS functionality (already implemented)
- **System Services** - Background services managing system resources
- **Security Layer** - Cryptography, access control, firewall, intrusion detection
- **Filesystem Layer** - Advanced file system features
- **Userland** - User-facing applications and interfaces

## System Services Modules (9 Services)

### 1. Aegis Service Manager (ASM)
**Location:** `services/service_manager.c`

Manages background services lifecycle and dependencies.

**Key Features:**
- Service registration with dependency tracking
- Automatic service startup/restart
- Crash detection and logging
- Service state monitoring
- Health checks

**API:**
- `asm_register_service()` - Register new service with dependencies
- `asm_start_service()` - Start service by ID
- `asm_stop_service()` - Stop running service
- `asm_restart_service()` - Restart service
- `asm_log_crash()` - Log service crash event
- `asm_enable_monitoring()` - Enable automatic health monitoring

### 2. Session & Login Manager
**Location:** `services/session_manager.c`

Handles user authentication and session management.

**Authentication Methods:**
- Password-based
- Biometric (fingerprint, face recognition)
- Keycard/Hardware token
- PIN
- Multi-Factor Authentication (MFA)

**Key Features:**
- Session creation and destruction
- Token-based authentication
- Lock/unlock screen with multiple auth methods
- User profile management
- Session state tracking

**API:**
- `session_authenticate()` - Authenticate user
- `session_create()` - Create new session
- `session_lock_screen()` / `session_unlock_screen()`
- `session_register_user()` - Create new user
- `session_enable_biometric()` - Enable biometric auth
- `session_enable_keycard()` - Enable keycard auth

### 3. Update & Patch Manager
**Location:** `services/update_manager.c`

Manages OS and application updates.

**Update Types:**
- Security patches (critical)
- Feature updates
- Bug fixes
- Driver updates
- Kernel patches (KPATCH)

**Key Features:**
- Delta updates for minimal download size
- Cryptographic signature verification
- Kernel patching without reboot (KPATCH)
- Automatic rollback on failure
- Scheduled maintenance windows

**API:**
- `update_check_for_updates()` - Check for available updates
- `update_download()` - Download update package
- `update_verify_signature()` - Verify update authenticity
- `update_install()` - Install update
- `update_kpatch_apply()` - Apply live kernel patch
- `update_rollback()` - Rollback failed update
- `update_schedule_install()` - Schedule installation

### 4. Network Manager
**Location:** `services/network_manager.c`

Manages all network connectivity.

**Connection Types:**
- Ethernet (wired)
- Wi-Fi (wireless)
- Cellular/Mobile
- VPN
- Bluetooth

**Key Features:**
- Connection prioritization
- Wi-Fi scanning and auto-connect
- VPN profile management
- DNS-over-HTTPS (DoH) support
- Secure connection preference
- Network statistics tracking

**API:**
- `network_mgr_get_connections()` - List active connections
- `network_mgr_connect()` - Establish connection
- `network_mgr_scan_wifi()` - Scan for Wi-Fi networks
- `network_mgr_connect_wifi()` - Connect to Wi-Fi
- `network_mgr_enable_vpn()` - Enable VPN connection
- `network_mgr_enable_doh()` - Enable DNS-over-HTTPS

### 5. Storage Manager
**Location:** `services/storage_manager.c`

Manages disks, partitions, and volumes.

**Features:**
- Disk/volume enumeration
- Partition management
- Volume encryption
- Health monitoring
- Integrity verification
- Auto-repair capabilities
- Storage statistics

**API:**
- `storage_mgr_get_disks()` - List physical disks
- `storage_mgr_get_volumes()` - List volumes
- `storage_mgr_mount_volume()` - Mount volume
- `storage_mgr_enable_encryption()` - Enable volume encryption
- `storage_mgr_verify_integrity()` - Check filesystem integrity
- `storage_mgr_repair_volume()` - Auto-repair filesystem
- `storage_mgr_get_disk_health()` - Get SMART health status

### 6. Package Manager (AegisPKG)
**Location:** `services/package_manager.c`

Manages software installation and updates.

**Key Features:**
- Package repository management
- Dependency resolution
- Container-based app deployment
- Code scanning before installation
- Signature verification
- Auto-update capability

**API:**
- `pkgmgr_search_packages()` - Search package repository
- `pkgmgr_install_package()` - Install package
- `pkgmgr_verify_package()` - Verify integrity
- `pkgmgr_scan_code()` - Scan for malware
- `pkgmgr_containerize_app()` - Run app in container
- `pkgmgr_resolve_dependencies()` - Resolve package dependencies
- `pkgmgr_enable_auto_update()` - Enable auto-updates

### 7. Logging & Telemetry Service
**Location:** `services/logging_service.c`

Aggregates and manages system logs.

**Features:**
- Log aggregation from all components
- Real-time monitoring dashboard
- Log export and analysis
- Zero telemetry upload by default
- Opt-in analytics
- Log level filtering

**API:**
- `logging_log()` - Log message
- `logging_get_entries()` - Retrieve log entries
- `logging_export_logs()` - Export to file
- `logging_enable_real_time_monitoring()` - Enable live dashboard
- `logging_set_telemetry_enabled()` - Control external upload
- `logging_get_statistics()` - Get log statistics
- `logging_set_retention_policy()` - Configure log retention

### 8. Event Audit Service
**Location:** `services/audit_service.c`

Logs security-critical events for compliance.

**Event Types:**
- Login/logout
- File access/modification
- Permission changes
- Network access
- Process execution
- Security alerts

**Compliance Modes:**
- HIPAA (healthcare)
- GDPR (privacy)
- ISO27001 (information security)
- SOC2 (trust)
- Custom modes

**API:**
- `audit_log_event()` - Log audit event
- `audit_get_events()` - Retrieve events
- `audit_set_compliance_mode()` - Set compliance standard
- `audit_verify_event_integrity()` - Verify tamper-proof log
- `audit_enable_tamper_protection()` - Enable log protection
- `audit_generate_compliance_report()` - Generate report
- `audit_search_events()` - Search events by criteria

### 9. Secure Sandbox Manager
**Location:** `services/sandbox_manager.c`

Isolates applications in restricted environments.

**Sandbox Capabilities:**
- Permission-based access control
- Hardware resource limits (memory, CPU)
- Network isolation
- Filesystem restrictions
- Device access control

**Permissions:**
- Camera access
- Microphone access
- Location services
- Contacts/Calendar access
- Photo library
- Network connectivity
- USB device access
- Clipboard access

**API:**
- `sandbox_mgr_create_sandbox()` - Create isolated environment
- `sandbox_mgr_grant_permission()` - Grant permission
- `sandbox_mgr_revoke_permission()` - Revoke permission
- `sandbox_mgr_set_resource_limit()` - Set resource quotas
- `sandbox_mgr_allow_path()` - Allow filesystem access
- `sandbox_mgr_isolate_network()` - Isolate network
- `sandbox_mgr_apply_default_policy()` - Apply standard policy

## Security & Cryptography Modules (6 Modules)

### 1. Cryptography Engine
**Location:** `security/crypto_engine.c`

Provides cryptographic operations with hardware acceleration.

**Ciphers:**
- AES-XTS (disk encryption)
- ChaCha20 (stream cipher)
- AES-GCM (authenticated encryption)

**Hash Algorithms:**
- SHA-256
- SHA-512
- BLAKE2b

**Asymmetric Algorithms:**
- RSA (2048-bit, 4096-bit)
- ECC (P-256, P-384)
- Post-Quantum: Kyber-768, Dilithium-3

**Features:**
- Hardware acceleration detection (AES-NI, AVX)
- Secure random number generation
- Key derivation functions
- Digital signatures

**API:**
- `crypto_create_cipher()` - Create cipher context
- `crypto_encrypt()` / `crypto_decrypt()` - Cipher operations
- `crypto_hash()` - Hash data
- `crypto_generate_rsa_keypair()` - Generate RSA keys
- `crypto_rsa_sign()` / `crypto_rsa_verify()` - Digital signatures
- `crypto_generate_random()` - CSPRNG
- `crypto_enable_hw_acceleration()` - Enable HW offload

### 2. Zero-Trust Access Controller (ZTAC)
**Location:** `security/zero_trust_ac.c`

Enforces zero-trust security model: "Everything denied unless explicitly allowed."

**Resource Types:**
- Files/directories
- Devices
- Network
- Processes
- IPC objects
- APIs

**Rule Actions:**
- Allow
- Deny
- Challenge (require MFA)
- Log-only

**Features:**
- Time-based access (temporary grants)
- MFA requirement enforcement
- Detailed audit logging
- Default-deny policy

**API:**
- `ztac_create_rule()` - Create access rule
- `ztac_check_access()` - Check if access allowed
- `ztac_challenge_access()` - Require MFA challenge
- `ztac_grant_temporary_access()` - Time-limited access
- `ztac_revoke_access()` - Revoke access
- `ztac_set_default_policy()` - Set default action
- `ztac_audit_all_denials()` - Audit denied attempts

### 3. Aegis Shield (Firewall + IDS + IPS)
**Location:** `security/aegis_shield.c`

Multi-layer network security.

**Firewall:**
- Stateful packet inspection
- Protocol-aware filtering
- Rate limiting
- Port-based rules

**IDS (Intrusion Detection):**
- Anomaly detection
- Signature-based detection
- DDoS pattern recognition
- Threat classification

**IPS (Intrusion Prevention):**
- Automatic threat quarantine
- IP blocking
- Rate throttling
- Connection termination

**Features:**
- Real-time threat analysis
- Threat intelligence integration
- Whitelist/blacklist management
- Attack pattern learning

**API:**
- `shield_add_firewall_rule()` - Add firewall rule
- `shield_check_packet()` - Inspect packet
- `shield_detect_intrusion()` - Detect attack
- `shield_detect_ddos()` - Detect DDoS
- `shield_quarantine_threat()` - Isolate threat
- `shield_block_ip()` - Block IP address
- `shield_whitelist_ip()` - Whitelist IP

### 4. Secure Boot & Attestation Module
**Location:** `security/secure_boot.c`

Ensures system integrity from boot.

**Features:**
- Kernel signature verification
- Driver code verification
- TPM 2.0 integration
- Measured boot (PCR tracking)
- System attestation
- Certificate revocation

**Boot Modes:**
- Secure (production)
- Debug (development)
- Recovery

**API:**
- `secure_boot_verify_kernel()` - Verify kernel image
- `secure_boot_verify_driver()` - Verify driver
- `secure_boot_enable_lockdown()` - Enable lockdown
- `secure_boot_load_tpm_pcrs()` - Load PCR values
- `secure_boot_attest_system()` - Perform attestation
- `secure_boot_set_boot_mode()` - Set boot mode
- `secure_boot_revoke_certificate()` - Revoke cert

### 5. Application Permission Manager
**Location:** `security/app_permissions.c`

Fine-grained app permission control (Android-style but stricter).

**Permissions:**
- Camera
- Microphone
- Location
- Contacts/Calendar
- Photos
- Storage
- Clipboard
- Bluetooth
- NFC
- USB
- Printer
- Network
- Sensors

**Features:**
- Per-app permission grants
- Permission request prompts
- Revocation capability
- Access logging
- Default policies

**API:**
- `appm_request_permission()` - Request permission
- `appm_grant_permission()` - Grant permission
- `appm_deny_permission()` - Deny permission
- `appm_check_permission()` - Check if granted
- `appm_get_access_logs()` - View access history
- `appm_set_default_policy()` - Set default behavior

### 6. System Call Gatekeeper
**Location:** `security/syscall_gatekeeper.c`

Prevents kernel-level attacks.

**Attack Detection:**
- ROP (Return-Oriented Programming) chains
- Stack pivots
- JOP (Jump-Oriented Programming) gadgets
- Heap sprays
- Privilege escalation attempts

**Protections:**
- Control Flow Integrity (CFI)
- ROP chain detection
- Suspicious syscall filtering
- Frequency-based anomaly detection

**API:**
- `gatekeeper_check_syscall()` - Validate syscall
- `gatekeeper_detect_rop_chain()` - Detect ROP
- `gatekeeper_detect_stack_pivot()` - Detect pivot
- `gatekeeper_detect_heap_spray()` - Detect spray
- `gatekeeper_prevent_privilege_escalation()` - Block privesc
- `gatekeeper_enable_rop_protection()` - Enable ROP defense
- `gatekeeper_enable_cfi()` - Enable CFI

## Userland Modules (5 Components)

### 1. Desktop Environment (AegisUI)
**Location:** `userland/aegis_ui.c`

User-facing graphical interface.

**Features:**
- Adaptive layout (Windows-like + Linux-like hybrid)
- Light/Dark theme support
- Window manager
- Taskbar with favorites
- System tray
- Notifications
- Wallpaper customization

**API:**
- `aegis_ui_set_layout()` - Set UI layout
- `aegis_ui_set_theme()` - Set theme
- `aegis_ui_create_window()` - Create window
- `aegis_ui_render_taskbar()` - Render taskbar
- `aegis_ui_show_notification()` - Show notification
- `aegis_ui_set_wallpaper()` - Set background
- `aegis_ui_render_frame()` - Render screen

### 2. Aegis App Runtime
**Location:** `userland/app_runtime.c`

Portable application runtime supporting multiple formats.

**App Types:**
- Native (ELF binaries)
- WebAssembly (WASM)
- Container-based
- Virtual machines

**Features:**
- App isolation
- Resource limitations
- Environment variable management
- Runtime monitoring

**API:**
- `app_runtime_start_app()` - Start application
- `app_runtime_stop_app()` - Stop application
- `app_runtime_load_wasm_module()` - Load WASM
- `app_runtime_create_container()` - Create container
- `app_runtime_create_vm()` - Create VM
- `app_runtime_set_environment()` - Set env vars
- `app_runtime_get_resource_usage()` - Monitor resources

### 3. Settings & Control Center
**Location:** `userland/settings_center.c`

Unified system configuration interface.

**Settings Categories:**
- Display (brightness, resolution, refresh rate)
- Sound (volume, output device)
- Network (connections, VPN, DNS)
- Security (permissions, firewall)
- Storage (disks, encryption)
- System (about, updates)
- Privacy (data sharing, telemetry)

**API:**
- `settings_get()` / `settings_set()` - Get/set settings
- `settings_get_category()` - Get category settings
- `settings_apply_display_settings()` - Apply display config
- `settings_apply_audio_settings()` - Apply audio config
- `settings_enable_developer_mode()` - Enable dev mode
- `settings_export_settings()` / `settings_import_settings()`

### 4. Terminal & Shell (AegisShell)
**Location:** `userland/aegis_shell.c`

Hybrid command-line shell.

**Features:**
- Bash/PowerShell syntax hybrid
- Command auto-completion
- AI command suggestions
- Error correction
- Command history
- Alias support
- Script execution
- Syntax highlighting

**API:**
- `shell_execute_command()` - Execute command
- `shell_auto_complete()` - Auto-complete
- `shell_suggest_command()` - Get suggestions
- `shell_create_alias()` - Create alias
- `shell_execute_script()` - Run script
- `shell_get_command_history()` - View history

### 5. Aegis App Store
**Location:** `userland/app_store.c`

Centralized application distribution.

**Features:**
- App search and discovery
- Code scanning before installation
- Signature verification
- User reviews and ratings
- Automatic updates
- Security assessment
- Permission review

**API:**
- `app_store_search()` - Search apps
- `app_store_download_app()` - Download
- `app_store_install_app()` - Install
- `app_store_scan_app()` - Security scan
- `app_store_verify_app_signature()` - Verify signature
- `app_store_check_permissions()` - Review perms

## Storage & Filesystem Modules (6 Modules)

### 1. Metadata Engine
**Location:** `filesystem/metadata_engine.c`

Manages file metadata and integrity.

**Features:**
- File ownership and permissions
- Timestamps (created, modified, accessed)
- Merkle tree-based integrity verification
- Directory listing
- File hash tracking

**API:**
- `metadata_get_file_info()` - Get metadata
- `metadata_set_permissions()` - Set permissions
- `metadata_compute_merkle_tree()` - Verify integrity
- `metadata_list_directory()` - List files
- `metadata_get_file_hash()` - Get file hash

### 2. Journaling Module
**Location:** `filesystem/journaling.c`

Ensures crash-safe atomic transactions.

**Operations:**
- Write operations
- File deletion
- Directory operations
- Permission changes
- Ownership changes

**Features:**
- Transaction support
- Crash recovery
- Journal compaction
- Checksum verification

**API:**
- `journaling_start_transaction()` - Begin transaction
- `journaling_commit_transaction()` - Commit
- `journaling_rollback_transaction()` - Rollback
- `journaling_recover_after_crash()` - Recover
- `journaling_verify_journal_integrity()` - Verify

### 3. File Encryption Layer
**Location:** `filesystem/file_encryption.c`

Per-file encryption with unique keys.

**Algorithms:**
- AES-XTS (recommended for storage)
- ChaCha20

**Features:**
- Per-file encryption
- Key derivation from passwords
- Hardware acceleration
- Key rotation
- Bulk encryption

**API:**
- `file_encryption_enable()` - Enable encryption
- `file_encryption_encrypt_file()` - Encrypt
- `file_encryption_decrypt_file()` - Decrypt
- `file_encryption_rotate_keys()` - Rotate keys
- `file_encryption_derive_key()` - Derive from password

### 4. Access Control Layer
**Location:** `filesystem/access_control.c`

ACL-based fine-grained access control.

**Principals:**
- Users
- Groups
- Roles

**Permissions:**
- Read
- Write
- Execute
- Delete
- Admin

**Features:**
- Role-based access
- Inheritance
- Time-based expiry
- Mandatory access control

**API:**
- `acl_set_permissions()` - Set ACL
- `acl_check_permission()` - Check access
- `acl_create_role()` - Create role
- `acl_inherit_permissions()` - Inherit ACLs

### 5. File Indexing & Search Module
**Location:** `filesystem/file_indexing.c`

Fast global file search with tagging.

**Search Capabilities:**
- Name pattern matching
- Content search
- Tag-based search
- Advanced filters (size, date, etc.)
- Metadata search

**Features:**
- Real-time indexing
- Custom tagging
- Full-text search
- Content-based search

**API:**
- `indexing_search_by_name()` - Name search
- `indexing_search_by_tags()` - Tag search
- `indexing_search_advanced()` - Advanced search
- `indexing_add_tag()` - Tag file
- `indexing_search_by_content()` - Content search

### 6. Snapshot & Versioning Module
**Location:** `filesystem/snapshot_versioning.c`

System restore points and file versioning.

**Snapshots:**
- System snapshots (full system state)
- User snapshots (user directories)
- Backup snapshots

**Versioning:**
- File version history
- Automatic versioning
- Version comparison
- Version restore

**Features:**
- Incremental snapshots
- Periodic scheduling
- Retention policies
- Compression
- Integrity verification

**API:**
- `snapshot_create()` - Create snapshot
- `snapshot_restore()` - Restore from snapshot
- `snapshot_schedule_periodic()` - Auto-snapshots
- `versioning_save_version()` - Save file version
- `versioning_restore_version()` - Restore version
- `versioning_list_versions()` - Version history

## Build Instructions

### Build All Modules

```bash
mkdir build
cd build
cmake -DARCH=x86_64 ..
make
```

### Build Specific Module

```bash
make aegis_services
make aegis_security
make aegis_userland
make aegis_filesystem
```

## Integration with Kernel

All userspace modules depend on the kernel layer:

```
Userland (aegis_userland)
    ↓
Filesystem (aegis_filesystem)
    ↓
Security (aegis_security)
    ↓
Services (aegis_services)
    ↓
Kernel (aegis_kernel) + Common (aegis_common)
    ↓
Architecture (arch)
```

## Security Architecture

1. **Kernel Layer** - Low-level process/memory/interrupt management
2. **Security Layer** - Cryptography, ZTAC, firewall, syscall gatekeeper
3. **System Services** - Session management, updates, audit logging
4. **Filesystem** - Encryption, access control, journaling
5. **Userland** - GUI, app runtime, shell
6. **Applications** - Run in sandboxes with limited permissions

## Compliance & Standards

- **HIPAA** - Healthcare data protection
- **GDPR** - Privacy regulation
- **ISO27001** - Information security
- **SOC2** - Trust and security
- **CIS** - Center for Internet Security benchmarks

## Future Enhancements

1. Machine learning-based anomaly detection
2. Advanced container orchestration
3. GPU acceleration for crypto operations
4. Distributed audit logging
5. Advanced backup strategies
6. Blockchain-based tamper proofs
7. Quantum-resistant crypto migration
8. Advanced VM management
