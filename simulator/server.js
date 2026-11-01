/**
 * AegisOS Desktop Environment & System Simulator Server
 * Pure Node.js implementation - Zero external dependencies required.
 */

const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawn, exec } = require('child_process');

const PORT = process.env.PORT || 3000;
const WEB_DIR = path.join(__dirname, 'web');
const SIM_EXE = path.join(__dirname, '..', 'aegis_sim.exe');

// In-Memory Simulation State
const systemState = {
    kernelVersion: "2.0.0-Alpha",
    architecture: "Dual-Kernel Microkernel (Aegis Core + Linux ABI)",
    startTime: Date.now(),
    layout: "hybrid", // "hybrid" | "windows" | "linux"
    theme: "quantum-cyan",
    optimizerMode: "WORK",
    securityStatus: {
        firewallActive: true,
        idsActive: true,
        guardAccuracy: 98.7,
        zeroTrustPolicy: "DEFAULT_DENY",
        threatsBlocked: 14,
        quarantinedFiles: [
            { id: 'THR-8821', name: 'ransom_injector.bin', type: 'Ransomware Heuristic', threatLevel: 'CRITICAL', time: '10 mins ago', blocked: true },
            { id: 'THR-8822', name: 'mem_escalate_poc', type: 'Privilege Escalation', threatLevel: 'HIGH', time: '25 mins ago', blocked: true }
        ],
        firewallRules: [
            { id: 1, name: "Block Inbound Telnet (23)", proto: "TCP", port: 23, direction: "INBOUND", action: "DENY", enabled: true },
            { id: 2, name: "Allow Secure Shell (SSH)", proto: "TCP", port: 22, direction: "INBOUND", action: "ALLOW", enabled: true },
            { id: 3, name: "Allow HTTPS Web Traffic", proto: "TCP", port: 443, direction: "OUTBOUND", action: "ALLOW", enabled: true },
            { id: 4, name: "Block Outbound Tor Relays", proto: "TCP", port: 9001, direction: "OUTBOUND", action: "DENY", enabled: true },
            { id: 5, name: "Zero-Trust Kernel IPC Gate", proto: "IPC", port: 0, direction: "INTERNAL", action: "ALLOW", enabled: true }
        ]
    },
    filesystem: {
        merkleRoot: "0x8f4d92a15c8e43f07a1b3d5c9e2a7b6f4c8d1e3a5b7c9d0e1f2a3b4c5d6e7f8",
        status: "INTEGRITY_VERIFIED",
        corruptedBlock: null,
        blocks: [
            { id: "blk-0", path: "/boot/vmlinuz-aegis", hash: "a4f912c...", size: "14.2 MB", status: "HEALTHY" },
            { id: "blk-1", path: "/boot/initramfs-aegis.img", hash: "6b2e98d...", size: "28.5 MB", status: "HEALTHY" },
            { id: "blk-2", path: "/system/lib/libaegis_kernel.so", hash: "3e1a77f...", size: "8.4 MB", status: "HEALTHY" },
            { id: "blk-3", path: "/system/security/zero_trust.db", hash: "9d4c21b...", size: "1.2 MB", status: "HEALTHY" },
            { id: "blk-4", path: "/userland/data/user_vault.dat", hash: "8f4d92a...", size: "54.1 MB", status: "HEALTHY" },
            { id: "blk-5", path: "/filesystem/merkle_journal.log", hash: "7c5e31a...", size: "512 KB", status: "HEALTHY" }
        ],
        snapshots: [
            { id: "chkpt-0001", name: "Clean Boot Baseline", timestamp: new Date(Date.now() - 3600000).toLocaleTimeString(), size: "482 MB", bootable: true },
            { id: "chkpt-0002", name: "Pre-Update Snapshot", timestamp: new Date(Date.now() - 1800000).toLocaleTimeString(), size: "512 MB", bootable: true },
            { id: "chkpt-0003", name: "System Restore Point Alpha", timestamp: new Date(Date.now() - 300000).toLocaleTimeString(), size: "515 MB", bootable: true }
        ]
    },
    telemetry: {
        cpuUsage: 18.4,
        ramUsage: 34.2,
        totalRamMb: 16384,
        usedRamMb: 5603,
        networkKbps: 420.5,
        entropyRate: 0.88,
        activeProcesses: 54,
        totalEventsLogged: 48920,
        eventRates: { fileOps: 142, memoryOps: 890, networkPackets: 210, processSpawns: 3 }
    }
};

