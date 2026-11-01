const fs = require('fs');
const path = require('path');

const serverFile = path.resolve('simulator/server.js');
let code = fs.readFileSync(serverFile, 'utf8');

// 1. Add AegisDB import and instance
if (!code.includes("require('./db/AegisDB')")) {
    const target = 'const authManager = new AuthManager(aegisDataDir);\r\n';
    const repl = 'const authManager = new AuthManager(aegisDataDir);\r\nconst AegisDB = require(\'./db/AegisDB\');\r\nconst aegisDB = new AegisDB(path.join(aegisDataDir, \'database\'));\r\n';
    if (code.includes(target)) {
        code = code.replace(target, repl);
    } else {
        code = code.replace('const authManager = new AuthManager(aegisDataDir);\n', 'const authManager = new AuthManager(aegisDataDir);\nconst AegisDB = require(\'./db/AegisDB\');\nconst aegisDB = new AegisDB(path.join(aegisDataDir, \'database\'));\n');
    }
}

// 2. Add in-memory staticAssetCache
if (!code.includes('const staticAssetCache = new Map();')) {
    const targetMime = 'const MIME_TYPES = {\r\n';
    const replMime = `const staticAssetCache = new Map();

function getCachedAsset(filePath) {
    try {
        const stats = fs.statSync(filePath);
        if (!stats.isFile()) return null;
        const cached = staticAssetCache.get(filePath);
        if (cached && cached.mtimeMs === stats.mtimeMs) {
            return cached;
        }
        const ext = path.extname(filePath).toLowerCase();
        const contentType = MIME_TYPES[ext] || 'application/octet-stream';
        const content = fs.readFileSync(filePath);
        const etag = 'W/"' + crypto.createHash('md5').update(content).digest('hex') + '"';
        const record = { content, contentType, etag, mtimeMs: stats.mtimeMs };
        staticAssetCache.set(filePath, record);
        return record;
    } catch (_) {
        return null;
    }
}

const MIME_TYPES = {\r\n`;

    if (code.includes(targetMime)) {
        code = code.replace(targetMime, replMime);
    } else {
        code = code.replace('const MIME_TYPES = {\n', replMime.replace(/\r\n/g, '\n'));
    }
}

// 3. Update static file serving to use cache
const oldStaticRegex = /\/\/ Static Web File Serving[\s\S]*?fs\.readFile\(filePath, \(readErr, content\) => \{[\s\S]*?\}\);\s*\}\);\s*\}\);/;
const newStatic = `// Static Web File Serving with In-Memory Cache & ETag acceleration
    let filePath = path.join(WEB_DIR, (pathname === '/' || pathname === '/game' || pathname === '/game.html') ? 'index.html' : pathname);
    filePath = path.normalize(filePath);

    if (!filePath.toLowerCase().startsWith(WEB_DIR.toLowerCase())) {
        res.writeHead(403, { 'Content-Type': 'text/plain' });
        return res.end('Access Denied');
    }

    let asset = getCachedAsset(filePath);
    if (!asset && !path.extname(pathname)) {
        filePath = path.join(WEB_DIR, 'index.html');
        asset = getCachedAsset(filePath);
    }

    if (!asset) {
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        return res.end('404 Not Found');
    }

    const ifNoneMatch = req.headers['if-none-match'];
    if (ifNoneMatch && ifNoneMatch === asset.etag) {
        res.writeHead(304, {
            'ETag': asset.etag,
            'Cache-Control': 'public, max-age=3600, stale-while-revalidate=86400'
        });
        return res.end();
    }

    res.writeHead(200, {
        'Content-Type': asset.contentType,
        'Content-Length': asset.content.length,
        'ETag': asset.etag,
        'Cache-Control': 'public, max-age=3600, stale-while-revalidate=86400'
    });
    res.end(asset.content);
});`;

code = code.replace(oldStaticRegex, newStatic);

// 4. Update campaign save index limit for up to 10 operations
code = code.replace(
    /data\.state\.index > 2/g,
    'data.state.index > 10'
);
code = code.replace(
    /data\.state\.done\.length > 3/g,
    'data.state.done.length > 12'
);

