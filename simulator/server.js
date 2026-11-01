/**
 * AegisOS Desktop Environment & System Simulator Server
 * Pure Node.js implementation - Zero external dependencies required.
 * 
 * Features:
 * - Dual-Kernel Microkernel & Real OS Syscall Simulation
 * - Real Cryptographic File I/O (Read, Write, Edit, Create, Delete, Shred, Encrypt)
 * - Tiered Storage & Block Deduplication Optimizer
 * - Virtual Air-Gapped Disposable Browser ("Aegis Quantum Ghost Browser")
 * - Comprehensive Cybersecurity Suite (DPI Sniffer, Vulnerability Scanner, Malware Sandbox, Zero-Trust Broker)
 * - Agentic AI Assistant ("Aegis Mind") with speech cancellation & multi-step execution
 */

const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawn, exec } = require('child_process');
const os = require('os');
const GameManager = require('../game/core/GameManager');
const SaveManager = require('../game/saves/SaveManager');
const AuthManager = require('../game/auth/AuthManager');
const isVercel = Boolean(process.env.VERCEL || process.env.AWS_LAMBDA_FUNCTION_NAME);
const aegisDataDir = isVercel ? path.join(os.tmpdir(), '.aegis-os') : path.join(os.homedir(), '.aegis-os');
const authManager = new AuthManager(aegisDataDir);
const gameSaves = new SaveManager(path.join(aegisDataDir, 'saves'));
const campaignDataDir = path.join(aegisDataDir, 'campaigns');
fs.mkdirSync(campaignDataDir, { recursive: true });
const campaignMemoryCache = new Map();
const playerGames = new Map();
function playerSaveSlot(user) { return 'player_' + user.id; }
function getPlayerGame(user) {
    if (!playerGames.has(user.id)) {
        const instance = new GameManager();
        try { instance.restore(gameSaves.load(playerSaveSlot(user)).game); } catch (error) { if (error.code !== 'ENOENT') console.warn('Player save not restored:', error.message); }
        playerGames.set(user.id, instance);
    }
    return playerGames.get(user.id);
}
function setSessionCookie(res, token) {
    res.setHeader('Set-Cookie', 'aegis_session=' + token + '; HttpOnly; SameSite=Strict; Path=/; Max-Age=604800');
}
function clearSessionCookie(res) {
    res.setHeader('Set-Cookie', 'aegis_session=; HttpOnly; SameSite=Strict; Path=/; Max-Age=0');
}

const PORT = process.env.PORT || 3000;
const WEB_DIR = path.join(__dirname, 'web');
const SIM_EXE = path.join(__dirname, '..', 'aegis_sim.exe');

