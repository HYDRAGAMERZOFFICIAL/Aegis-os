const fs = require('fs');
const path = require('path');

const serverFile = path.join(__dirname, '..', 'simulator', 'server.js');
let content = fs.readFileSync(serverFile, 'utf8');

// 1. Add campaignMemoryCache declarations
const cacheDeclarations = `
// ==========================================
// HIGH-PERFORMANCE IN-MEMORY CACHE LAYER
// ==========================================
const campaignMemoryCache = new Map(); // safeUserId -> { state, updatedAt }
const cacheWriteTimers = new Map();

function getCachedCampaignState(safeUserId, savePath) {
    if (campaignMemoryCache.has(safeUserId)) {
        return campaignMemoryCache.get(safeUserId);
    }
    try {
        const saved = JSON.parse(fs.readFileSync(savePath, 'utf8'));
        const entry = { state: saved.state || null, updatedAt: saved.updatedAt || null };
        campaignMemoryCache.set(safeUserId, entry);
        return entry;
    } catch (e) {
        if (e.code === 'ENOENT') {
            const entry = { state: null, updatedAt: null };
            campaignMemoryCache.set(safeUserId, entry);
            return entry;
        }
        throw e;
    }
}

function setCachedCampaignState(safeUserId, savePath, state) {
    const entry = { state, updatedAt: Date.now() };
    campaignMemoryCache.set(safeUserId, entry);
    if (cacheWriteTimers.has(safeUserId)) {
        clearTimeout(cacheWriteTimers.get(safeUserId));
    }
    const timer = setTimeout(() => {
        cacheWriteTimers.delete(safeUserId);
        const serialized = JSON.stringify(entry);
        const tempPath = savePath + '.tmp';
        try {
            fs.writeFileSync(tempPath, serialized, { mode: 0o600 });
            fs.renameSync(tempPath, savePath);
        } catch (_) {}
    }, 100);
    cacheWriteTimers.set(safeUserId, timer);
    return entry;
}
`;

if (!content.includes('campaignMemoryCache = new Map()')) {
    content = content.replace("const campaignDataDir = path.join(DATA_DIR, 'campaigns');", `const campaignDataDir = path.join(DATA_DIR, 'campaigns');\n${cacheDeclarations}`);
}

// 2. Replace campaign/state handler with cached version
const oldCampaignHandler = `        if (req.method === 'GET') {
            try {
                const saved = JSON.parse(fs.readFileSync(savePath, 'utf8'));
                return sendJson(res, 200, { state: saved.state || null, updatedAt: saved.updatedAt || null });
            } catch (error) {
                if (error.code === 'ENOENT') return sendJson(res, 200, { state: null, updatedAt: null });
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
                    fs.writeFileSync(tempPath, serialized, { mode: 0o600 });
                    fs.renameSync(tempPath, savePath);
                    return sendJson(res, 200, { success: true, updatedAt: Date.now() });
                } catch (error) {
                    try { if (fs.existsSync(tempPath)) fs.unlinkSync(tempPath); } catch (_) {}
                    return sendJson(res, 500, { error: 'Campaign save could not be written.' });
                }
            });
            return;
        }`;

const newCampaignHandler = `        if (req.method === 'GET') {
            try {
                const entry = getCachedCampaignState(safeUserId, savePath);
                return sendJson(res, 200, { state: entry.state, updatedAt: entry.updatedAt, cached: true });
            } catch (error) {
                return sendJson(res, 500, { error: 'Campaign save could not be read.' });
            }
        }
        if (req.method === 'POST') {
            parseBody(req, (err, data) => {
                if (err || !data || !data.state || data.state.v !== 1 || !Number.isInteger(data.state.index) || data.state.index < 0 || data.state.index > 3 || !Array.isArray(data.state.done) || !Array.isArray(data.state.log)) {
                    return sendJson(res, 400, { error: 'Invalid campaign save payload.' });
                }
                const serialized = JSON.stringify({ state: data.state, updatedAt: Date.now() });
                if (serialized.length > 120000 || data.state.done.length > 4 || data.state.log.length > 250) {
                    return sendJson(res, 413, { error: 'Campaign save exceeds allowed limits.' });
                }
                try {
                    const entry = setCachedCampaignState(safeUserId, savePath, data.state);
                    return sendJson(res, 200, { success: true, updatedAt: entry.updatedAt, cached: true });
                } catch (error) {
                    return sendJson(res, 500, { error: 'Campaign save could not be written.' });
                }
            });
            return;
        }`;

content = content.replace(oldCampaignHandler, newCampaignHandler);

// 3. Add critical breach wipe endpoint /api/system/trigger-wiped
const breachWipeEndpoint = `
    // ----------------------------------------------------
    // CRITICAL BREACH FILE WIPE & RECOVERY PROTOCOL
    // ----------------------------------------------------
    if (pathname === '/api/system/trigger-wiped' && req.method === 'POST') {
        const userFiles = systemState.filesystem.files.filter(f => f.path.startsWith('/userland/data/'));
        userFiles.forEach(f => {
            f.content = "[CORRUPTED INODE: DATA PURGED BY CRITICAL ZERO-DAY BREACH]\\nRestore this file using the File Recovery & Hex Carver in Files & Storage.";
            f.size = Buffer.byteLength(f.content, 'utf8');
            f.sizeStr = \`\${Math.max(1, Math.round(f.size / 1024))} KB\`;
            f.hash = "0xBAD_WIPED_" + Math.random().toString(16).substring(2, 6);
        });

        const list = systemState.filesystem.corruptedFiles || [];
        const vaultCorrupt = list.find(f => f.name === 'user_vault.dat.corrupt');
        if (!vaultCorrupt) {
            list.push({
                id: \`corrupt-wiped-\${Date.now()}\`,
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
                cleanContent: "CLASSIFIED USER VAULT\\nUser: Root Administrator\\nAuth Method: Biometric + Hardware Token\\nStatus: Restored and Verified Clean."
            });
        }
        systemState.filesystem.status = "CORRUPTED";
        return sendJson(res, 200, {
            success: true,
            message: "Critical breach executed: local files wiped and moved to corrupted manifest. Recovery protocol initiated.",
            wipedCount: userFiles.length
        });
    }
`;

if (!content.includes('/api/system/trigger-wiped')) {
    content = content.replace("if (pathname === '/api/files/corrupted-list'", `${breachWipeEndpoint}\n    if (pathname === '/api/files/corrupted-list'`);
}

fs.writeFileSync(serverFile, content, 'utf8');
console.log('Successfully added in-memory cache and breach wipe endpoint to server.js');