// MIME Types
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

// Helper to send JSON response
function sendJson(res, statusCode, data) {
    res.writeHead(statusCode, {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
        'Access-Control-Allow-Headers': 'Content-Type'
    });
    res.end(JSON.stringify(data));
}

// Request dispatcher
const server = http.createServer((req, res) => {
    // Handle CORS preflight
    if (req.method === 'OPTIONS') {
        res.writeHead(204, {
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
            'Access-Control-Allow-Headers': 'Content-Type'
        });
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
    let filePath = path.join(WEB_DIR, pathname === '/' ? 'index.html' : pathname);
    filePath = path.normalize(filePath);

    // Prevent directory traversal
    if (!filePath.startsWith(WEB_DIR)) {
        res.writeHead(403, { 'Content-Type': 'text/plain' });
        return res.end('Access Denied');
    }

    fs.stat(filePath, (err, stats) => {
        if (err || !stats.isFile()) {
            // Fallback to index.html for SPA if not an asset request
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

// Parse POST body helper
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

// API Handler
function handleApi(req, res, pathname, url) {
    // 1. System Info & Status
    if (pathname === '/api/system/status' && req.method === 'GET') {
        const uptimeSeconds = Math.floor((Date.now() - systemState.startTime) / 1000);
        return sendJson(res, 200, {
            ...systemState,
            uptimeSeconds,
            nativeSimAvailable: fs.existsSync(SIM_EXE)
        });
    }

    // 2. Neural Activity Telemetry
    if (pathname === '/api/neural/telemetry' && req.method === 'GET') {
        // Generate minor jitter for real-time visualization
        const cpuJitter = (Math.sin(Date.now() / 1500) * 5) + (Math.random() * 4);
        const ramJitter = (Math.cos(Date.now() / 3000) * 2);
        
        let targetCpu = 18;
        if (systemState.optimizerMode === 'GAME') targetCpu = 42;
        else if (systemState.optimizerMode === 'ECO') targetCpu = 10;
        
        systemState.telemetry.cpuUsage = Math.max(5, Math.min(99, Math.round((targetCpu + cpuJitter) * 10) / 10));
        systemState.telemetry.ramUsage = Math.max(15, Math.min(90, Math.round((34 + ramJitter) * 10) / 10));
        systemState.telemetry.totalEventsLogged += Math.floor(Math.random() * 25) + 10;

        return sendJson(res, 200, {
            telemetry: systemState.telemetry,
            timestamp: Date.now()
        });
    }

    // 3. Shield Firewall & IDS
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

    // 4. Run Guard Intrusion Scan
    if (pathname === '/api/guard/scan' && req.method === 'POST') {
        return sendJson(res, 200, {
            scannedProcesses: 68,
            threatsFound: systemState.securityStatus.quarantinedFiles.length,
            accuracy: systemState.securityStatus.guardAccuracy,
            status: "ALL_SYSTEMS_PROTECTED",
            threats: systemState.securityStatus.quarantinedFiles
        });
    }

    // 5. System Troubleshooter 7-Point Scan
    if (pathname === '/api/troubleshoot/run' && req.method === 'POST') {
        setTimeout(() => {
            const diagnosticPoints = [
                { id: 1, name: "Kernel ASLR & Microkernel Page Table Isolation", status: "PASS", message: "Kernel space randomized with 64-bit entropy. No overlapping rings detected." },
                { id: 2, name: "Zero-Trust Driver Sandboxes (Ring 3 Isolation)", status: "PASS", message: "12 registered user-space drivers strictly sandboxed without direct DMA." },
                { id: 3, name: "Merkle Tree Filesystem Hash Consistency", status: systemState.filesystem.status === "HEALTHY" || systemState.filesystem.status === "INTEGRITY_VERIFIED" ? "PASS" : "FAIL", message: systemState.filesystem.status === "FAIL" ? "Hash mismatch detected in block #4!" : "All Merkle branches match cryptographic root." },
                { id: 4, name: "Dynamic Memory Leak & Double-Free Heuristics", status: "PASS", message: "Heap canary markers intact. 0 unfreed memory chunks older than TTL." },
                { id: 5, name: "Aegis Shield Stateful Packet Inspection & TLS 1.3", status: "PASS", message: "Firewall rules enforced. Zero open debug ports." },
                { id: 6, name: "System Checkpoint & Snapshot Storage State", status: "PASS", message: "3 valid restore points found. Sub-10s rollback ready." },
                { id: 7, name: "CFS Scheduler & Task Optimizer Affinity", status: "PASS", message: `Active Mode: ${systemState.optimizerMode}. Priority threads tuned.` }
            ];
            return sendJson(res, 200, { diagnosticPoints, timestamp: Date.now() });
        }, 300);
        return;
    }

    // 6. Task Optimizer Mode Switch
    if (pathname === '/api/optimizer/set-mode' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.mode) return sendJson(res, 400, { error: 'Invalid mode' });
            systemState.optimizerMode = data.mode.toUpperCase();
            return sendJson(res, 200, {
                success: true,
                mode: systemState.optimizerMode,
                profile: {
                    GAME: { cpuBoost: "+25%", gpuBoost: "+30%", backgroundPaused: true, audioLatency: "1.2ms" },
                    WORK: { cpuBoost: "Balanced", gpuBoost: "Standard", backgroundPaused: false, audioLatency: "8.0ms" },
                    STREAMING: { cpuBoost: "+15%", gpuBoost: "+20%", backgroundPaused: false, networkQoS: "High" },
                    ECO: { cpuBoost: "-30%", gpuBoost: "-40%", backgroundPaused: true, powerWattage: "9W" }
                }[systemState.optimizerMode] || {}
            });
        });
        return;
    }

    // 7. Merkle Tree Corruption Simulation (Ransomware Attack)
    if (pathname === '/api/merkle/corrupt' && req.method === 'POST') {
        const targetBlock = systemState.filesystem.blocks[4]; // /userland/data/user_vault.dat
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

    // 8. Atomic Sub-10s Rollback Simulation
    if (pathname === '/api/merkle/rollback' && req.method === 'POST') {
        const startRollback = Date.now();
        // Restore all blocks
        systemState.filesystem.blocks.forEach(b => {
            b.status = "HEALTHY";
            if (b.id === "blk-4") b.hash = "8f4d92a...";
        });
        systemState.filesystem.status = "INTEGRITY_VERIFIED";
        systemState.filesystem.corruptedBlock = null;

        const duration = (Date.now() - startRollback + 1420) / 1000; // Simulated 1.42s atomic recovery

        return sendJson(res, 200, {
            success: true,
            restoredCheckpoint: systemState.filesystem.snapshots[systemState.filesystem.snapshots.length - 1],
            recoveryTimeSeconds: duration,
            blocksRestored: 1,
            dataLossBytes: 0,
            merkleRoot: systemState.filesystem.merkleRoot,
            message: `Atomic sub-10s rollback completed in ${duration.toFixed(2)}s. Zero data loss.`
        });
    }

    // 9. Interactive Aegis Terminal Execution
    if (pathname === '/api/terminal/exec' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data.command) return sendJson(res, 400, { error: 'Command missing' });
            const cmd = data.command.trim();

            executeShellCommand(cmd, (output) => {
                return sendJson(res, 200, { output });
            });
        });
        return;
    }

    // Default 404 for unknown API
    sendJson(res, 404, { error: `Endpoint not found: ${pathname}` });
}

// Shell command executor
function executeShellCommand(cmd, callback) {
    const parts = cmd.split(' ').filter(Boolean);
    const root = parts[0].toLowerCase();
    const arg = parts[1] ? parts[1].toLowerCase() : '';

    switch (root) {
        case 'help':
            return callback(`AegisOS Shell Commands:
  help                    - List available commands
  sysinfo                 - Print system & kernel architecture info
  ps                      - List active processes
  kill <pid>              - Terminate process by PID
  shield [status|toggle]  - Inspect or configure Aegis Shield Firewall
  guard [scan]            - Trigger AI heuristic malware scanner
  ztac                    - Inspect Zero-Trust Access Controller tokens
  troubleshoot            - Run 7-point automated diagnostic suite
  opt [game|work|stream]  - Switch Task Optimizer performance profile
  checkpoint [list|heal]  - Manage snapshots and 10s rollback recovery
  neural                  - Dump real-time neural telemetry counters
  fs [ls|cat|corrupt|heal]- Test Merkle filesystem operations
  theme [cyan|amber|dark] - Switch Adaptive UI visual theme
  layout [hybrid|win|linux]- Switch desktop interface paradigm
  clear                   - Clear terminal display`);

        case 'sysinfo':
            return callback(`========================================================================
   *   AEGIS OS - HYBRID SECURE INTELLIGENCE OPERATING SYSTEM   *
   *   Kernel v2.0-Alpha | Dual-Kernel Hybrid | AI Self-Healing  *
========================================================================
Architecture: x86_64 Microkernel + Linux ABI Emulation
Active CPU Cores: 8 (Hyperthreaded) | RAM: 16384 MB (ASLR Enabled)
Scheduler: Completely Fair Scheduler (CFS) with Adaptive Mode
Filesystem: AegisFS (Cryptographic Merkle Tree Integrity)
Security: Aegis Shield (IDS/IPS) + Aegis Guard AI + Zero-Trust Engine`);

        case 'ps':
            return callback(`PID   PPID  USER      CPU%  MEM(MB)  STATE   NAME
  1      0  root       0.1      8.4  SLEEP   aegis_kernel_init
  2      1  root       0.0      4.2  SLEEP   ksoftirqd/0
  8      1  system     0.8     32.1  RUN     aegis_shield_daemon
  9      1  system     1.4     48.5  RUN     aegis_guard_ai
 12      1  system     0.2     16.0  SLEEP   zero_trust_broker
 24      1  user       2.1     85.0  RUN     aegis_ui_compositor
 45     24  user       1.2     64.2  RUN     neural_activity_monitor
 98     24  user       0.5     22.1  RUN     aegis_shell`);

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
            if (arg === 'game') {
                systemState.optimizerMode = 'GAME';
                return callback(`[Task Optimizer] Game Mode Activated! CPU Boost: +25% | GPU Boost: +30% | Background Tasks: Paused.`);
            } else if (arg === 'stream' || arg === 'streaming') {
                systemState.optimizerMode = 'STREAMING';
                return callback(`[Task Optimizer] Streaming Mode Activated! Network QoS Prioritized.`);
            } else if (arg === 'eco') {
                systemState.optimizerMode = 'ECO';
                return callback(`[Task Optimizer] Eco Mode Activated! Power draw reduced to 9W.`);
            } else {
                systemState.optimizerMode = 'WORK';
                return callback(`[Task Optimizer] Work Mode Activated! Balanced multitasking enabled.`);
            }

        case 'checkpoint':
            if (arg === 'heal' || arg === 'rollback') {
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

        case 'neural':
            return callback(`Neural Activity Telemetry:
  CPU Usage:       ${systemState.telemetry.cpuUsage}%
  RAM Usage:       ${systemState.telemetry.ramUsage}% (${systemState.telemetry.usedRamMb} / ${systemState.telemetry.totalRamMb} MB)
  Active Procs:    ${systemState.telemetry.activeProcesses}
  Network Rate:    ${systemState.telemetry.networkKbps} KB/s
  Events Logged:   ${systemState.telemetry.totalEventsLogged} (Files: ${systemState.telemetry.eventRates.fileOps}/s, Mem: ${systemState.telemetry.eventRates.memoryOps}/s)`);

        case 'fs':
            if (arg === 'corrupt') {
                systemState.filesystem.blocks[4].status = "CORRUPTED";
                systemState.filesystem.status = "CORRUPTED";
                return callback(`[SIMULATION] Injected corruption into block /userland/data/user_vault.dat! Merkle Root diverged.`);
            }
            return callback(`AegisFS Cryptographic Merkle Filesystem:
  Root Hash: ${systemState.filesystem.merkleRoot}
  Status:    ${systemState.filesystem.status}
  Blocks:    ${systemState.filesystem.blocks.length} allocated blocks.`);

        default:
            return callback(`Command '${root}' executed in Aegis Shell environment.`);
    }
}

// Start server on available port
server.listen(PORT, () => {
    console.log(`========================================================================`);
    console.log(`   *  AEGIS OS WEB SIMULATOR SERVER ONLINE  *`);
    console.log(`========================================================================`);
    console.log(`   Dashboard: http://localhost:${PORT}`);
    console.log(`   Direct API: http://localhost:${PORT}/api/system/status`);
    console.log(`========================================================================`);
});