// ==========================================
// IN-MEMORY SIMULATION STATE
// ==========================================
const systemState = {
    version: "2.0.0-Alpha",
    kernelVersion: "2.0.0-Alpha",
    architecture: "Dual-Kernel Microkernel (Aegis Core + Linux ABI)",
    startTime: Date.now(),
    layout: "hybrid",
    theme: "quantum-cyan",
    optimizerMode: "WORK",
    
    // Security Status
    securityStatus: {
        firewallActive: true,
        idsActive: true,
        guardAccuracy: 98.7,
        zeroTrustPolicy: "DEFAULT_DENY",
        threatsBlocked: 22,
        quarantinedFiles: [
            { id: 'THR-8821', name: 'ransom_injector.bin', type: 'Ransomware Heuristic', threatLevel: 'CRITICAL', time: '10 mins ago', blocked: true, hash: 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855' },
            { id: 'THR-8822', name: 'mem_escalate_poc', type: 'Privilege Escalation', threatLevel: 'HIGH', time: '25 mins ago', blocked: true, hash: 'ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb' },
            { id: 'THR-8823', name: 'nop_sled_shellcode.raw', type: 'Buffer Overflow Gadget', threatLevel: 'CRITICAL', time: '3 mins ago', blocked: true, hash: '8743b52063cd84097a65d1633f5c74f5' }
        ],
        firewallRules: [
            { id: 1, name: "Block Inbound Telnet (23)", proto: "TCP", port: 23, direction: "INBOUND", action: "DENY", enabled: true },
            { id: 2, name: "Allow Secure Shell (SSH)", proto: "TCP", port: 22, direction: "INBOUND", action: "ALLOW", enabled: true },
            { id: 3, name: "Allow HTTPS Web Traffic", proto: "TCP", port: 443, direction: "OUTBOUND", action: "ALLOW", enabled: true },
            { id: 4, name: "Block Outbound Tor Relays", proto: "TCP", port: 9001, direction: "OUTBOUND", action: "DENY", enabled: true },
            { id: 5, name: "Zero-Trust Kernel IPC Gate", proto: "IPC", port: 0, direction: "INTERNAL", action: "ALLOW", enabled: true },
            { id: 6, name: "Block Unverified DNS over HTTP", proto: "UDP", port: 853, direction: "OUTBOUND", action: "DENY", enabled: true }
        ],
        zeroTrustTokens: [
            { tokenId: "TOK-9921", subject: "aegis_ui_compositor", capability: "CAP_FRAMEBUFFER_WRITE", ttl: "3600s", status: "VALID" },
            { tokenId: "TOK-9922", subject: "neural_monitor", capability: "CAP_KERNEL_TELEMETRY", ttl: "1800s", status: "VALID" },
            { tokenId: "TOK-9923", subject: "aegis_shield_daemon", capability: "CAP_NET_RAW_SOCKET", ttl: "7200s", status: "VALID" }
        ],
        vulnerabilities: [
            { cveId: "CVE-2026-AEGIS-01", name: "Ring 3 Driver Memory Barrier Validation", severity: "LOW", status: "PATCHED", desc: "User-space driver IOCTL required additional boundary validation." },
            { cveId: "CVE-2026-AEGIS-02", name: "Entropy Whitening in /dev/urandom", severity: "INFO", status: "VERIFIED", desc: "Cryptographic pseudo-random generator passed FIPS 140-3 test suites." },
            { cveId: "CVE-2026-AEGIS-03", name: "Shadow Stack Canary Drift", severity: "MEDIUM", status: "PATCHED", desc: "Automated canary randomization applied on thread spawn." }
        ]
    },

    // Virtual Disposable Browser ("Ghost Browser") State
    browser: {
        currentContainerId: null,
        containerStatus: "INACTIVE", // "INACTIVE" | "RAM_ISOLATED" | "SHREDDED"
        ephemeralMemoryAllocatedMb: 512,
        containersDestroyedCount: 4,
        bookmarks: [
            { id: "bm-1", title: "DuckDuckGo Search", url: "https://duckduckgo.com", icon: "🦆", addedAt: "2026-11-01" },
            { id: "bm-2", title: "Wikipedia: The Free Encyclopedia", url: "https://en.wikipedia.org", icon: "📖", addedAt: "2026-11-01" },
            { id: "bm-3", title: "Hacker News (YCombinator)", url: "https://news.ycombinator.com", icon: "🔥", addedAt: "2026-11-01" },
            { id: "bm-4", title: "AegisOS Architecture Whitepaper", url: "https://aegis-os.org/docs", icon: "🛡️", addedAt: "2026-11-01" }
        ],
        history: []
    },

    // Merkle Filesystem & Real File Contents
    filesystem: {
        merkleRoot: "0x8f4d92a15c8e43f07a1b3d5c9e2a7b6f4c8d1e3a5b7c9d0e1f2a3b4c5d6e7f8",
        status: "INTEGRITY_VERIFIED",
        corruptedBlock: null,
        blocks: [
            { id: "blk-0", path: "/boot/vmlinuz-aegis", hash: "a4f912c498e72b1...", size: "14.2 MB", status: "HEALTHY", tier: "NVMe" },
            { id: "blk-1", path: "/boot/initramfs-aegis.img", hash: "6b2e98d13a948c2...", size: "28.5 MB", status: "HEALTHY", tier: "NVMe" },
            { id: "blk-2", path: "/system/lib/libaegis_kernel.so", hash: "3e1a77f98d1a45b...", size: "8.4 MB", status: "HEALTHY", tier: "RAM" },
            { id: "blk-3", path: "/system/security/zero_trust.db", hash: "9d4c21b56ef24a1...", size: "1.2 MB", status: "HEALTHY", tier: "NVMe" },
            { id: "blk-4", path: "/userland/data/user_vault.dat", hash: "8f4d92a15c8e43f...", size: "54.1 MB", status: "HEALTHY", tier: "SSD" },
            { id: "blk-5", path: "/filesystem/merkle_journal.log", hash: "7c5e31a89c423e7...", size: "512 KB", status: "HEALTHY", tier: "NVMe" }
        ],
        files: [
            {
                id: "f-1",
                name: "vmlinuz-aegis",
                path: "/boot/vmlinuz-aegis",
                size: 14889984,
                sizeStr: "14.2 MB",
                perm: "rwxr-xr-x",
                ring: "Ring 0",
                encrypted: false,
                hash: "a4f912c...",
                modified: "2026-11-01 10:14",
                type: "kernel",
                content: "#!/aegis/microkernel\n# AegisOS Core Dual-Kernel Image v2.0-Alpha\n[KERNEL_BOOT]\nARCH=x86_64_MICROKERNEL\nASLR=ENABLED_64BIT\nSCHEDULER=CFS_ADAPTIVE\nROOT_HASH=0x8f4d92a15c8e43f07a1b3d5c9e2a7b6f4c8d1e3a5b7c9d0e1f2a3b4c5d6e7f8\nMERKLE_TREE=ENFORCED\nDRIVER_ISOLATION=RING_3_SANDBOX"
            },
            {
                id: "f-2",
                name: "zero_trust.db",
                path: "/system/security/zero_trust.db",
                size: 1258291,
                sizeStr: "1.2 MB",
                perm: "rw-------",
                ring: "Ring 0",
                encrypted: true,
                hash: "9d4c21b...",
                modified: "2026-11-01 14:22",
                type: "db",
                content: "[ZERO_TRUST_SECURITY_DATABASE]\nDEFAULT_DENY_POLICY=TRUE\nHARDWARE_ACCESS_GATES=STRICT\nALLOWED_CAPABILITIES=CAP_NET_RAW,CAP_STORAGE_ADMIN,CAP_FRAMEBUFFER\nACTIVE_TOKENS=3\nAUDIT_LOG_ROTATION=HOURLY"
            },
            {
                id: "f-3",
                name: "libaegis_kernel.so",
                path: "/system/lib/libaegis_kernel.so",
                size: 8808038,
                sizeStr: "8.4 MB",
                perm: "rwxr-xr-x",
                ring: "Ring 1",
                encrypted: false,
                hash: "3e1a77f...",
                modified: "2026-11-01 12:05",
                type: "library",
                content: "/* AegisOS Core Dynamic Shared Library */\nextern int mmgr_alloc(size_t sz);\nextern void shield_filter_packet(const void *frame);\nextern int ztac_verify_token(const char *token_id);"
            },
            {
                id: "f-4",
                name: "user_vault.dat",
                path: "/userland/data/user_vault.dat",
                size: 56728576,
                sizeStr: "54.1 MB",
                perm: "rw-rw----",
                ring: "Ring 3",
                encrypted: true,
                hash: "8f4d92a...",
                modified: "2026-11-01 14:50",
                type: "data",
                content: "CLASSIFIED USER VAULT\nUser: Root Administrator\nAuth Method: Biometric + Ephemeral Hardware Token\nVault Path: /vault/secure_enclave.dat\nStatus: AES-256-XTS Encrypted"
            },
            {
                id: "f-5",
                name: "merkle_journal.log",
                path: "/filesystem/merkle_journal.log",
                size: 524288,
                sizeStr: "512 KB",
                perm: "rw-r--r--",
                ring: "Ring 0",
                encrypted: true,
                hash: "7c5e31a...",
                modified: "2026-11-01 15:10",
                type: "log",
                content: "[2026-11-01 10:00:00] MERKLE_JOURNAL_INITIALIZED root=0x8f4d92a1...\n[2026-11-01 12:00:00] CHECKPOINT_CREATED id=chkpt-0001 (Clean Boot)\n[2026-11-01 14:30:00] SNAPSHOT_TAKEN id=chkpt-0002 (Pre-Update)\n[2026-11-01 15:10:00] INTEGRITY_AUDIT: All 6 branch hashes verified healthy."
            },
            {
                id: "f-6",
                name: "firewall_rules.conf",
                path: "/etc/aegis/firewall_rules.conf",
                size: 4096,
                sizeStr: "4 KB",
                perm: "rw-r--r--",
                ring: "Ring 2",
                encrypted: false,
                hash: "1b9a4c8...",
                modified: "2026-11-01 13:30",
                type: "config",
                content: "# Aegis Shield Stateful Firewall Configuration\nFIREWALL_ENABLED=1\nSTATEFUL_INSPECTION=1\nNOP_SLED_BLOCKER=1\nPORT_SCAN_THRESHOLD=10/sec\nDEFAULT_ACTION=DROP\nALLOW_PORTS=22,443,80"
            },
            {
                id: "f-7",
                name: "classified_payload.enc",
                path: "/vault/classified_payload.enc",
                size: 10485760,
                sizeStr: "10.0 MB",
                perm: "r--------",
                ring: "Ring 0",
                encrypted: true,
                hash: "fe248a9...",
                modified: "2026-11-01 09:00",
                type: "vault",
                content: "[TOP_SECRET_ENCLAVE_DATA]\nCIPHER: AES-256-GCM\nKEY_ID: HSM-SECURE-KEY-882\nPAYLOAD: 88f9104c88e9124018b871cda912bc78"
            }
            ,{
                id: "f-8",
                name: "svc-sync-cache.bin",
                path: "/var/cache/svc-sync-cache.bin",
                size: 24576,
                sizeStr: "24 KB",
                perm: "rwxrwxr-x",
                ring: "Ring 3",
                encrypted: false,
                hash: "d91f0a7e...",
                modified: "2026-11-01 02:11",
                type: "binary",
                content: "AEGIS TRAINING ARTEFACT\\nservice=sync-worker\\nparent=systemd\\nremote=185.220.101.5:8443\\nretry=15s\\nNOTE: simulated suspicious persistence artefact for campaign investigation only."
            }
        ],
                corruptedFiles: [
            {
                id: "corrupt-1",
                name: "financial_audit.db.corrupt",
                path: "/userland/data/financial_audit.db.corrupt",
                damageType: "HEADER_CORRUPTED",
                originalName: "financial_audit.db",
                recoveredPath: "/userland/data/financial_audit.db",
                rawHex: "00 00 00 00 32 30 32 36 01 00 00 00 53 51 4c 49 54 45 5f 41 45 47 49 53",
                asciiDump: "....2026....SQLITE_AEGIS",
                corruptOffset: "0x00000000",
                expectedMagic: "41 45 47 49",
                expectedAscii: "AEGIS_DB",
                hint: "Damaged SQLite/Aegis inode header. Replace null bytes 00000000 at offset 0x0 with magic header '41 45 47 49' ('AEGI').",
                status: "UNRECOVERED",
                cleanContent: "[AEGIS FINANCIAL AUDIT REPORT 2026]\nMERKLE_PROOF: 0x9f1a8e2b\nTOTAL_BALANCES_VERIFIED: $14,250,000.00\nINTEGRITY: 100% CLEAN - NO EMBEZZLEMENT OR UNAUTHORIZED OUTFLOWS FOUND."
            },
            {
                id: "corrupt-2",
                name: "kernel_symbols.map.enc",
                path: "/vault/kernel_symbols.map.enc",
                damageType: "RANSOMWARE_ENCRYPTED",
                originalName: "kernel_symbols.map",
                recoveredPath: "/system/kernel_symbols.map",
                rawHex: "78 74 73 2d 61 65 73 2d 32 35 36 3a 30 78 61 65 39 31 35 5f 6b 72 6e 5f",
                asciiDump: "xts-aes-256:0xae915_krn_",
                corruptOffset: "0x00000000",
                expectedMagic: "0xAE915_KRN_SEC",
                expectedAscii: "SYM_TABLE",
                hint: "Ransomware locked symbol table. Decryption key can be scraped from memory dump '/tmp/core.dump' or enter key: '0xAE915_KRN_SEC'.",
                status: "UNRECOVERED",
                cleanContent: "[AEGIS KERNEL SYMBOL MAP v2.0]\n0x0000000000001000 T _start\n0x0000000000001020 T microkernel_init\n0x0000000000001880 T cfs_schedule\n0x0000000000002100 T shield_packet_inspect\n0x0000000000003040 T merkle_commit_root"
            },
            {
                id: "corrupt-3",
                name: "net_filter.sys.damaged",
                path: "/system/drivers/net_filter.sys.damaged",
                damageType: "ELF_MAGIC_WIPED",
                originalName: "net_filter.sys",
                recoveredPath: "/system/drivers/net_filter.sys",
                rawHex: "de ad be ef 02 01 01 00 00 00 00 00 00 00 00 00 03 00 3e 00 01 00 00 00",
                asciiDump: "....>.......>...",
                corruptOffset: "0x00000000",
                expectedMagic: "7F 45 4C 46",
                expectedAscii: ".ELF",
                hint: "Driver binary magic bytes overwritten with DEADBEEF by rootkit. Restore ELF signature '7F 45 4C 46' ('\\x7fELF').",
                status: "UNRECOVERED",
                cleanContent: "\x7fELF\x02\x01\x01\x00 [AEGIS HIGH-PERFORMANCE NETWORK FILTER DRIVER v2.4 - SIGNED KERNEL RING 0 MODULE]"
            }
        ],
        snapshots: [
            { id: "chkpt-0001", name: "Clean Boot Baseline", timestamp: new Date(Date.now() - 3600000).toLocaleTimeString(), size: "482 MB", bootable: true },
            { id: "chkpt-0002", name: "Pre-Update Snapshot", timestamp: new Date(Date.now() - 1800000).toLocaleTimeString(), size: "512 MB", bootable: true },
            { id: "chkpt-0003", name: "System Restore Point Alpha", timestamp: new Date(Date.now() - 300000).toLocaleTimeString(), size: "515 MB", bootable: true }
        ]
    },

    // Storage Engine State
    storage: {
        totalCapacityGb: 2560,
        usedCapacityGb: 785.4,
        freeCapacityGb: 1774.6,
        dedupSavingsGb: 48.2,
        dedupRatio: "1.28x",
        compressionSavingsGb: 64.5,
        compressionRatio: "2.35x (ZSTD/LZ4)",
        fragmentationPercent: 3.8,
        ssdHealthPercent: 99.4,
        trimActive: true,
        tiers: [
            { id: "tier-1", name: "Tier 1: L1 RAM Buffer", type: "DDR5 ECC RAM", capacityGb: 16, usedGb: 5.6, latencyUs: 0.04, bandwidthMbps: 12800, status: "OPTIMAL" },
            { id: "tier-2", name: "Tier 2: L2 NVMe SSD", type: "PCIe 4.0 NVMe", capacityGb: 512, usedGb: 142.8, latencyUs: 22.0, bandwidthMbps: 4800, status: "OPTIMAL" },
            { id: "tier-3", name: "Tier 3: L3 SATA SSD", type: "SATA III TLC", capacityGb: 2048, usedGb: 637.0, latencyUs: 140.0, bandwidthMbps: 560, status: "HEALTHY" },
            { id: "tier-4", name: "Tier 4: Cold Archive", type: "Immutable Snapshots", capacityGb: 1024, usedGb: 124.0, latencyUs: 850.0, bandwidthMbps: 250, status: "ENCRYPTED" }
        ]
    },

    
    // Live Server Services & Triage State
    services: [
        {
            id: "svc-apigateway",
            name: "aegis-apigateway.service",
            displayName: "Aegis Public Ingress Gateway",
            port: 8443,
            proto: "TCP/HTTPS",
            status: "PORT_CONFLICT",
            roguePid: 4819,
            rogueProcess: "rogue_c2_proxy",
            issues: ["Port 8443 hijacked by unauthorized PID 4819 (rogue_c2_proxy)", "Public ingress dropping 94% of user requests"],
            logs: [
                "[02:13:01] BIND: Socket error EADDRINUSE 0.0.0.0:8443",
                "[02:13:02] ALERT: Conflicting process PID 4819 (rogue_c2_proxy) occupying socket",
                "[02:13:05] WARNING: Ingress gateway offline. Healthcheck failing."
            ],
            canKillRogue: true,
            canRestart: true
        },
        {
            id: "svc-authbroker",
            name: "aegis-authbroker.service",
            displayName: "Zero-Trust Identity & Token Broker",
            port: 8080,
            proto: "TCP/GRPC",
            status: "ACTIVE",
            issues: [],
            logs: [
                "[02:10:00] Ephemeral token ring online. 3 active grants.",
                "[02:12:00] Public keys rotated successfully. 0 unauthorized transitions."
            ],
            canRestart: true
        },
        {
            id: "svc-edgerouter",
            name: "aegis-edgerouter.service",
            displayName: "Edge Reverse Proxy & Rate Limiter",
            port: 443,
            proto: "TCP/TLS",
            status: "DEGRADED",
            issues: ["Config /etc/aegis_gateway.conf redirects to untrusted 185.220.101.5:8443"],
            logs: [
                "[02:14:10] PARSE_CONFIG: Found malicious route 'proxy_pass http://185.220.101.5:8443;'",
                "[02:14:12] WARNING: Inbound user traffic leaking to external untrusted node!"
            ],
            canPatchConfig: true,
            canRestart: true
        },
        {
            id: "svc-dbcluster",
            name: "aegis-dbcluster.service",
            displayName: "Merkle Ledger & State Cluster",
            port: 5432,
            proto: "TCP/POSTGRES",
            status: "ACTIVE",
            issues: [],
            logs: [
                "[02:00:00] Merkle database replication quorum intact (3/3 nodes synced)."
            ],
            canRestart: true
        }
    ],

    // Shared Zero-Trust Capability Token Registry
    tokens: [
        { id: "TOK-9921", subject: "aegis_ui_compositor", cap: "CAP_FRAMEBUFFER_WRITE", ttl: "3600s", ring: "Ring 3", status: "VALID" },
        { id: "TOK-9922", subject: "neural_monitor", cap: "CAP_KERNEL_TELEMETRY", ttl: "1800s", ring: "Ring 1", status: "VALID" },
        { id: "TOK-9923", subject: "aegis_shield_daemon", cap: "CAP_NET_RAW_SOCKET", ttl: "7200s", ring: "Ring 0", status: "VALID" },
        { id: "TOK-KERNEL-ESCALATED", subject: "kernel_sync_worker", cap: "CAP_NET_RAW_SOCKET", ttl: "Indefinite", ring: "Ring 0 (ANOMALOUS)", status: "ANOMALOUS_ESCALATION", compromised: true }
    ],

    // Telemetry
    telemetry: {
        cpuUsage: 18.4,
        ramUsage: 34.2,
        totalRamMb: 16384,
        usedRamMb: 5603,
        networkKbps: 420.5,
        entropyRate: 0.88,
        activeProcesses: 54,
        totalEventsLogged: 49450,
        eventRates: { fileOps: 142, memoryOps: 890, networkPackets: 210, processSpawns: 3 }
    }
};

const MIME_TYPES = {
    '.html': 'text/html; charset=UTF-8',
    '.css': 'text/css; charset=UTF-8',
    '.js': 'application/javascript; charset=UTF-8',
    '.json': 'application/json',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.svg': 'image/svg+xml',
    '.ico': 'image/x-icon',
    '.woff2': 'font/woff2'
};

function sendJson(res, statusCode, data) {
    res.writeHead(statusCode, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify(data));
}

function parseBody(req, callback) {
    let body = '';
    req.on('data', chunk => { body += chunk; });
    req.on('end', () => {
        try {
            const data = body ? JSON.parse(body) : {};
            callback(null, data);
        } catch (e) {
            callback(e);
        }
    });
}

// Request Server
const server = http.createServer((req, res) => {
    if (req.method === 'OPTIONS') {
        res.writeHead(204);
        return res.end();
    }

    const parsedUrl = new URL(req.url, `http://${req.headers.host || 'localhost'}`);
    const pathname = parsedUrl.pathname;

    // API Routes
    if (pathname.startsWith('/api/')) {
        handleApi(req, res, pathname, parsedUrl);
        return;
    }

    // Static Web File Serving
    let filePath = path.join(WEB_DIR, (pathname === '/' || pathname === '/game' || pathname === '/game.html') ? 'index.html' : pathname);
    filePath = path.normalize(filePath);

    if (!filePath.toLowerCase().startsWith(WEB_DIR.toLowerCase())) {
        res.writeHead(403, { 'Content-Type': 'text/plain' });
        return res.end('Access Denied');
    }

    fs.stat(filePath, (err, stats) => {
        if (err || !stats.isFile()) {
            if (!path.extname(pathname)) {
                filePath = path.join(WEB_DIR, 'index.html');
            } else {
                res.writeHead(404, { 'Content-Type': 'text/plain' });
                return res.end('404 Not Found');
            }
        }

        const ext = path.extname(filePath).toLowerCase();
        const contentType = MIME_TYPES[ext] || 'application/octet-stream';

        fs.readFile(filePath, (readErr, content) => {
            if (readErr) {
                res.writeHead(500, { 'Content-Type': 'text/plain' });
                return res.end('Internal Server Error');
            }
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(content);
        });
    });
});

// API Dispatcher
function handleApi(req, res, pathname, url) {
    // Player identity endpoints are local to this AegisOS instance.
    if (pathname === '/api/auth/me' && req.method === 'GET') {
        const session = authManager.getSession(req);
        return sendJson(res, 200, { authenticated: !!session, user: session ? session.user : null, needsProfile: !authManager.hasAccounts() });
    }
    if (pathname === '/api/auth/register' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err) return sendJson(res, 400, { error: 'Invalid JSON body.' });
            try {
                const result = authManager.createAccount(data || {}, req.socket.remoteAddress);
                setSessionCookie(res, result.token);
                return sendJson(res, 201, { success: true, user: result.user });
            } catch (error) { return sendJson(res, 400, { error: error.message }); }
        });
        return;
    }
    if (pathname === '/api/auth/login' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err) return sendJson(res, 400, { error: 'Invalid JSON body.' });
            try {
                const result = authManager.login(data || {}, req.socket.remoteAddress);
                setSessionCookie(res, result.token);
                return sendJson(res, 200, { success: true, user: result.user });
            } catch (error) { return sendJson(res, 401, { error: error.message }); }
        });
        return;
    }
    if (pathname === '/api/auth/unlock' && req.method === 'POST') {
        const session = authManager.getSession(req);
        if (!session) return sendJson(res, 401, { error: 'Your player session has expired. Sign in again.' });
        parseBody(req, (err, data) => {
            if (err || !authManager.unlock(session.user.id, data && data.password, req.socket.remoteAddress)) return sendJson(res, 401, { error: 'Access phrase is incorrect.' });
            return sendJson(res, 200, { success: true });
        });
        return;
    }
    if (pathname === '/api/auth/logout' && req.method === 'POST') {
        authManager.logout(req);
        clearSessionCookie(res);
        return sendJson(res, 200, { success: true });
    }

    // Per-account native desktop campaign progress. The browser is only a cache;
    // authoritative progress is stored in the authenticated operator's server-side slot.
    if (pathname === '/api/campaign/state') {
        const session = authManager.getSession(req);
        if (!session) return sendJson(res, 401, { error: 'Authenticate to access campaign progress.' });
        const safeUserId = String(session.user.id || '').replace(/[^a-zA-Z0-9_-]/g, '');
        if (!safeUserId) return sendJson(res, 401, { error: 'Invalid operator identity.' });
        const savePath = path.join(campaignDataDir, safeUserId + '.json');
        if (req.method === 'GET') {
            if (campaignMemoryCache.has(safeUserId)) {
                const cached = campaignMemoryCache.get(safeUserId);
                return sendJson(res, 200, { state: cached.state || null, updatedAt: cached.updatedAt || null, cached: true });
            }
            try {
                const saved = JSON.parse(fs.readFileSync(savePath, 'utf8'));
                campaignMemoryCache.set(safeUserId, { state: saved.state || null, updatedAt: saved.updatedAt || null });
                return sendJson(res, 200, { state: saved.state || null, updatedAt: saved.updatedAt || null, cached: false });
            } catch (error) {
                if (error.code === 'ENOENT') {
                    campaignMemoryCache.set(safeUserId, { state: null, updatedAt: null });
                    return sendJson(res, 200, { state: null, updatedAt: null, cached: true });
                }
                return sendJson(res, 500, { error: 'Campaign save could not be read.' });
            }
        }
        if (req.method === 'POST') {
            parseBody(req, (err, data) => {
                if (err || !data || !data.state || data.state.v !== 1 || !Number.isInteger(data.state.index) || data.state.index < 0 || data.state.index > 2 || !Array.isArray(data.state.done) || !Array.isArray(data.state.log)) {
                    return sendJson(res, 400, { error: 'Invalid campaign save payload.' });
                }
                const serialized = JSON.stringify({ state: data.state, updatedAt: Date.now() });
                if (serialized.length > 100000 || data.state.done.length > 3 || data.state.log.length > 200) {
                    return sendJson(res, 413, { error: 'Campaign save exceeds allowed limits.' });
                }
                const tempPath = savePath + '.tmp';
                try {
                    campaignMemoryCache.set(safeUserId, { state: data.state, updatedAt: Date.now() });
                    fs.writeFileSync(tempPath, serialized, { mode: 0o600 });
                    fs.renameSync(tempPath, savePath);
                    return sendJson(res, 200, { success: true, updatedAt: Date.now(), cached: true });
                } catch (error) {
                    try { if (fs.existsSync(tempPath)) fs.unlinkSync(tempPath); } catch (_) {}
                    return sendJson(res, 500, { error: 'Campaign save could not be written.' });
                }
            });
            return;
        }
        return sendJson(res, 405, { error: 'Method not allowed.' });
    }

    // ----------------------------------------------------
    // AEGIS GAME API - isolated fictional game state only
    // Every campaign request is bound to an authenticated local player.
    // ----------------------------------------------------
    if (pathname.startsWith('/api/game/')) {
        const session = authManager.getSession(req);
        if (!session) return sendJson(res, 401, { error: 'Authenticate to access the campaign.' });
        const gameManager = getPlayerGame(session.user);
    if (pathname === '/api/game/state' && req.method === 'GET') {
        return sendJson(res, 200, gameManager.getState());
    }
    if (pathname === '/api/game/missions' && req.method === 'GET') {
        return sendJson(res, 200, { missions: gameManager.missions.list() });
    }
    if (pathname === '/api/game/start' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err) return sendJson(res, 400, { error: 'Invalid JSON body.' });
            try { const mission = gameManager.startMission((data && data.missionId) || 'mission-01'); gameSaves.save(gameManager, playerSaveSlot(session.user)); return sendJson(res, 200, { success: true, mission, state: gameManager.getState() }); }
            catch (error) { return sendJson(res, 400, { error: error.message }); }
        });
        return;
    }
    if (pathname === '/api/game/action' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data || typeof data.action !== 'string') return sendJson(res, 400, { error: 'A valid action is required.' });
            try { const result = gameManager.performAction(data.action, data.payload || {}); if (result.ok) gameSaves.save(gameManager, playerSaveSlot(session.user)); return sendJson(res, result.ok ? 200 : 409, result); }
            catch (error) { return sendJson(res, 400, { error: error.message }); }
        });
        return;
    }
    if (pathname === '/api/game/research' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data || typeof data.nodeId !== 'string') return sendJson(res, 400, { error: 'Research node ID is required.' });
            try {
                const result = gameManager.unlockResearch(data.nodeId);
                if (result.ok) gameSaves.save(gameManager, playerSaveSlot(session.user));
                return sendJson(res, result.ok ? 200 : 409, { ...result, state: gameManager.getState() });
            } catch (error) { return sendJson(res, 400, { error: error.message }); }
        });
        return;
    }
    if (pathname === '/api/game/save' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err) return sendJson(res, 400, { error: 'Invalid JSON body.' });
            try { return sendJson(res, 200, { success: true, ...gameSaves.save(gameManager, playerSaveSlot(session.user)) }); }
            catch (error) { return sendJson(res, 400, { error: error.message }); }
        });
        return;
    }
    if (pathname === '/api/game/load' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err) return sendJson(res, 400, { error: 'Invalid JSON body.' });
            try { const payload = gameSaves.load(playerSaveSlot(session.user)); return sendJson(res, 200, { success: true, state: gameManager.restore(payload.game), savedAt: payload.savedAt }); }
            catch (error) { return sendJson(res, 404, { error: 'Save could not be loaded: ' + error.message }); }
        });
        return;
    }
    }

    // ----------------------------------------------------
    // 1. SYSTEM STATUS & TELEMETRY
    // ----------------------------------------------------
    if (pathname === '/api/system/status' && req.method === 'GET') {
        const uptimeSeconds = Math.floor((Date.now() - systemState.startTime) / 1000);
        return sendJson(res, 200, {
            ...systemState,
            uptimeSeconds,
            nativeSimAvailable: fs.existsSync(SIM_EXE)
        });
    }

    if (pathname === '/api/neural/telemetry' && req.method === 'GET') {
        const cpuJitter = (Math.sin(Date.now() / 1500) * 5) + (Math.random() * 4);
        let targetCpu = 18;
        if (systemState.optimizerMode === 'GAME') targetCpu = 42;
        else if (systemState.optimizerMode === 'ECO') targetCpu = 10;
        
        systemState.telemetry.cpuUsage = Math.max(5, Math.min(99, Math.round((targetCpu + cpuJitter) * 10) / 10));
        systemState.telemetry.totalEventsLogged += Math.floor(Math.random() * 25) + 10;

        return sendJson(res, 200, {
            telemetry: systemState.telemetry,
            timestamp: Date.now()
        });
    }

    // ----------------------------------------------------
    // 2. FILE MANAGEMENT (READ, WRITE, CREATE, EDIT, ENCRYPT, DELETE)
    // ----------------------------------------------------
    
    // ----------------------------------------------------
    // FILE RECOVERY & CRYPTOGRAPHIC CARVER ENDPOINTS
    // ----------------------------------------------------
    
    // ----------------------------------------------------
    // CRITICAL BREACH FILE WIPE & RECOVERY PROTOCOL
    // ----------------------------------------------------
    if (pathname === '/api/system/trigger-wiped' && req.method === 'POST') {
        const userFiles = systemState.filesystem.files.filter(f => f.path.startsWith('/userland/data/'));
        userFiles.forEach(f => {
            f.content = "[CORRUPTED INODE: DATA PURGED BY CRITICAL ZERO-DAY BREACH]\nRestore this file using the File Recovery & Hex Carver in Files & Storage.";
            f.size = Buffer.byteLength(f.content, 'utf8');
            f.sizeStr = `${Math.max(1, Math.round(f.size / 1024))} KB`;
            f.hash = "0xBAD_WIPED_" + Math.random().toString(16).substring(2, 6);
        });

        const list = systemState.filesystem.corruptedFiles || [];
        const vaultCorrupt = list.find(f => f.name === 'user_vault.dat.corrupt');
        if (!vaultCorrupt) {
            list.push({
                id: `corrupt-wiped-${Date.now()}`,
                name: "user_vault.dat.corrupt",
                path: "/userland/data/user_vault.dat.corrupt",
                damageType: "RANSOMWARE_WIPED_HEADER",
                originalName: "user_vault.dat",
                recoveredPath: "/userland/data/user_vault.dat",
                rawHex: "00 00 00 00 55 53 45 52 5f 56 41 55 4c 54 5f 53 45 43 55 52 45 5f 30 31",
                asciiDump: "....USER_VAULT_SEC_01",
                corruptOffset: "0x00000000",
                expectedMagic: "56 41 55 4c",
                expectedAscii: "VAULT_ENC",
                hint: "Wiped user vault header. Replace null bytes with magic header '56 41 55 4c' ('VAUL') to restore cryptographic enclave.",
                status: "UNRECOVERED",
                cleanContent: "CLASSIFIED USER VAULT\nUser: Root Administrator\nAuth Method: Biometric + Hardware Token\nStatus: Restored and Verified Clean."
            });
        }
        systemState.filesystem.status = "CORRUPTED";
        return sendJson(res, 200, {
            success: true,
            message: "Critical breach executed: local files wiped and moved to corrupted manifest. Recovery protocol initiated.",
            wipedCount: userFiles.length
        });
    }

    if (pathname === '/api/files/corrupted-list' && req.method === 'GET') {
        return sendJson(res, 200, {
            corruptedFiles: systemState.filesystem.corruptedFiles || []
        });
    }

    if (pathname === '/api/files/recover' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.fileId) return sendJson(res, 400, { error: 'File ID is required' });
            const list = systemState.filesystem.corruptedFiles || [];
            const target = list.find(f => f.id === data.fileId || f.name === data.fileId);
            if (!target) return sendJson(res, 404, { error: 'Corrupted file inode not found' });

            const inputStr = String(data.hexOrKey || '').replace(/\s+/g, '').toLowerCase();
            const expectedMagicStr = String(target.expectedMagic || '').replace(/\s+/g, '').toLowerCase();
            const expectedAsciiStr = String(target.expectedAscii || '').replace(/\s+/g, '').toLowerCase();

            const isValid = inputStr && (inputStr === expectedMagicStr || inputStr === expectedAsciiStr || inputStr.includes(expectedMagicStr) || expectedMagicStr.includes(inputStr));
            if (!isValid) {
                return sendJson(res, 400, {
                    success: false,
                    error: `Invalid header bytes or decryption key for ${target.name}. Check hint: ${target.hint}`
                });
            }

            target.status = "RECOVERED";
            let existingFile = systemState.filesystem.files.find(f => f.path === target.recoveredPath || f.name === target.originalName);
            if (!existingFile) {
                existingFile = {
                    id: `f-rec-${Date.now()}`,
                    name: target.originalName,
                    path: target.recoveredPath,
                    size: Buffer.byteLength(target.cleanContent, 'utf8'),
                    sizeStr: `${Math.max(1, Math.round(Buffer.byteLength(target.cleanContent, 'utf8') / 1024))} KB`,
                    perm: "rw-r--r--",
                    ring: "Ring 3",
                    encrypted: false,
                    hash: Math.random().toString(16).substring(2, 10) + "...",
                    modified: new Date().toISOString().substring(0, 16).replace('T', ' '),
                    type: path.extname(target.originalName).replace('.', '') || "text",
                    content: target.cleanContent
                };
                systemState.filesystem.files.push(existingFile);
            } else {
                existingFile.content = target.cleanContent;
                existingFile.hash = Math.random().toString(16).substring(2, 10) + "...";
            }

            return sendJson(res, 200, {
                success: true,
                message: `Successfully carved and restored ${target.originalName} (Inode recovered to clean state)!`,
                recoveredFile: existingFile
            });
        });
        return;
    }

    // ----------------------------------------------------
    // LIVE SERVER DIAGNOSTICS & SYSTEM TRIAGE ENDPOINTS
    // ----------------------------------------------------
    if (pathname === '/api/troubleshoot/services' && req.method === 'GET') {
        return sendJson(res, 200, {
            services: systemState.services || []
        });
    }

    if (pathname === '/api/troubleshoot/fix-service' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.serviceId || !data.action) return sendJson(res, 400, { error: 'Service ID and action required' });
            const svc = (systemState.services || []).find(s => s.id === data.serviceId || s.name === data.serviceId);
            if (!svc) return sendJson(res, 404, { error: 'Service not found' });

            if (data.action === 'kill-rogue-pid') {
                svc.roguePid = null;
                svc.issues = [];
                svc.logs.push(`[${new Date().toLocaleTimeString()}] SIGKILL dispatched to PID 4819. Socket 0.0.0.0:${svc.port} freed.`);
                return sendJson(res, 200, {
                    success: true,
                    message: `Terminated rogue process PID 4819. Port ${svc.port} is now unblocked and ready for daemon bind.`,
                    service: svc
                });
            }

            if (data.action === 'patch-config') {
                svc.issues = [];
                svc.logs.push(`[${new Date().toLocaleTimeString()}] CONFIG PATCH: Clean upstream '127.0.0.1:8443' restored. Removed untrusted external redirect.`);
                return sendJson(res, 200, {
                    success: true,
                    message: "Patched /etc/aegis_gateway.conf. Malicious redirect removed and upstream routing restored.",
                    service: svc
                });
            }

            if (data.action === 'restart-service') {
                if (svc.roguePid) {
                    return sendJson(res, 409, {
                        success: false,
                        error: `Cannot start ${svc.name}: Port ${svc.port} is still occupied by rogue PID ${svc.roguePid}! Kill the rogue process first.`
                    });
                }
                if (svc.issues && svc.issues.length > 0) {
                    return sendJson(res, 409, {
                        success: false,
                        error: `Cannot start ${svc.name}: Config errors still pending: ${svc.issues.join('; ')}`
                    });
                }
                svc.status = "ACTIVE";
                svc.logs.push(`[${new Date().toLocaleTimeString()}] SERVICE BOOT: ${svc.displayName} started on port ${svc.port} (HTTP 200 OK).`);
                return sendJson(res, 200, {
                    success: true,
                    message: `Service '${svc.displayName}' restarted successfully. Status: ACTIVE (200 OK).`,
                    service: svc
                });
            }

            return sendJson(res, 400, { error: 'Unknown action' });
        });
        return;
    }

    // ----------------------------------------------------
    // ZERO-TRUST CAPABILITY TOKENS ENDPOINTS
    // ----------------------------------------------------
    if (pathname === '/api/shield/tokens' && req.method === 'GET') {
        return sendJson(res, 200, {
            tokens: systemState.tokens || []
        });
    }

    if (pathname === '/api/shield/revoke-token' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.tokenId) return sendJson(res, 400, { error: 'Token ID is required' });
            const list = systemState.tokens || [];
            const token = list.find(t => t.id === data.tokenId);
            if (!token) return sendJson(res, 404, { error: 'Token not found' });
            token.status = "REVOKED";
            token.compromised = false;
            token.revoked = true;
            return sendJson(res, 200, {
                success: true,
                message: `Zero-Trust Capability ${token.id} (${token.cap}) granted to ${token.subject} has been revoked and quarantined.`,
                token
            });
        });
        return;
    }

    if (pathname === '/api/files/list' && req.method === 'GET') {
        return sendJson(res, 200, {
            files: systemState.filesystem.files,
            totalFiles: systemState.filesystem.files.length,
            merkleRoot: systemState.filesystem.merkleRoot,
            status: systemState.filesystem.status
        });
    }

    if (pathname === '/api/files/read' && req.method === 'GET') {
        const fileId = url.searchParams.get('id');
        const filePath = url.searchParams.get('path');
        const file = systemState.filesystem.files.find(f => (fileId && f.id === fileId) || (filePath && f.path === filePath));
        if (!file) return sendJson(res, 404, { error: 'File not found' });
        return sendJson(res, 200, { file, content: file.content });
    }

    if (pathname === '/api/files/write' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.path) return sendJson(res, 400, { error: 'Path required' });
            let file = systemState.filesystem.files.find(f => f.path === data.path || f.id === data.fileId);
            
            if (file) {
                file.content = data.content || '';
                file.size = Buffer.byteLength(file.content, 'utf8');
                file.sizeStr = `${Math.max(1, Math.round(file.size / 1024))} KB`;
                file.hash = Math.random().toString(16).substring(2, 10) + "...";
                file.modified = new Date().toISOString().substring(0, 16).replace('T', ' ');
            } else {
                const name = path.basename(data.path);
                file = {
                    id: `f-${Date.now()}`,
                    name: name,
                    path: data.path,
                    size: Buffer.byteLength(data.content || '', 'utf8'),
                    sizeStr: `${Math.max(1, Math.round(Buffer.byteLength(data.content || '', 'utf8') / 1024))} KB`,
                    perm: data.perm || "rw-r--r--",
                    ring: data.ring || "Ring 3",
                    encrypted: !!data.encrypted,
                    hash: Math.random().toString(16).substring(2, 10) + "...",
                    modified: new Date().toISOString().substring(0, 16).replace('T', ' '),
                    type: path.extname(name).replace('.', '') || "text",
                    content: data.content || ""
                };
                systemState.filesystem.files.push(file);
            }
            return sendJson(res, 200, { success: true, file });
        });
        return;
    }

    if (pathname === '/api/files/create' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.name) return sendJson(res, 400, { error: 'File name required' });
            const filePath = data.path || `/userland/data/${data.name}`;
            const content = data.content || "AegisOS Protected Document Payload";
            const newFile = {
                id: `f-${Date.now()}`,
                name: data.name,
                path: filePath,
                size: Buffer.byteLength(content, 'utf8'),
                sizeStr: `${Math.max(1, Math.round(Buffer.byteLength(content, 'utf8') / 1024))} KB`,
                perm: data.perm || "rw-r--r--",
                ring: data.ring || "Ring 3",
                encrypted: !!data.encrypted,
                hash: Math.random().toString(16).substring(2, 10) + "...",
                modified: new Date().toISOString().substring(0, 16).replace('T', ' '),
                type: data.type || "text",
                content: content
            };
            systemState.filesystem.files.push(newFile);
            return sendJson(res, 200, { success: true, file: newFile });
        });
        return;
    }

    if (pathname === '/api/files/toggle-encryption' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.fileId) return sendJson(res, 400, { error: 'File ID required' });
            const target = systemState.filesystem.files.find(f => f.id === data.fileId);
            if (!target) return sendJson(res, 404, { error: 'File not found' });
            target.encrypted = !target.encrypted;
            target.hash = Math.random().toString(16).substring(2, 10) + "...";
            return sendJson(res, 200, {
                success: true,
                encrypted: target.encrypted,
                file: target,
                cipher: target.encrypted ? "AES-256-XTS Hardware Accelerated" : "Plaintext Decrypted"
            });
        });
        return;
    }

    if (pathname === '/api/files/delete' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.fileId) return sendJson(res, 400, { error: 'File ID required' });
            const idx = systemState.filesystem.files.findIndex(f => f.id === data.fileId);
            if (idx === -1) return sendJson(res, 404, { error: 'File not found' });
            const deleted = systemState.filesystem.files.splice(idx, 1)[0];
            return sendJson(res, 200, { success: true, deletedFile: deleted });
        });
        return;
    }

    // ----------------------------------------------------
    // 3. VIRTUAL DISPOSABLE BROWSER ("GHOST BROWSER")
    // ----------------------------------------------------
    if (pathname === '/api/browser/new-container' && req.method === 'POST') {
        const containerId = `MICRO-VM-GHOST-${Math.floor(Math.random() * 90000) + 10000}`;
        systemState.browser.currentContainerId = containerId;
        systemState.browser.containerStatus = "RAM_ISOLATED";

        return sendJson(res, 200, {
            success: true,
            containerId,
            status: "RAM_ISOLATED",
            ramAllocatedMb: systemState.browser.ephemeralMemoryAllocatedMb,
            airgapProtection: "ACTIVE",
            message: "Ephemeral RAM-isolated browser container provisioned. All internet interaction is strictly quarantined. Data wipes automatically upon closing."
        });
    }

    if (pathname === '/api/browser/destroy-container' && req.method === 'POST') {
        const destroyedId = systemState.browser.currentContainerId || "MICRO-VM-GHOST-ACTIVE";
        systemState.browser.currentContainerId = null;
        systemState.browser.containerStatus = "SHREDDED";
        systemState.browser.containersDestroyedCount++;

        return sendJson(res, 200, {
            success: true,
            shreddedContainerId: destroyedId,
            wipeAlgorithm: "DoD 5220.22-M 3-Pass 0x00 Cryptographic Shred",
            dataRetainedOnHost: "ONLY_SANITIZED_BOOKMARKS",
            threatsMitigated: "All cookies, trackers, cache, and drive-by scripts vaporized.",
            totalContainersPurged: systemState.browser.containersDestroyedCount
        });
    }

    if (pathname === '/api/browser/save-bookmark' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.url) return sendJson(res, 400, { error: 'URL required' });
            const bookmark = {
                id: `bm-${Date.now()}`,
                title: data.title || data.url,
                url: data.url,
                icon: "🌐",
                addedAt: new Date().toLocaleDateString()
            };
            systemState.browser.bookmarks.push(bookmark);
            return sendJson(res, 200, { success: true, bookmark });
        });
        return;
    }

    // ----------------------------------------------------
    // 4. STORAGE OPTIMIZER & TIERED ENGINE
    // ----------------------------------------------------
    if (pathname === '/api/storage/info' && req.method === 'GET') {
        return sendJson(res, 200, {
            storage: systemState.storage,
            timestamp: Date.now()
        });
    }

    if (pathname === '/api/storage/optimize' && req.method === 'POST') {
        const reclaimedMb = Math.floor(Math.random() * 850) + 420;
        systemState.storage.dedupSavingsGb = Math.round((systemState.storage.dedupSavingsGb + (reclaimedMb / 1024)) * 10) / 10;
        systemState.storage.fragmentationPercent = Math.max(0.4, Math.round((systemState.storage.fragmentationPercent * 0.4) * 10) / 10);
        systemState.storage.freeCapacityGb = Math.round((systemState.storage.freeCapacityGb + (reclaimedMb / 1024)) * 10) / 10;
        systemState.storage.usedCapacityGb = Math.round((systemState.storage.usedCapacityGb - (reclaimedMb / 1024)) * 10) / 10;

        return sendJson(res, 200, {
            success: true,
            reclaimedSpaceMb: reclaimedMb,
            newFragmentation: `${systemState.storage.fragmentationPercent}%`,
            newFreeGb: systemState.storage.freeCapacityGb,
            message: `Storage optimization complete! Reclaimed ${reclaimedMb} MB via block deduplication & TRIM wear leveling.`
        });
    }

    // ----------------------------------------------------
    // 5. CYBERSECURITY SUITE
    // ----------------------------------------------------
    if (pathname === '/api/cybersec/packets' && req.method === 'GET') {
        const protocols = ['TLS 1.3', 'HTTPS', 'SSH-2.0', 'DNS-SEC', 'TCP SYN', 'ICMP', 'QUIC'];
        const samplePackets = [];
        for (let i = 0; i < 6; i++) {
            const proto = protocols[Math.floor(Math.random() * protocols.length)];
            const isSuspicious = Math.random() < 0.2;
            samplePackets.push({
                id: `pkt-${Math.random().toString(16).substring(2, 8)}`,
                timestamp: new Date().toLocaleTimeString(),
                protocol: proto,
                srcIp: `192.168.1.${Math.floor(Math.random() * 200) + 10}`,
                dstIp: `10.0.0.${Math.floor(Math.random() * 50) + 1}`,
                port: proto.includes('HTTPS') ? 443 : proto.includes('SSH') ? 22 : proto.includes('DNS') ? 53 : 8080,
                status: isSuspicious ? 'FLAGGED_INSPECTION' : 'ALLOWED',
                payloadFlag: isSuspicious ? 'Unusual entropy anomaly / Potential probe' : 'Encrypted application frame'
            });
        }
        return sendJson(res, 200, { packets: samplePackets, blockedCount: systemState.securityStatus.threatsBlocked });
    }

    if (pathname === '/api/cybersec/vulnerability-scan' && req.method === 'POST') {
        return sendJson(res, 200, {
            status: "AUDIT_COMPLETE",
            systemHardenScore: "98/100 (EXTREME DEFENSE)",
            auditPoints: [
                { title: "64-bit ASLR Entropy", status: "PASS", detail: "Stack, heap, mmap, and kernel text fully randomized with high entropy." },
                { title: "Hardware IOMMU / DMA Sandboxing", status: "PASS", detail: "Ring 3 user-space drivers strictly isolated from direct physical DMA." },
                { title: "Shadow Call Stack & Heap Canaries", status: "PASS", detail: "Active return address protection on all microkernel syscall gates." },
                { title: "Cryptographic Merkle Root Invariance", status: systemState.filesystem.status === "INTEGRITY_VERIFIED" ? "PASS" : "WARN", detail: "SHA-256 hash trees verify zero unauthorized file alterations." },
                { title: "Zero-Trust Default Deny Access", status: "PASS", detail: "Every hardware call requires signed, ephemeral capability tokens." },
                { title: "Stateful Packet Inspection (SPI)", status: "PASS", detail: "Port knocking and NOP sled detection active on network tap." }
            ],
            cveList: systemState.securityStatus.vulnerabilities
        });
    }

    if (pathname === '/api/cybersec/sandbox-detonate' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            const sampleName = data.sampleName || "unknown_suspicious_payload.bin";
            const threatScore = Math.floor(Math.random() * 15) + 85;
            return sendJson(res, 200, {
                success: true,
                sampleName,
                sandboxPid: 8492,
                quarantineId: `THR-${Math.floor(Math.random() * 9000) + 1000}`,
                threatConfidence: `${threatScore}%`,
                detectedBehaviors: [
                    "Attempted unauthorized hook on Kernel Syscall Table (Blocked by Ring 0 Gate)",
                    "Attempted rapid traversal of /userland/data/ to encrypt file headers (Ransomware Tripwire Fired)",
                    "Attempted raw socket injection to external IP 185.220.101.5 (Dropped by Aegis Shield)"
                ],
                actionTaken: "AUTOMATICALLY QUARANTINED TO IMMUTABLE MERKLE VAULT",
                systemCompromised: false
            });
        });
        return;
    }

    if (pathname === '/api/cybersec/issue-token' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            const token = {
                tokenId: `TOK-${Math.floor(Math.random() * 9000) + 1000}`,
                subject: data.subject || "aegis_user_app",
                capability: data.capability || "CAP_NETWORK_CLIENT",
                ttl: "300s (5 Minutes)",
                status: "VALID",
                issuedAt: new Date().toLocaleTimeString()
            };
            systemState.securityStatus.zeroTrustTokens.unshift(token);
            return sendJson(res, 200, { success: true, token });
        });
        return;
    }

    // ----------------------------------------------------
    // 6. AGENTIC AI ASSISTANT ("Aegis Mind")
    // ----------------------------------------------------
    if ((pathname === '/api/ai/execute' || pathname === '/api/ai/chat') && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.prompt) return sendJson(res, 400, { error: 'Prompt required' });
            const prompt = data.prompt.trim();
            handleAIAssistant(prompt, (response) => {
                return sendJson(res, 200, response);
            });
        });
        return;
    }

    // ----------------------------------------------------
    // 7. EXISTING AEGIS CONTROLS (Shield, Merkle, Opt, Term)
    // ----------------------------------------------------
    if (pathname === '/api/shield/toggle-rule' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.ruleId) return sendJson(res, 400, { error: 'Invalid payload' });
            const rule = systemState.securityStatus.firewallRules.find(r => r.id === data.ruleId);
            if (!rule) return sendJson(res, 404, { error: 'Rule not found' });
            rule.enabled = !rule.enabled;
            return sendJson(res, 200, { success: true, rule });
        });
        return;
    }

    if (pathname === '/api/guard/scan' && req.method === 'POST') {
        return sendJson(res, 200, {
            scannedProcesses: 68,
            threatsFound: systemState.securityStatus.quarantinedFiles.length,
            accuracy: systemState.securityStatus.guardAccuracy,
            status: "ALL_SYSTEMS_PROTECTED",
            threats: systemState.securityStatus.quarantinedFiles
        });
    }

    if (pathname === '/api/troubleshoot/run' && req.method === 'POST') {
        const diagnosticPoints = [
            { id: 1, name: "Kernel ASLR & Microkernel Page Table Isolation", status: "PASS", message: "Kernel space randomized with 64-bit entropy. No overlapping rings detected." },
            { id: 2, name: "Zero-Trust Driver Sandboxes (Ring 3 Isolation)", status: "PASS", message: "12 registered user-space drivers strictly sandboxed without direct DMA." },
            { id: 3, name: "Merkle Tree Filesystem Hash Consistency", status: systemState.filesystem.status === "INTEGRITY_VERIFIED" ? "PASS" : "FAIL", message: systemState.filesystem.status === "FAIL" ? "Hash mismatch detected in block #4!" : "All Merkle branches match cryptographic root." },
            { id: 4, name: "Dynamic Memory Leak & Double-Free Heuristics", status: "PASS", message: "Heap canary markers intact. 0 unfreed memory chunks older than TTL." },
            { id: 5, name: "Aegis Shield Stateful Packet Inspection & TLS 1.3", status: "PASS", message: "Firewall rules enforced. Zero open debug ports." },
            { id: 6, name: "System Checkpoint & Snapshot Storage State", status: "PASS", message: "3 valid restore points found. Sub-10s rollback ready." },
            { id: 7, name: "CFS Scheduler & Task Optimizer Affinity", status: "PASS", message: `Active Mode: ${systemState.optimizerMode}. Priority threads tuned.` }
        ];
        return sendJson(res, 200, { diagnosticPoints, timestamp: Date.now() });
    }

    if (pathname === '/api/optimizer/set-mode' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.mode) return sendJson(res, 400, { error: 'Invalid mode' });
            systemState.optimizerMode = data.mode.toUpperCase();
            return sendJson(res, 200, { success: true, mode: systemState.optimizerMode });
        });
        return;
    }

    if (pathname === '/api/merkle/corrupt' && req.method === 'POST') {
        const targetBlock = systemState.filesystem.blocks[4];
        targetBlock.status = "CORRUPTED";
        targetBlock.hash = "0xBAD_RANSOMWARE_ENCRYPTED_BLOCK";
        systemState.filesystem.status = "CORRUPTED";
        systemState.filesystem.corruptedBlock = targetBlock.id;
        return sendJson(res, 200, {
            success: true,
            alert: "CRITICAL: Merkle tree cryptographic divergence detected!",
            corruptedBlock: targetBlock,
            expectedHash: "8f4d92a...",
            detectedHash: targetBlock.hash,
            rollbackRecommended: true,
            estimatedRollbackTimeSeconds: 1.8
        });
    }

    if (pathname === '/api/merkle/rollback' && req.method === 'POST') {
        systemState.filesystem.blocks.forEach(b => {
            b.status = "HEALTHY";
            if (b.id === "blk-4") b.hash = "8f4d92a...";
        });
        systemState.filesystem.status = "INTEGRITY_VERIFIED";
        systemState.filesystem.corruptedBlock = null;
        return sendJson(res, 200, {
            success: true,
            restoredCheckpoint: systemState.filesystem.snapshots[systemState.filesystem.snapshots.length - 1],
            recoveryTimeSeconds: 1.42,
            blocksRestored: 1,
            dataLossBytes: 0,
            merkleRoot: systemState.filesystem.merkleRoot,
            message: "Atomic sub-10s rollback completed in 1.42s. Zero data loss."
        });
    }

    if (pathname === '/api/terminal/exec' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.command) return sendJson(res, 400, { error: 'Command missing' });
            executeShellCommand(data.command.trim(), (output) => {
                return sendJson(res, 200, { output });
            });
        });
        return;
    }

    sendJson(res, 404, { error: `Endpoint not found: ${pathname}` });
}

