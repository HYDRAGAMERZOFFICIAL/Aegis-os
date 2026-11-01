'use strict';
const fs = require('fs');
const path = require('path');

const serverPath = path.resolve('simulator/server.js');
let server = fs.readFileSync(serverPath, 'utf8');

const anchor = 'AEGIS GAME API - isolated fictional game state only';
const idx = server.indexOf(anchor);

if (idx !== -1 && !server.includes('/api/user/vault')) {
    // Find the comment block start before idx
    const commentStart = server.lastIndexOf('// ---', idx);
    const before = server.slice(0, commentStart);
    const after = server.slice(commentStart);

    const vaultCode = `    // ----------------------------------------------------
    // AEGIS OPERATOR USER DATABASE & VAULT PERSISTENCE API
    // Stores and restores complete user progress across logins
    // ----------------------------------------------------
    if (pathname === '/api/user/vault' && req.method === 'GET') {
        const session = authManager.getSession(req);
        if (!session || !session.user) return sendJson(res, 401, { error: 'Operator authentication required to access database vault.' });
        
        const userId = session.user.id;
        const safeUserId = String(userId).replace(/[^a-zA-Z0-9_-]/g, '');
        
        let profiles = aegisDB.findBy('user_profiles', 'userId', userId);
        let profile = profiles[0] || null;
        if (!profile) {
            profile = {
                id: 'prf_' + safeUserId,
                userId: userId,
                rank: 'Field Investigator',
                xp: 250,
                credits: 500,
                specialization: 'Tactical Cybersecurity Operations',
                completedMissions: 0,
                sideQuestsDone: 0
            };
            aegisDB.insert('user_profiles', profile);
        }

        let campaign = null;
        if (campaignMemoryCache.has(safeUserId)) {
            campaign = campaignMemoryCache.get(safeUserId).state;
        } else {
            const savePath = path.join(campaignDataDir, safeUserId + '.json');
            try {
                if (fs.existsSync(savePath)) {
                    const saved = JSON.parse(fs.readFileSync(savePath, 'utf8'));
                    campaign = saved.state || null;
                    campaignMemoryCache.set(safeUserId, { state: campaign, updatedAt: saved.updatedAt || Date.now() });
                }
            } catch (_) {}
        }

        let notes = '';
        const caseFiles = aegisDB.findBy('user_case_files', 'userId', userId);
        if (caseFiles.length > 0 && caseFiles[0].content) {
            notes = caseFiles[0].content;
        } else {
            const notesPath = path.join(storageDir, 'userland', 'notes', 'case_notes.txt');
            try {
                if (fs.existsSync(notesPath)) notes = fs.readFileSync(notesPath, 'utf8');
            } catch (_) {}
        }

        const achievements = aegisDB.findBy('user_achievements', 'userId', userId);
        const auditLogs = aegisDB.findBy('user_audit_logs', 'userId', userId).slice(-15);
        const stats = aegisDB.getStats();

        return sendJson(res, 200, {
            success: true,
            status: 'ONLINE',
            dbStats: stats,
            vault: {
                user: {
                    id: session.user.id,
                    username: session.user.username,
                    displayName: session.user.displayName || session.user.username,
                    role: session.user.role || 'SECURITY_OPERATOR',
                    clearanceLevel: session.user.clearanceLevel || 'LEVEL_4_RESTRICTED',
                    createdAt: session.user.createdAt || Date.now()
                },
                profile,
                campaign,
                notes,
                achievements,
                auditLogs: auditLogs.reverse()
            }
        });
    }

    if (pathname === '/api/user/vault/sync' && req.method === 'POST') {
        const session = authManager.getSession(req);
        if (!session || !session.user) return sendJson(res, 401, { error: 'Operator authentication required to sync database vault.' });
        
        parseBody(req, (err, data) => {
            if (err || !data) return sendJson(res, 400, { error: 'Invalid vault payload.' });
            const userId = session.user.id;
            const safeUserId = String(userId).replace(/[^a-zA-Z0-9_-]/g, '');

            if (data.profile) {
                const profiles = aegisDB.findBy('user_profiles', 'userId', userId);
                if (profiles.length > 0) {
                    aegisDB.update('user_profiles', profiles[0].id, {
                        xp: data.profile.xp !== undefined ? data.profile.xp : profiles[0].xp,
                        credits: data.profile.credits !== undefined ? data.profile.credits : profiles[0].credits,
                        rank: data.profile.rank || profiles[0].rank,
                        completedMissions: data.profile.completedMissions !== undefined ? data.profile.completedMissions : profiles[0].completedMissions
                    });
                }
            }

            if (typeof data.notes === 'string') {
                const existingFiles = aegisDB.findBy('user_case_files', 'userId', userId);
                if (existingFiles.length > 0) {
                    aegisDB.update('user_case_files', existingFiles[0].id, {
                        content: data.notes,
                        updatedAt: Date.now()
                    });
                } else {
                    aegisDB.insert('user_case_files', {
                        id: 'cf_' + safeUserId + '_' + Date.now(),
                        userId: userId,
                        fileName: 'case_notes.txt',
                        classification: 'CONFIDENTIAL',
                        content: data.notes,
                        updatedAt: Date.now()
                    });
                }

                const notesDir = path.join(storageDir, 'userland', 'notes');
                try {
                    fs.mkdirSync(notesDir, { recursive: true });
                    fs.writeFileSync(path.join(notesDir, 'case_notes.txt'), data.notes, 'utf8');
                } catch (_) {}
            }

            if (data.campaign && data.campaign.v === 1) {
                campaignMemoryCache.set(safeUserId, { state: data.campaign, updatedAt: Date.now() });
                const savePath = path.join(campaignDataDir, safeUserId + '.json');
                try {
                    fs.writeFileSync(savePath, JSON.stringify({ state: data.campaign, updatedAt: Date.now() }), 'utf8');
                } catch (_) {}
            }

            aegisDB.logAudit(userId, 'VAULT_DATABASE_SYNC', 'Operator progress, notes, and profile synchronized', 'SUCCESS', 'INFO');
            aegisDB._persistSync();

            return sendJson(res, 200, {
                success: true,
                message: 'All operator data securely synchronized to AegisDB in-memory database and persistent storage.',
                syncedAt: Date.now()
            });
        });
        return;
    }\n\n`;

    server = before + vaultCode + after;
    fs.writeFileSync(serverPath, server, 'utf8');
    console.log('Successfully patched server.js with user vault endpoints');
} else {
    console.log('Anchor not found or already patched, idx:', idx);
}