// 5. Add Database API endpoints into handleApi
if (!code.includes('/api/db/tables')) {
    const dbApiEndpoints = `
    // ----------------------------------------------------
    // AEGIS EMBEDDED SYSTEM DATABASE (AegisDB) ENDPOINTS
    // ----------------------------------------------------
    if (pathname === '/api/db/tables' && req.method === 'GET') {
        const stats = aegisDB.getStats();
        return sendJson(res, 200, {
            success: true,
            status: 'ONLINE',
            totalTables: stats.totalTables,
            totalRecords: stats.totalRecords,
            tables: Array.from(aegisDB.tables.keys()).map(name => ({
                name,
                records: aegisDB.tables.get(name).size,
                status: 'SYNCHRONIZED',
                cached: true
            })),
            cache: stats.cache
        });
    }

    if (pathname === '/api/db/table' && req.method === 'GET') {
        const tableName = (url.searchParams.get('name') || '').toLowerCase();
        if (!tableName) return sendJson(res, 400, { error: 'Table name parameter (?name=...) required.' });
        const result = aegisDB.getTable(tableName);
        if (!result) return sendJson(res, 404, { error: \`Table '\${tableName}' not found in AegisDB.\` });
        return sendJson(res, 200, result);
    }

    if (pathname === '/api/db/query' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data || !data.query) return sendJson(res, 400, { error: 'Query string required.' });
            const result = aegisDB.executeQuery(data.query);
            return sendJson(res, 200, result);
        });
        return;
    }

    if (pathname === '/api/db/stats' && req.method === 'GET') {
        return sendJson(res, 200, aegisDB.getStats());
    }

    if (pathname === '/api/db/cache/flush' && req.method === 'POST') {
        const result = aegisDB.executeQuery('FLUSH CACHE');
        return sendJson(res, 200, result);
    }

    if (pathname === '/api/db/side-quest/claim' && req.method === 'POST') {
        parseBody(req, (err, data) => {
            if (err || !data || !data.questId) return sendJson(res, 400, { error: 'Quest ID required.' });
            const session = authManager.getSession(req);
            const quest = aegisDB.findById('user_side_quests', data.questId) || 
                          aegisDB.findBy('user_side_quests', 'questId', data.questId)[0];
            if (!quest) return sendJson(res, 404, { error: 'Side quest not found.' });
            
            const updated = aegisDB.update('user_side_quests', quest.id, {
                status: 'COMPLETED',
                progress: '1/1 Objective Accomplished',
                completedAt: Date.now()
            });

            if (session && session.user) {
                const profiles = aegisDB.findBy('user_profiles', 'userId', session.user.id);
                if (profiles.length > 0) {
                    aegisDB.update('user_profiles', profiles[0].id, {
                        xp: (profiles[0].xp || 0) + (quest.rewardXp || 350),
                        credits: (profiles[0].credits || 0) + (quest.rewardCredits || 250),
                        sideQuestsDone: (profiles[0].sideQuestsDone || 0) + 1
                    });
                }
                aegisDB.logAudit(session.user.id, 'SIDE_QUEST_COMPLETED', quest.questTitle, 'CLAIMED', 'INFO');
            }
            return sendJson(res, 200, {
                success: true,
                message: \`Side quest '\${quest.questTitle}' completed! +\${quest.rewardXp} XP, +\${quest.rewardCredits} Credits awarded.\`,
                quest: updated
            });
        });
        return;
    }
`;
    // Insert before "// ----------------------------------------------------\n    // AEGIS GAME API"
    const insertPoint = "// ----------------------------------------------------\r\n    // AEGIS GAME API";
    if (code.includes(insertPoint)) {
        code = code.replace(insertPoint, dbApiEndpoints + '\r\n    ' + insertPoint);
    } else {
        const insertPointLF = "// ----------------------------------------------------\n    // AEGIS GAME API";
        code = code.replace(insertPointLF, dbApiEndpoints.replace(/\r\n/g, '\n') + '\n    ' + insertPointLF);
    }
}