// ==========================================
// AGENTIC AI ASSISTANT REASONING
// ==========================================
function handleAIAssistant(prompt, callback) {
    const p = prompt.toLowerCase();
    
    if (p.includes('storage') || p.includes('dedup') || p.includes('trim') || p.includes('clean disk')) {
        const reclaimed = Math.floor(Math.random() * 800) + 400;
        systemState.storage.dedupSavingsGb += Math.round((reclaimed / 1024) * 10) / 10;
        systemState.storage.fragmentationPercent = Math.max(0.4, Math.round((systemState.storage.fragmentationPercent * 0.4) * 10) / 10);
        
        return callback({
            role: "assistant",
            intent: "STORAGE_OPTIMIZATION",
            reply: `Tiered storage analyzed and optimized.\n1. Ran real-time deduplication pass across NVMe & SATA SSD. Reclaimed ${reclaimed} MB.\n2. Dispatched I/O TRIM commands to SSD controller.\n3. Compacted kernel memory pages. Fragmentation down to ${systemState.storage.fragmentationPercent}%.`,
            actionsExecuted: [
                { task: "Block Deduplication Sweep", result: `Reclaimed ${reclaimed} MB` },
                { task: "I/O TRIM Wear-Leveling", result: "Complete" },
                { task: "Tier 1 Page Compaction", result: "Optimal" }
            ],
            suggestedFollowups: ["Show storage tiers", "Encrypt all user files", "Run system diagnostics"]
        });
    }

    if (p.includes('browser') || p.includes('web') || p.includes('ghost') || p.includes('internet')) {
        return callback({
            role: "assistant",
            intent: "BROWSER_SPAWN",
            reply: `Aegis Quantum Ghost Browser is active.\nEvery browsing session is completely isolated in an ephemeral RAM container. Once you close the browser window, the container is destroyed with a 0x00 cryptographic shred so no cookies, malware, or trackers touch your host system.`,
            actionsExecuted: [
                { task: "Spawn Ephemeral Container", result: "RAM-Isolated (512 MB sandbox)" },
                { task: "Zero-Trust Airgap", result: "Enforced" }
            ],
            suggestedFollowups: ["Open Quantum Ghost Browser", "Audit vulnerabilities", "Clean storage"]
        });
    }

    if (p.includes('security') || p.includes('vulnerabilit') || p.includes('harden') || p.includes('audit')) {
        return callback({
            role: "assistant",
            intent: "SECURITY_AUDIT",
            reply: `Comprehensive cybersecurity audit complete. Defense rating: 98/100 (EXTREME DEFENSE).\n- Aegis Shield: Stateful Packet Inspection active, 22 attacks dropped.\n- Aegis Guard AI: 98.7% heuristic accuracy with zero-day honeypots.\n- Microkernel Isolation: 64-bit ASLR entropy and Ring-3 driver sandboxes verified.`,
            actionsExecuted: [
                { task: "Audit ASLR & Shadow Stack Canaries", result: "PASS (64-bit entropy verified)" },
                { task: "Evaluate Firewall Rules", result: "6 rules enforced, 0 open debug ports" },
                { task: "Verify Zero-Trust Microkernel Gates", result: "Default-Deny strictly enforced" }
            ],
            suggestedFollowups: ["Detonate sample in malware sandbox", "Sniff network packets", "Switch to game mode"]
        });
    }

    if (p.includes('ransomware') || p.includes('attack') || p.includes('corrupt') || p.includes('heal') || p.includes('rollback')) {
        if (p.includes('rollback') || p.includes('heal') || p.includes('repair')) {
            systemState.filesystem.blocks.forEach(b => b.status = "HEALTHY");
            systemState.filesystem.status = "INTEGRITY_VERIFIED";
            return callback({
                role: "assistant",
                intent: "RESTORE_MERKLE",
                reply: `Atomic Sub-10s Rollback completed in 1.42 seconds! The Merkle root inode has been re-pointed to clean restore point 'chkpt-0003'. All files are healthy with 0 data loss.`,
                actionsExecuted: [
                    { task: "Lookup Snapshot Checkpoint", result: "chkpt-0003 verified" },
                    { task: "Atomic Root Pointer Pivot", result: "1.42s execution time" },
                    { task: "SHA-256 Hash Verification", result: "100% integrity match" }
                ],
                suggestedFollowups: ["Check filesystem status", "Optimize storage", "Audit vulnerabilities"]
            });
        } else {
            systemState.filesystem.blocks[4].status = "CORRUPTED";
            systemState.filesystem.status = "CORRUPTED";
            return callback({
                role: "assistant",
                intent: "CORRUPT_MERKLE_ALERT",
                reply: `ALERT: Ransomware attack simulated on block '/userland/data/user_vault.dat'. Aegis Guard caught the cryptographic divergence immediately.\nWould you like me to execute an atomic rollback?`,
                actionsExecuted: [
                    { task: "Simulate Ransomware Payload Injection", result: "Hash diverged on blk-4" },
                    { task: "Aegis Guard Detection Tripwire", result: "ALARM TRIGGERED (Confidence: 99.2%)" }
                ],
                suggestedFollowups: ["Execute atomic rollback", "Run system diagnostics"]
            });
        }
    }

    if (p.includes('game') || p.includes('gaming') || p.includes('boost') || p.includes('fps')) {
        systemState.optimizerMode = "GAME";
        return callback({
            role: "assistant",
            intent: "MODE_CHANGE",
            reply: `Game Mode activated via Task Optimizer!\n- CPU Priority: +25% (4.85 GHz dynamic clock).\n- GPU Pipeline: +30% Overclock boost.\n- Background tasks suspended, audio latency set to 1.2ms.`,
            actionsExecuted: [
                { task: "Set CFS Scheduler Priority", result: "Boost +25%" },
                { task: "GPU Pipeline Clock", result: "+30% Overclock" },
                { task: "Background Tasks", result: "Suspended" }
            ],
            suggestedFollowups: ["Revert to work mode", "Check CPU telemetry", "Run security audit"]
        });
    }

    if (p.includes('correlat') || p.includes('timeline') || p.includes('root cause') || p.includes('incident') || p.includes('campaign')) {
        let correlationDetail = "Timeline correlation: unauthorized service file modification preceded process spawn, followed by outbound beacon to 185.220.101.5:8443.";
        let taskName = "Timeline Forensic Reconstruction";
        if (p.includes('kernel') || p.includes('privilege') || p.includes('token')) {
            correlationDetail = "Kernel audit correlation: component 'kernel_sync_worker' requested unneeded ring-0 capability. Compromised token: TOK-KERNEL-ESCALATED.";
            taskName = "Kernel Privilege Boundary Audit";
        } else if (p.includes('traffic') || p.includes('flood') || p.includes('gateway') || p.includes('route')) {
            correlationDetail = "Traffic telemetry correlation: public API gateway degraded under distributed flood on edge-route-01. Resilient route: edge-route-02.";
            taskName = "Network Route Ingress Correlation";
        }
        return callback({
            role: "assistant",
            intent: "INCIDENT_CORRELATION",
            reply: 'Aegis Mind Incident Correlation Engine:\n' + correlationDetail + '\n\nEvidence recorded in your active operation dossier.',
            actionsExecuted: [
                { task: taskName, result: "Evidence Verified" },
                { task: "Update Operations HUD Dossier", result: "Complete" }
            ],
            suggestedFollowups: ["Open Operations HUD", "Inspect in Terminal", "Run Aegis Guard scan"]
        });
    }

    return callback({
        role: "assistant",
        intent: "GENERAL_EXECUTION",
        reply: `I am **Aegis Mind**, your autonomous operating system copilot with Ring-0 authority.\n\nTell me any task:\n- *"Optimize tiered storage and clean disk"*\n- *"Open virtual ghost browser for private search"*\n- *"Run full cybersecurity vulnerability audit"*\n- *"Simulate ransomware attack and heal with 10s rollback"*\n- *"Encrypt or edit files in the secure vault"*`,
        actionsExecuted: [
            { task: "Parse Natural Language Intent", result: "Ready" },
            { task: "Check System Health", result: "Normal (CPU: 18%, Security: Optimal)" }
        ],
        suggestedFollowups: [
            "Optimize storage now",
            "Open virtual ghost browser",
            "Run security vulnerability scan",
            "Switch to game mode"
        ]
    });
}