// 6. Connect User Registration & Login to AegisDB
if (!code.includes('aegisDB.syncUserAccount(result.user);')) {
    code = code.replace(
        'setSessionCookie(res, result.token);\r\n                return sendJson(res, 201, result);',
        'setSessionCookie(res, result.token);\r\n                aegisDB.syncUserAccount(result.user);\r\n                aegisDB.logAudit(result.user.id, \'USER_REGISTER\', \'AUTH_GATEWAY\', \'SUCCESS\');\r\n                return sendJson(res, 201, result);'
    );
}

if (!code.includes('aegisDB.logAudit(result.user.id, \'USER_LOGIN\',')) {
    code = code.replace(
        'setSessionCookie(res, result.token);\r\n                return sendJson(res, 200, result);',
        'setSessionCookie(res, result.token);\r\n                aegisDB.syncUserAccount(result.user);\r\n                aegisDB.logAudit(result.user.id, \'USER_LOGIN\', \'SESSION_GATEWAY\', \'SUCCESS\');\r\n                return sendJson(res, 200, result);'
    );
}

// 7. Add terminal commands for database inspection
if (!code.includes("case 'db':")) {
    const dbTerminalHandler = `
            case 'db': {
                const sub = (parts[1] || 'tables').toLowerCase();
                if (sub === 'tables') {
                    const stats = aegisDB.getStats();
                    const tableRows = Array.from(aegisDB.tables.keys()).map(t => \`  [+] \${t.padEnd(20)} | Records: \${String(aegisDB.tables.get(t).size).padStart(4)} | Status: HEALTHY [IN-RAM]\`).join('\\n');
                    output = \`AegisDB v2.4 In-Memory Hybrid Database Engine\\nTotal Tables: \${stats.totalTables} | Total Records: \${stats.totalRecords} | Cache Hit Ratio: \${stats.cache.hitRatioPercent}%\\n---------------------------------------------------------------\\n\${tableRows}\\n\\nUse 'db query <sql>' or 'db stats' to inspect.\`;
                } else if (sub === 'stats') {
                    const stats = aegisDB.getStats();
                    output = \`AegisDB Performance Telemetry:\\nStatus: \${stats.status}\\nEngine: \${stats.engine}\\nTotal In-Memory Records: \${stats.totalRecords}\\nCache Hits: \${stats.cache.hits} | Misses: \${stats.cache.misses}\\nCache Hit Ratio: \${stats.cache.hitRatioPercent}%\\nAverage Query Latency: \${stats.cache.avgQueryLatencyMs} ms [HIGH-SPEED RAM]\`;
                } else if (sub === 'query') {
                    const q = parts.slice(2).join(' ');
                    if (!q) {
                        output = \`Usage: db query SELECT * FROM <table> [WHERE <col> = '<val>']\`;
                    } else {
                        const res = aegisDB.executeQuery(q);
                        if (!res.success) {
                            output = \`AegisDB Query Error: \${res.error}\`;
                        } else if (res.rows) {
                            const sample = res.rows.slice(0, 5).map(r => JSON.stringify(r)).join('\\n');
                            output = \`[AegisDB Execution Complete: \${res.count} rows in \${res.latencyMs}ms (CACHED)]\\n\${sample}\${res.count > 5 ? \`\\n...and \${res.count - 5} more records.\` : ''}\`;
                        } else {
                            output = JSON.stringify(res, null, 2);
                        }
                    }
                } else if (sub === 'flush') {
                    aegisDB.executeQuery('FLUSH CACHE');
                    output = \`AegisDB in-memory cache flushed and indexed.\`;
                } else {
                    output = \`Unknown db sub-command: '\${sub}'. Valid: db tables, db stats, db query <sql>, db flush\`;
                }
                break;
            }
`;
    code = code.replace(
        "case 'achievements': {",
        dbTerminalHandler + "\r\n            case 'achievements': {"
    );
}

fs.writeFileSync(serverFile, code, 'utf8');
console.log('Successfully patched simulator/server.js with AegisDB and static caching!');