// ==========================================
// REAL OS COMMAND EXECUTOR (SHELL SUBSYSTEM)
// ==========================================
function executeShellCommand(cmd, callback) {
    const parts = cmd.split(' ').filter(Boolean);
    const root = parts[0].toLowerCase();
    const arg1 = parts[1] ? parts[1] : '';
    const arg2 = parts[2] ? parts[2] : '';

    switch (root) {
        case 'help':
            return callback(`AegisOS Full System Command Manual:
  [Filesystem & Editing]
  ls [path]               - List files and directories
  cat <file>              - Read and print file contents
  write <file> <text>     - Write or append text to file
  touch <file>            - Create new empty file
  rm <file>               - Delete file from filesystem
  shred <file>            - Cryptographic multi-pass 0x00 file wipe
  encrypt <file>          - Toggle AES-256 hardware encryption
  
  [System & Telemetry]
  sysinfo                 - OS architecture & kernel status
  uname [-a]              - Print system kernel release
  whoami                  - Current user and privilege ring
  uptime                  - System uptime and load averages
  date                    - Print system timestamp
  top / ps                - Active microkernel task manager
  kill <pid>              - Terminate process by PID
  df -h                   - Storage partition & Merkle disk usage
  free -m                 - Memory allocations (RAM & page cache)
  
  [Network & Security]
  ping <host>             - Send simulated ICMP probe
  curl <url>              - Make HTTP/TLS request in virtual stack
  netstat                 - List active sockets & firewall state
  shield [status|toggle]  - Configure Aegis Shield Firewall
  guard [scan]            - Trigger AI heuristic malware scanner
  cybersec [scan|sniff]   - Run CVE audit or packet sniffer
  
  [Applications & AI]
  browser [url]           - Launch virtual disposable Ghost Browser
  storage [info|opt]      - Inspect tiered storage & run TRIM
  opt [game|work|stream]  - Switch Task Optimizer profile
  checkpoint [list|heal]  - Rollback to cryptographic snapshot
  capability-audit        - Inspect simulated Zero-Trust grants
  verify-containment      - Verify simulated process and network isolation
  verify-root             - Compare Merkle root with trusted checkpoint
  quarantine-component    - Quarantine a simulated training component
  service-health          - Inspect simulated service dependency health
  route-health            - Compare simulated edge route health
  route-select <edge>     - Select a simulated resilient edge route
  ai <instruction>        - Dispatch autonomous task to Aegis Mind
  theme [cyan|amber|dark] - Switch Adaptive UI visual theme
  clear                   - Clear screen`);

        // REAL OS COMMANDS: FILES
        case 'ls':
            const filterPath = arg1.toLowerCase();
            const listFiles = systemState.filesystem.files.filter(f => !filterPath || f.path.toLowerCase().includes(filterPath));
            if (listFiles.length === 0) return callback(`ls: cannot access '${arg1}': No such file or directory`);
            return callback(listFiles.map(f => `${f.perm}  ${f.ring.padEnd(7)}  ${f.sizeStr.padStart(8)}  ${f.encrypted ? '[ENC]' : '[RAW]'}  ${f.path}`).join('\n'));

        case 'cat':
            if (!arg1) return callback(`cat: missing file operand`);
            const targetCat = systemState.filesystem.files.find(f => f.path === arg1 || f.name === arg1 || f.id === arg1);
            if (!targetCat) return callback(`cat: ${arg1}: No such file or directory`);
            return callback(`[FILE: ${targetCat.path} (${targetCat.sizeStr}, ${targetCat.encrypted ? 'AES-256' : 'Plaintext'})]\n${targetCat.content || '(empty file)'}`);

        case 'touch':
            if (!arg1) return callback(`touch: missing file operand`);
            let existingTouch = systemState.filesystem.files.find(f => f.path === arg1 || f.name === arg1);
            if (!existingTouch) {
                const name = path.basename(arg1);
                existingTouch = {
                    id: `f-${Date.now()}`,
                    name: name,
                    path: arg1.startsWith('/') ? arg1 : `/userland/data/${arg1}`,
                    size: 0,
                    sizeStr: "0 KB",
                    perm: "rw-r--r--",
                    ring: "Ring 3",
                    encrypted: false,
                    hash: Math.random().toString(16).substring(2, 10) + "...",
                    modified: new Date().toISOString().substring(0, 16).replace('T', ' '),
                    type: "text",
                    content: ""
                };
                systemState.filesystem.files.push(existingTouch);
                return callback(`Created file: ${existingTouch.path}`);
            }
            existingTouch.modified = new Date().toISOString().substring(0, 16).replace('T', ' ');
            return callback(`Updated timestamp for ${existingTouch.path}`);

        case 'write':
            if (!arg1) return callback(`write: missing file operand. Usage: write <file> <text>`);
            const writeContent = parts.slice(2).join(' ') || '';
            let targetWrite = systemState.filesystem.files.find(f => f.path === arg1 || f.name === arg1);
            if (!targetWrite) {
                const name = path.basename(arg1);
                targetWrite = {
                    id: `f-${Date.now()}`,
                    name: name,
                    path: arg1.startsWith('/') ? arg1 : `/userland/data/${arg1}`,
                    size: writeContent.length,
                    sizeStr: `${Math.max(1, Math.round(writeContent.length / 1024))} KB`,
                    perm: "rw-r--r--",
                    ring: "Ring 3",
                    encrypted: false,
                    hash: Math.random().toString(16).substring(2, 10) + "...",
                    modified: new Date().toISOString().substring(0, 16).replace('T', ' '),
                    type: "text",
                    content: writeContent
                };
                systemState.filesystem.files.push(targetWrite);
                return callback(`Created and wrote ${writeContent.length} bytes to ${targetWrite.path}`);
            }
            targetWrite.content = writeContent;
            targetWrite.size = writeContent.length;
            targetWrite.sizeStr = `${Math.max(1, Math.round(writeContent.length / 1024))} KB`;
            targetWrite.modified = new Date().toISOString().substring(0, 16).replace('T', ' ');
            return callback(`Wrote ${writeContent.length} bytes to ${targetWrite.path}`);

        case 'echo':
            const fullEcho = parts.slice(1).join(' ');
            if (fullEcho.includes('>')) {
                const isAppend = fullEcho.includes('>>');
                const sep = isAppend ? '>>' : '>';
                const echoTokens = fullEcho.split(sep);
                const text = echoTokens[0].trim().replace(/^["']|["']$/g, '');
                const targetFile = echoTokens[1].trim();
                let f = systemState.filesystem.files.find(x => x.path === targetFile || x.name === targetFile);
                if (!f) {
                    const fname = path.basename(targetFile);
                    f = {
                        id: `f-${Date.now()}`,
                        name: fname,
                        path: targetFile.startsWith('/') ? targetFile : `/userland/data/${targetFile}`,
                        size: Buffer.byteLength(text, 'utf8'),
                        sizeStr: `${Math.max(1, Math.round(Buffer.byteLength(text, 'utf8') / 1024))} KB`,
                        perm: "rw-r--r--",
                        ring: "Ring 3",
                        encrypted: false,
                        hash: Math.random().toString(16).substring(2, 10) + "...",
                        modified: new Date().toISOString().substring(0, 16).replace('T', ' '),
                        type: "text",
                        content: text
                    };
                    systemState.filesystem.files.push(f);
                    return callback(`Created and wrote to ${f.path}`);
                } else {
                    f.content = isAppend ? (f.content + '\n' + text) : text;
                    f.size = Buffer.byteLength(f.content, 'utf8');
                    f.sizeStr = `${Math.max(1, Math.round(f.size / 1024))} KB`;
                    f.modified = new Date().toISOString().substring(0, 16).replace('T', ' ');
                    return callback(`Wrote ${f.size} bytes to ${f.path}`);
                }
            }
            return callback(parts.slice(1).join(' ').replace(/^["']|["']$/g, ''));

        case 'pwd':
            return callback(`/userland/data`);

        case 'mkdir':
            if (!arg1) return callback(`mkdir: missing operand`);
            return callback(`Created directory ${arg1.startsWith('/') ? arg1 : '/userland/data/' + arg1}`);

        case 'rm':
            if (!arg1) return callback(`rm: missing operand`);
            const rmIdx = systemState.filesystem.files.findIndex(f => f.path === arg1 || f.name === arg1 || f.id === arg1);
            if (rmIdx === -1) return callback(`rm: cannot remove '${arg1}': No such file`);
            const removed = systemState.filesystem.files.splice(rmIdx, 1)[0];
            return callback(`Removed inode: ${removed.path}`);

        case 'shred':
            if (!arg1) return callback(`shred: missing operand`);
            const shredIdx = systemState.filesystem.files.findIndex(f => f.path === arg1 || f.name === arg1);
            if (shredIdx === -1) return callback(`shred: '${arg1}': No such file`);
            const shredded = systemState.filesystem.files.splice(shredIdx, 1)[0];
            return callback(`shred: ${shredded.path}: Overwritten with 3-pass 0x00 cryptographic zeroes. Inode purged.`);

        case 'encrypt':
            if (!arg1) return callback(`encrypt: missing file operand`);
            const encFile = systemState.filesystem.files.find(f => f.path === arg1 || f.name === arg1);
            if (!encFile) return callback(`encrypt: '${arg1}': No such file`);
            encFile.encrypted = !encFile.encrypted;
            return callback(`AegisFS Encryption toggled for ${encFile.path}: ${encFile.encrypted ? 'LOCKED (AES-256-XTS)' : 'UNLOCKED (Plaintext)'}`);

        // REAL OS COMMANDS: SYSTEM INFO & STATS
        case 'whoami':
            return callback(`root (Authority: Zero-Trust Ring 0 Supervisor, UID: 0, GID: 0)`);

        case 'date':
            return callback(new Date().toString());

        case 'uname':
            return callback(`AegisOS kernel-ring0 2.0.0-Alpha #1 SMP PREEMPT 2026 x86_64 Dual-Kernel/Microkernel GNU/Linux-ABI`);

        case 'uptime':
            const upSec = Math.floor((Date.now() - systemState.startTime) / 1000);
            const hrs = Math.floor(upSec / 3600);
            const mins = Math.floor((upSec % 3600) / 60);
            const secs = upSec % 60;
            return callback(` ${new Date().toLocaleTimeString()} up ${hrs}h ${mins}m ${secs}s, 1 user, load average: 0.18, 0.22, 0.15`);

        case 'top':
            return callback(`AegisOS CFS Scheduler - Tasks: 54 total, 4 running, 50 sleeping
CPU: ${systemState.telemetry.cpuUsage}% usr, 2.1% sys, 0.0% ni, 78.4% id
Mem: 16384 MB total, 5603 MB used, 10781 MB free, 840 MB buff/cache

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
    1 root      20   0   14.2M   8.4M   4.2M S   0.1   0.1   0:02.14 aegis_kernel_init
    8 system    10 -10   48.0M  32.1M  16.0M R   0.8   0.2   0:15.30 shield_daemon
    9 system    10 -10   72.0M  48.5M  24.0M R   1.4   0.3   0:28.12 aegis_guard_ai
   18 system    20   0   96.0M  64.0M  32.0M R   1.8   0.4   0:45.02 aegis_mind_ai
   24 user      20   0  128.0M  85.0M  42.0M R   2.1   0.5   1:12.44 ui_compositor`);

        case 'df':
            return callback(`Filesystem                Size      Used     Avail  Use% Mounted on
/dev/aegis_merkle0       512G      142G      370G   28% /
/dev/aegis_ramdisk        16G      5.6G     10.4G   35% /tmp/ramfs
/dev/aegis_sata0        2048G      637G     1411G   31% /vault/archive
/dev/aegis_ghost_vm      512M       48M      464M   10% /var/run/ghost_browser`);

        case 'free':
            return callback(`               total        used        free      shared  buff/cache   available
Mem:           16384        5603        9941         840         840       10781
Swap:           8192           0        8192`);

        case 'netstat':
            return callback(`Active Internet Connections (servers and established)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name
tcp        0      0 127.0.0.1:3000          0.0.0.0:*               LISTEN      1/aegis_server
tcp        0      0 10.0.0.1:22             0.0.0.0:*               LISTEN      8/shield_daemon
tcp        0      0 10.0.0.1:443            0.0.0.0:*               LISTEN      8/shield_daemon
udp        0      0 127.0.0.1:53            0.0.0.0:*                           12/dns_proxy`);

        case 'ping':
            const host = arg1 || '127.0.0.1';
            return callback(`PING ${host} (${host}) 56(84) bytes of data.
64 bytes from ${host}: icmp_seq=1 ttl=64 time=0.042 ms
64 bytes from ${host}: icmp_seq=2 ttl=64 time=0.038 ms
64 bytes from ${host}: icmp_seq=3 ttl=64 time=0.045 ms
--- ${host} ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2004ms
rtt min/avg/max/mdev = 0.038/0.041/0.045/0.003 ms`);

        case 'curl':
            const targetUrl = arg1 || 'https://aegis-os.org';
            return callback(`HTTP/2 200 OK
server: Aegis-Edge-Security/2.0
content-type: text/html; charset=UTF-8
strict-transport-security: max-age=31536000; includeSubDomains; preload
content-security-policy: default-src 'self'; sandbox allow-scripts

<!DOCTYPE html><html><head><title>AegisOS Virtual Network Stack</title></head>
<body><h1>Connected via Aegis Shield Zero-Trust Gateway</h1></body></html>`);

        case 'browser':
            return callback(`Launched Virtual Ghost Browser container. Session is RAM-isolated. Close window to shred.`);

        // EXISTING HANDLERS
        case 'storage':
            if (arg1 === 'optimize' || arg1 === 'trim') {
                systemState.storage.dedupSavingsGb += 0.6;
                systemState.storage.fragmentationPercent = 1.2;
                return callback(`[Tiered Storage Optimizer] Defrag & TRIM complete! Reclaimed 614 MB of duplicate blocks. Fragmentation down to 1.2%.`);
            }
            return callback(`AegisOS Tiered Storage Engine:
  Total Capacity: ${systemState.storage.totalCapacityGb} GB (Used: ${systemState.storage.usedCapacityGb} GB, Free: ${systemState.storage.freeCapacityGb} GB)
  Block Deduplication: ${systemState.storage.dedupSavingsGb} GB saved (${systemState.storage.dedupRatio})
  Compression: ${systemState.storage.compressionSavingsGb} GB saved (${systemState.storage.compressionRatio})
  Tier 1 (L1 RAM Buffer): 16 GB | Latency: 0.04 us
  Tier 2 (L2 NVMe SSD):   512 GB | Latency: 22.0 us
  Tier 3 (L3 SATA SSD):   2048 GB | Latency: 140.0 us`);

        case 'cybersec':
            if (arg1 === 'sniff') {
                return callback(`[Deep Packet Inspector] Sniffing live frames...
  TLS 1.3 frame from 192.168.1.42:443 -> PASS (Encrypted payload)
  TCP SYN probe on port 23 -> BLOCKED (Telnet rule enforced)
  SSH-2.0 frame on port 22 -> PASS (Verified key handshake)`);
            }
            return callback(`Aegis Cybersecurity Status:
  Overall Security Rating: 98/100 (EXTREME DEFENSE)
  Vulnerabilities: 0 Critical, 0 High, 3 Patched
  ASLR: 64-bit Entropy Verified | Shadow Stack: Active | Microkernel Sandbox: Enforced`);

        case 'ai':
            const promptText = parts.slice(1).join(' ') || 'help';
            handleAIAssistant(promptText, (res) => {
                return callback(`[Aegis Mind AI Response]\n${res.reply}`);
            });
            return;

        case 'sysinfo':
            return callback(`========================================================================
   *   AEGIS OS - HYBRID SECURE INTELLIGENCE OPERATING SYSTEM   *
   *   Kernel v2.0-Alpha | Dual-Kernel Hybrid | AI Self-Healing  *
========================================================================
Architecture: x86_64 Microkernel + Linux ABI Emulation
Active CPU Cores: 8 (Hyperthreaded) | RAM: 16384 MB (ASLR Enabled)
Scheduler: Completely Fair Scheduler (CFS) with Adaptive Mode
Filesystem: AegisFS (Cryptographic Merkle Tree Integrity)
Storage: Tiered Storage Engine (L1 RAM -> L2 NVMe -> L3 SATA)
Security: Aegis Shield (IDS/IPS) + Aegis Guard AI + Zero-Trust Engine
Virtual Browser: RAM-Isolated Disposable Container Engine Active`);

        case 'shield':
            return callback(`Aegis Shield Firewall & IDS/IPS:
Status: ACTIVE [Stateful Packet Inspection: ON | NOP Sled Filter: ON]
Blocked Inbound Attacks: ${systemState.securityStatus.threatsBlocked}
Firewall Rules: ${systemState.securityStatus.firewallRules.length} rules active.`);

        case 'guard':
            return callback(`Aegis Guard AI Heuristic Detection Engine:
ML Model Confidence: ${systemState.securityStatus.guardAccuracy}%
Active Scanners: Ransomware Canary, Privilege Escalation Tracker, Zero-Day Trap
Quarantined Threats: ${systemState.securityStatus.quarantinedFiles.length} files secured in vault.`);

        case 'opt':
            if (arg1 === 'game') {
                systemState.optimizerMode = 'GAME';
                return callback(`[Task Optimizer] Game Mode Activated! CPU Boost: +25% | GPU Boost: +30% | Background Tasks: Paused.`);
            } else if (arg1 === 'stream' || arg1 === 'streaming') {
                systemState.optimizerMode = 'STREAMING';
                return callback(`[Task Optimizer] Streaming Mode Activated! Network QoS Prioritized.`);
            } else if (arg1 === 'eco') {
                systemState.optimizerMode = 'ECO';
                return callback(`[Task Optimizer] Eco Mode Activated! Power draw reduced to 9W.`);
            } else {
                systemState.optimizerMode = 'WORK';
                return callback(`[Task Optimizer] Work Mode Activated! Balanced multitasking enabled.`);
            }

        case 'checkpoint':
            if (arg1 === 'heal' || arg1 === 'rollback') {
                systemState.filesystem.blocks.forEach(b => b.status = "HEALTHY");
                systemState.filesystem.status = "INTEGRITY_VERIFIED";
                return callback(`[Merkle Self-Healing] Checkpoint restored in 1.42s! All cryptographic hashes verified.`);
            }
            return callback(`System Checkpoints:
  1. chkpt-0001 (Clean Boot Baseline) - Size: 482 MB [BOOTABLE]
  2. chkpt-0002 (Pre-Update Snapshot)  - Size: 512 MB [BOOTABLE]
  3. chkpt-0003 (Alpha Restore Point)   - Size: 515 MB [BOOTABLE]
  Estimated Sub-10s Rollback Time: 1.8 seconds.`);

        case 'troubleshoot':
            return callback(`Running 7-Point Diagnostic Scan...
[PASS] Kernel ASLR & Microkernel Page Table Isolation
[PASS] Zero-Trust Driver Sandboxes
[PASS] Merkle Tree Filesystem Hash Consistency
[PASS] Dynamic Memory Leak & Double-Free Heuristics
[PASS] Aegis Shield Stateful Packet Inspection
[PASS] System Checkpoint Storage State
[PASS] CFS Scheduler & Task Optimizer Affinity
Result: ALL 7 DIAGNOSTIC CHECKS PASSED.`);

        // Fictional incident-response commands used by the Aegis training campaign.
        // These return simulated evidence only; they never execute host commands.
        
        case 'carve-file':
        case 'recover-file': {
            const fileName = arg1 || 'financial_audit.db.corrupt';
            const hexOrKey = parts.slice(2).join(' ') || (fileName.includes('symbol') ? '0xAE915_KRN_SEC' : '41 45 47 49');
            const list = systemState.filesystem.corruptedFiles || [];
            const target = list.find(f => f.name === fileName || f.id === fileName);
            if (!target) return callback(`Error: Corrupted file '${fileName}' not found in filesystem. Use 'inspect-hex' to list files.`);
            target.status = "RECOVERED";
            return callback(`[AEGIS FS CARVER] File '${target.originalName}' carved and restored!\n[PASS] Reconstructed inode header at offset ${target.corruptOffset}\n[PASS] Merkle block verified clean.\nFile available at ${target.recoveredPath}`);
        }

        case 'inspect-hex': {
            const list = systemState.filesystem.corruptedFiles || [];
            if (!arg1) {
                const summary = list.map(f => `  - ${f.name} [${f.damageType}] (Status: ${f.status})`).join('\n');
                return callback(`CORRUPTED INODE CANDIDATES FOR CARVING:\n${summary}\n\nUsage: inspect-hex <filename>`);
            }
            const target = list.find(f => f.name === arg1 || f.id === arg1);
            if (!target) return callback(`Error: File '${arg1}' not found in corruption manifest.`);
            return callback(`HEX DUMP & CORRUPTION ANALYSIS: ${target.name}\nOffset:   ${target.corruptOffset}\nHex:      ${target.rawHex}\nASCII:    ${target.asciiDump}\nDamage:   ${target.damageType}\nExpected: ${target.expectedMagic} (${target.expectedAscii})\nHint:     ${target.hint}`);
        }

        case 'kill': {
            const pid = parseInt(arg1 || '0', 10);
            if (pid === 4819) {
                const svc = (systemState.services || []).find(s => s.id === 'svc-apigateway');
                if (svc) {
                    svc.roguePid = null;
                    svc.issues = (svc.issues || []).filter(i => !i.includes('hijacked'));
                }
                return callback('[SIGKILL] Process PID 4819 (rogue_c2_proxy) terminated.\nPort 8443 freed successfully. Run \'service-restart aegis-apigateway\' to boot the gateway.');
            }
            return callback(`[SIGKILL] Signal delivered to PID ${pid}.`);
        }

        case 'patch-config': {
            const svc = (systemState.services || []).find(s => s.id === 'svc-edgerouter');
            if (svc) {
                svc.issues = [];
                svc.logs.push(`[${new Date().toLocaleTimeString()}] Patched /etc/aegis_gateway.conf upstream.`);
            }
            return callback('[Aegis Config Patch] /etc/aegis_gateway.conf patched successfully.\nRestored upstream to 127.0.0.1:8443. Rogue external C2 proxy removed.');
        }

        case 'service-restart': {
            const svcName = arg1 || 'aegis-apigateway';
            const svc = (systemState.services || []).find(s => s.name === svcName || s.id === svcName || s.name.includes(svcName));
            if (!svc) return callback(`Error: Service '${svcName}' not recognized.`);
            if (svc.roguePid) return callback(`Error: Port ${svc.port} is still occupied by rogue PID ${svc.roguePid}! Run 'kill ${svc.roguePid}' first.`);
            if (svc.issues && svc.issues.length > 0) return callback(`Error: Unresolved issues: ${svc.issues.join('; ')}. Run 'patch-config' first.`);
            svc.status = "ACTIVE";
            return callback(`[SYSTEMD/AEGIS_INIT] Service '${svc.displayName}' restarted cleanly.\nStatus: ACTIVE (200 OK) on port ${svc.port}.`);
        }

        case 'revoke-token': {
            const tokId = arg1 || 'TOK-KERNEL-ESCALATED';
            const tok = (systemState.tokens || []).find(t => t.id === tokId);
            if (!tok) return callback(`Error: Token '${tokId}' not found.`);
            tok.status = "REVOKED";
            return callback(`[ZERO-TRUST BROKER] Token ${tok.id} (${tok.cap}) REVOKED.\nPrivilege boundary restored for ${tok.subject}.`);
        }

        case 'capability-audit':
            return callback('ZERO-TRUST CAPABILITY AUDIT\\n[VALID] aegis_ui_compositor -> CAP_FRAMEBUFFER_WRITE\\n[VALID] neural_monitor -> CAP_KERNEL_TELEMETRY\\n[ANOMALY] kernel_sync_worker -> CAP_NET_RAW_SOCKET (unexpected grant)\\nFinding: TOK-KERNEL-ESCALATED exceeds the component minimum-authority profile.');
        case 'verify-containment':
            return callback('CONTAINMENT VERIFICATION\\n[PASS] suspicious worker isolated in the simulated process table\\n[PASS] hostile outbound session no longer established\\n[PASS] trusted desktop services remain available\\nResult: containment verified in the training environment.');
        case 'verify-root':
            return callback('MERKLE ROOT VERIFICATION\\n[PASS] checkpoint manifest readable\\n[PASS] affected branch compared against trusted snapshot\\nRoot: ' + (systemState.filesystem.merkleRoot || 'AEGIS-ROOT-VERIFIED') + '\\nResult: simulated integrity verification complete.');
        case 'quarantine-component':
            return callback('COMPONENT QUARANTINE\\n[PASS] kernel_sync_worker moved to isolated training namespace\\n[PASS] privileged capability removed from active session\\n[PASS] audit record sealed\\nNo host process was affected.');
        case 'service-health':
            return callback('SERVICE HEALTH MATRIX\\n[HEALTHY] identity broker 12ms\\n[HEALTHY] DNS resolver 18ms\\n[DEGRADED] public API gateway queue depth elevated\\n[HEALTHY] priority service edge-02 latency 24ms\\nFinding: preserve priority service path while mitigating ingress.');
        case 'route-health':
            return callback('EDGE ROUTE HEALTH\\nedge-route-01: DEGRADED / queue saturation 91%\\nedge-route-02: HEALTHY / queue 28% / priority traffic enabled\\nedge-route-03: HEALTHY / capacity reserve 12%\\nRecommendation: edge-route-02 for critical service traffic.');
        case 'route-select':
            if (arg1 !== 'edge-route-02') return callback('Route rejected: select a route with passing health and priority capacity.');
            return callback('ROUTE CHANGE ACCEPTED\\nCritical service path moved to edge-route-02. Health check passed; priority traffic preserved.');

        case 'neural':
            return callback(`Neural Activity Telemetry:
  CPU Usage:       ${systemState.telemetry.cpuUsage}%
  RAM Usage:       ${systemState.telemetry.ramUsage}% (${systemState.telemetry.usedRamMb} / ${systemState.telemetry.totalRamMb} MB)
  Active Procs:    ${systemState.telemetry.activeProcesses}
  Network Rate:    ${systemState.telemetry.networkKbps} KB/s
  Events Logged:   ${systemState.telemetry.totalEventsLogged}`);

        case 'achievements':
            return callback('AEGIS OPERATOR ACHIEVEMENTS & MEDALS:\n' +
              '  [1]  🛡️ First Line of Defense        - Contain first security breach\n' +
              '  [2]  ⏱️ Crisis Responder             - Neutralize a time-critical attack\n' +
              '  [3]  ⚡ Sub-60s SecOps               - Neutralize incident under 60 seconds\n' +
              '  [4]  🔐 Zero-Trust Sentinel          - Revoke unauthorized capability token\n' +
              '  [5]  🧬 Merkle Restorer              - Perform atomic Merkle rollback\n' +
              '  [6]  📡 Traffic Forensics            - Correlate external C2 beacon relay\n' +
              '  [7]  🧠 Forensic Deduction Master    - Submit 100% correct case report\n' +
              '  [8]  💻 Ring-0 Operator              - Execute 8 security shell commands\n' +
              '  [9]  🌐 Ephemeral Airgap             - Browse via isolated Ghost RAM sandbox\n' +
              '  [10] 📁 Cryptographic Custody        - Encrypt evidence file in AegisFS\n' +
              '  [11] 🏆 Grade S Commander            - Secure operational case with Grade S\n' +
              '  [12] 👑 Cyber Fortress Vanguard      - Complete all 3 cases and repel APT\n\n' +
              'Check active progress in the Case Dossier HUD (Hotkey: F4).');

        case 'drill':
        case 'simulate-attack':
            return callback('[EMERGENCY SEC-OPS DRILL TRIGGERED]\n' +
              'Alarm initialized: Dynamic time-critical incident launched.\n' +
              'Consult Aegis Mind Copilot and deploy defensive countermeasures before expiration!');

        case 'emergency-status':
            return callback('EMERGENCY INCIDENT SYSTEM STATUS:\n' +
              'Real-time intrusion detector: ARMED (Zero-Trust Heuristics)\n' +
              'Automated drill dispatch: READY\n' +
              'Issue \'drill\' or \'simulate-attack\' to trigger a live emergency scenario.');

        default:
            return callback(`Command '${root}' executed in Aegis Shell environment.`);
    }
}

const HOST = process.env.HOST || '127.0.0.1';
if (require.main === module && !process.env.VERCEL) {
    server.listen(PORT, HOST, () => {
    console.log(`========================================================================`);
    console.log(`   *  AEGIS OS WEB SIMULATOR SERVER ONLINE  *`);
    console.log(`========================================================================`);
    console.log(`   Dashboard: http://127.0.0.1:${PORT}`);
    console.log(`   Direct API: http://127.0.0.1:${PORT}/api/system/status`);
    console.log(`========================================================================`);
    });
}

module.exports = server;



