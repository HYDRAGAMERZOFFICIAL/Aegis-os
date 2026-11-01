'use strict';

const fs = require('fs');
const path = require('path');

class AegisDB {
    constructor(dbDirectory) {
        this.directory = path.resolve(dbDirectory);
        this.dbFilePath = path.join(this.directory, 'aegis_master.db.json');
        this.tables = new Map();
        this.indexes = new Map(); // table -> indexName -> Map(key, Set(ids))
        this.cacheStats = {
            hits: 0,
            misses: 0,
            totalQueries: 0,
            avgLatencyMs: 0.04
        };
        this.isDirty = false;
        this.flushTimer = null;

        fs.mkdirSync(this.directory, { recursive: true });
        this._initTables();
        this._loadFromDisk();
        this._startPeriodicFlush();
    }

    _initTables() {
        const schema = [
            'users',
            'user_profiles',
            'user_sessions',
            'user_case_files',
            'user_achievements',
            'user_audit_logs',
            'user_side_quests',
            'system_cache'
        ];
        schema.forEach(tableName => {
            this.tables.set(tableName, new Map());
            this.indexes.set(tableName, new Map());
        });
    }

    _loadFromDisk() {
        try {
            if (fs.existsSync(this.dbFilePath)) {
                const raw = fs.readFileSync(this.dbFilePath, 'utf8');
                const data = JSON.parse(raw);
                if (data && typeof data === 'object') {
                    for (const [tableName, rows] of Object.entries(data.tables || {})) {
                        if (this.tables.has(tableName) && Array.isArray(rows)) {
                            const tableMap = this.tables.get(tableName);
                            tableMap.clear();
                            rows.forEach(row => {
                                if (row && row.id) {
                                    tableMap.set(String(row.id), row);
                                }
                            });
                        }
                    }
                    this._rebuildIndexes();
                }
            } else {
                this._seedDefaultData();
                this._persistSync();
            }
        } catch (err) {
            console.warn('[AegisDB] Could not load disk snapshot, initializing in-memory tables:', err.message);
            this._seedDefaultData();
        }
    }

    _seedDefaultData() {
        // Default system admin and seed records
        const sysAdmin = {
            id: 'usr_root_001',
            username: 'root',
            displayName: 'Root Administrator',
            role: 'SECURITY_DIRECTOR',
            clearanceLevel: 'LEVEL_5_COSMIC',
            createdAt: Date.now() - 86400000 * 30,
            lastLoginAt: Date.now(),
            status: 'ACTIVE'
        };
        this.tables.get('users').set(sysAdmin.id, sysAdmin);

        const adminProfile = {
            id: 'prf_root_001',
            userId: sysAdmin.id,
            rank: 'Grandmaster Operator',
            xp: 25000,
            credits: 10000,
            specialization: 'Microkernel Defense & Zero-Trust Architecture',
            completedMissions: 6,
            sideQuestsDone: 5
        };
        this.tables.get('user_profiles').set(adminProfile.id, adminProfile);

        // Initial default cases
        const defaultCases = [
            {
                id: 'case_sec_01',
                userId: sysAdmin.id,
                caseId: 'OP-01-SILENT-INTRUSION',
                caseTitle: 'The Silent Intrusion: APT-29 C2 Relay Analysis',
                status: 'RESOLVED',
                priority: 'CRITICAL',
                openedAt: Date.now() - 3600000 * 48,
                closedAt: Date.now() - 3600000 * 12,
                notes: 'Attribution confirmed: Spectre APT. Port 8443 beacon severed. Quarantined infected PID 1337.',
                iocList: ['185.220.101.5:8443', 'SHA256:4f8a2...'],
                evidenceCount: 5
            },
            {
                id: 'case_sec_02',
                userId: sysAdmin.id,
                caseId: 'OP-02-GHOST-KERNEL',
                caseTitle: 'Ghost in the Kernel: Unauthorized Ring-0 Privilege Escalation',
                status: 'ACTIVE',
                priority: 'EMERGENCY',
                openedAt: Date.now() - 3600000 * 6,
                closedAt: null,
                notes: 'Anomalous memory offset 0x7FFF0000 detected. Ingress hook revoking root capability tokens.',
                iocList: ['0x7FFF0000', 'CAP_NET_RAW_SOCKET', 'PID 4819'],
                evidenceCount: 3
            },
            {
                id: 'case_sec_03',
                userId: sysAdmin.id,
                caseId: 'OP-03-BLACKOUT-PROTOCOL',
                caseTitle: 'Blackout Protocol: Coordinated DDoS and DNS Amplification',
                status: 'ACTIVE',
                priority: 'HIGH',
                openedAt: Date.now() - 3600000 * 2,
                closedAt: null,
                notes: 'DDoS flooding edge gateway on port 53/UDP. Synchronizing adaptive rate limit to 750 req/s.',
                iocList: ['198.51.100.22', 'DNS-SEC Amplification'],
                evidenceCount: 2
            }
        ];
        defaultCases.forEach(c => this.tables.get('user_case_files').set(c.id, c));

        // Default Side Quests
        const defaultSideQuests = [
            {
                id: 'sq_01',
                userId: sysAdmin.id,
                questId: 'quest_airgap',
                questTitle: 'Ghost in the Air-Gap: Steganography Analysis',
                category: 'FORENSICS',
                difficulty: 'HARD',
                rewardXp: 450,
                rewardCredits: 300,
                status: 'AVAILABLE',
                description: 'Recover hidden exfiltration payload concealed in air-gapped system memory block 0xDEADBEEF.',
                progress: '0/1 Payload Carved'
            },
            {
                id: 'sq_02',
                userId: sysAdmin.id,
                questId: 'quest_crypto',
                questTitle: 'Crypto-Heist Intercept: Rogue Token Revocation',
                category: 'ZERO_TRUST',
                difficulty: 'MEDIUM',
                rewardXp: 350,
                rewardCredits: 200,
                status: 'AVAILABLE',
                description: 'Audit live capability tokens in Aegis Guard and purge unauthorized privilege grants.',
                progress: '0/1 Tokens Audited'
            },
            {
                id: 'sq_03',
                userId: sysAdmin.id,
                questId: 'quest_dns',
                questTitle: 'DNS Poisoning Triage: Cache Flush & Sinkhole',
                category: 'NETWORK',
                difficulty: 'MEDIUM',
                rewardXp: 300,
                rewardCredits: 250,
                status: 'AVAILABLE',
                description: 'Detect tainted resolver records on port 53, flush the resolver cache, and deploy a sinkhole rule.',
                progress: '0/1 Sockets Secured'
            },
            {
                id: 'sq_04',
                userId: sysAdmin.id,
                questId: 'quest_insider',
                questTitle: 'Insider Threat Audit: Midnight Escalation Hunt',
                category: 'AUDIT',
                difficulty: 'HARD',
                rewardXp: 500,
                rewardCredits: 350,
                status: 'AVAILABLE',
                description: 'Correlate AegisDB user audit logs to expose anomalous credential escalation occurring at 03:00 UTC.',
                progress: '0/1 Log Signatures Verified'
            },
            {
                id: 'sq_05',
                userId: sysAdmin.id,
                questId: 'quest_rootkit',
                questTitle: 'Firmware Rootkit Purge: Merkle Root Restoration',
                category: 'KERNEL',
                difficulty: 'EXTREME',
                rewardXp: 600,
                rewardCredits: 400,
                status: 'AVAILABLE',
                description: 'Perform microkernel ASLR diagnostic and execute sub-10s Merkle atomic rollback on corrupted blocks.',
                progress: '0/1 Inodes Healed'
            }
        ];
        defaultSideQuests.forEach(sq => this.tables.get('user_side_quests').set(sq.id, sq));

        // Default initial audit logs
        const defaultLogs = [
            {
                id: 'log_001',
                userId: sysAdmin.id,
                action: 'SYSTEM_BOOT',
                targetResource: 'KERNEL_VFS',
                result: 'SUCCESS',
                severity: 'INFO',
                timestamp: Date.now() - 3600000 * 10
            },
            {
                id: 'log_002',
                userId: sysAdmin.id,
                action: 'CAPABILITY_AUDIT',
                targetResource: 'TOKEN_REGISTRY',
                result: 'WARNING',
                severity: 'HIGH',
                timestamp: Date.now() - 3600000 * 5
            },
            {
                id: 'log_003',
                userId: sysAdmin.id,
                action: 'ZERO_TRUST_DEFENSE',
                targetResource: 'FIREWALL_RULE_99',
                result: 'BLOCKED',
                severity: 'CRITICAL',
                timestamp: Date.now() - 3600000 * 2
            }
        ];
        defaultLogs.forEach(l => this.tables.get('user_audit_logs').set(l.id, l));

        this._rebuildIndexes();
    }

    _rebuildIndexes() {
        this.tables.forEach((map, tableName) => {
            const tableIndexes = this.indexes.get(tableName);
            tableIndexes.clear();

            map.forEach((row, id) => {
                // Common index columns: userId, username, status, caseId
                for (const col of ['userId', 'username', 'status', 'caseId', 'action']) {
                    if (row[col] !== undefined) {
                        if (!tableIndexes.has(col)) tableIndexes.set(col, new Map());
                        const colIndex = tableIndexes.get(col);
                        const valKey = String(row[col]);
                        if (!colIndex.has(valKey)) colIndex.set(valKey, new Set());
                        colIndex.get(valKey).add(id);
                    }
                }
            });
        });
    }

    _startPeriodicFlush() {
        if (this.flushTimer) clearInterval(this.flushTimer);
        this.flushTimer = setInterval(() => {
            if (this.isDirty) {
                this._persistSync();
            }
        }, 3000);
    }

    _persistSync() {
        try {
            const exportData = {
                version: 1,
                lastPersisted: Date.now(),
                tables: {}
            };
            this.tables.forEach((map, tableName) => {
                exportData.tables[tableName] = Array.from(map.values());
            });
            const tempFile = this.dbFilePath + '.tmp';
            fs.writeFileSync(tempFile, JSON.stringify(exportData, null, 2), { encoding: 'utf8', mode: 0o600 });
            fs.renameSync(tempFile, this.dbFilePath);
            this.isDirty = false;
        } catch (err) {
            console.error('[AegisDB] Persistence failed:', err.message);
        }
    }

    // ----------------------------------------------------
    // Public CRUD & Query Methods with In-Memory Acceleration
    // ----------------------------------------------------

    getTable(tableName) {
        const start = process.hrtime.bigint();
        this.cacheStats.totalQueries++;
        if (!this.tables.has(tableName)) {
            this.cacheStats.misses++;
            return null;
        }
        this.cacheStats.hits++;
        const rows = Array.from(this.tables.get(tableName).values());
        const elapsedMs = Number(process.hrtime.bigint() - start) / 1e6;
        this.cacheStats.avgLatencyMs = (this.cacheStats.avgLatencyMs * 0.95) + (elapsedMs * 0.05);
        return {
            table: tableName,
            count: rows.length,
            rows,
            latencyMs: Math.round(elapsedMs * 1000) / 1000,
            cached: true
        };
    }

    findById(tableName, id) {
        this.cacheStats.totalQueries++;
        if (!this.tables.has(tableName)) return null;
        const row = this.tables.get(tableName).get(String(id));
        if (row) {
            this.cacheStats.hits++;
            return row;
        }
        this.cacheStats.misses++;
        return null;
    }

    findBy(tableName, column, value) {
        const start = process.hrtime.bigint();
        this.cacheStats.totalQueries++;
        if (!this.tables.has(tableName)) return [];
        const tableIndexes = this.indexes.get(tableName);
        const colIndex = tableIndexes && tableIndexes.get(column);
        if (colIndex && colIndex.has(String(value))) {
            this.cacheStats.hits++;
            const ids = colIndex.get(String(value));
            const table = this.tables.get(tableName);
            const res = [];
            ids.forEach(id => {
                const r = table.get(id);
                if (r) res.push(r);
            });
            return res;
        }
        // Fallback scan
        const rows = [];
        this.tables.get(tableName).forEach(row => {
            if (row[column] === value || String(row[column]) === String(value)) {
                rows.push(row);
            }
        });
        if (rows.length > 0) this.cacheStats.hits++; else this.cacheStats.misses++;
        return rows;
    }

    insert(tableName, row) {
        if (!this.tables.has(tableName)) {
            this.tables.set(tableName, new Map());
            this.indexes.set(tableName, new Map());
        }
        if (!row.id) {
            row.id = `${tableName.substring(0, 3)}_${Date.now()}_${Math.floor(Math.random() * 9000 + 1000)}`;
        }
        if (!row.createdAt && !row.timestamp) {
            row.createdAt = Date.now();
        }
        this.tables.get(tableName).set(String(row.id), row);
        this.isDirty = true;
        this._rebuildIndexes();
        return row;
    }

    update(tableName, id, updates) {
        if (!this.tables.has(tableName)) return null;
        const table = this.tables.get(tableName);
        const existing = table.get(String(id));
        if (!existing) return null;
        const updated = Object.assign({}, existing, updates, { updatedAt: Date.now() });
        table.set(String(id), updated);
        this.isDirty = true;
        this._rebuildIndexes();
        return updated;
    }

    delete(tableName, id) {
        if (!this.tables.has(tableName)) return false;
        const deleted = this.tables.get(tableName).delete(String(id));
        if (deleted) {
            this.isDirty = true;
            this._rebuildIndexes();
        }
        return deleted;
    }

    // ----------------------------------------------------
    // User Sync & Integration Helpers
    // ----------------------------------------------------

    syncUserAccount(account) {
        if (!account || !account.id) return;
        const existing = this.findById('users', account.id);
        if (existing) {
            this.update('users', account.id, {
                username: account.username,
                displayName: account.displayName || existing.displayName,
                lastLoginAt: Date.now()
            });
        } else {
            this.insert('users', {
                id: account.id,
                username: account.username,
                displayName: account.displayName || account.username,
                role: 'CYBER_DEFENSE_OPERATOR',
                clearanceLevel: 'LEVEL_3_SECRET',
                createdAt: account.createdAt || Date.now(),
                lastLoginAt: Date.now(),
                status: 'ACTIVE'
            });
            this.insert('user_profiles', {
                id: 'prf_' + account.id,
                userId: account.id,
                rank: 'Junior Operator',
                xp: 100,
                credits: 50,
                specialization: 'Defensive Triage & Log Forensics',
                completedMissions: 0,
                sideQuestsDone: 0
            });
        }
    }

    logAudit(userId, action, targetResource, result = 'SUCCESS', severity = 'INFO') {
        return this.insert('user_audit_logs', {
            id: `audit_${Date.now()}_${Math.floor(Math.random() * 9000 + 1000)}`,
            userId: userId || 'system',
            action,
            targetResource,
            result,
            severity,
            timestamp: Date.now()
        });
    }

    // ----------------------------------------------------
    // AegisQL Query Execution
    // ----------------------------------------------------

    executeQuery(queryString) {
        const start = process.hrtime.bigint();
        this.cacheStats.totalQueries++;
        const q = String(queryString || '').trim();

        if (/^CACHE\s+STATS$/i.test(q)) {
            this.cacheStats.hits++;
            return {
                query: q,
                success: true,
                type: 'COMMAND',
                data: this.getStats(),
                latencyMs: 0.02
            };
        }

        if (/^FLUSH\s+CACHE$/i.test(q)) {
            this.cacheStats.hits = 0;
            this.cacheStats.misses = 0;
            return {
                query: q,
                success: true,
                type: 'COMMAND',
                message: 'AegisDB in-memory cache flushed and re-indexed.',
                latencyMs: 0.05
            };
        }

        // Parse: SELECT * FROM <table> [WHERE <col> = '<val>'] [LIMIT <n>]
        const selectMatch = q.match(/^SELECT\s+(\*|[\w,\s]+)\s+FROM\s+(\w+)(?:\s+WHERE\s+(\w+)\s*=\s*['"]?([^'"]+)['"]?)?(?:\s+ORDER\s+BY\s+(\w+)(?:\s+(ASC|DESC))?)?(?:\s+LIMIT\s+(\d+))?$/i);
        if (selectMatch) {
            const [, fieldsStr, tableName, whereCol, whereVal, orderCol, orderDir, limitStr] = selectMatch;
            const table = tableName.toLowerCase();
            if (!this.tables.has(table)) {
                this.cacheStats.misses++;
                return { query: q, success: false, error: `Table '${tableName}' does not exist.` };
            }
            this.cacheStats.hits++;
            let rows = [];
            if (whereCol && whereVal) {
                rows = this.findBy(table, whereCol, whereVal);
            } else {
                rows = Array.from(this.tables.get(table).values());
            }

            if (orderCol) {
                const dir = (orderDir || 'ASC').toUpperCase() === 'DESC' ? -1 : 1;
                rows.sort((a, b) => {
                    if (a[orderCol] < b[orderCol]) return -1 * dir;
                    if (a[orderCol] > b[orderCol]) return 1 * dir;
                    return 0;
                });
            }

            if (limitStr) {
                rows = rows.slice(0, parseInt(limitStr, 10));
            }

            const elapsedMs = Number(process.hrtime.bigint() - start) / 1e6;
            return {
                query: q,
                success: true,
                type: 'SELECT',
                table,
                count: rows.length,
                rows,
                latencyMs: Math.round(elapsedMs * 1000) / 1000,
                cached: true
            };
        }

        // Parse: SHOW TABLES
        if (/^SHOW\s+TABLES$/i.test(q)) {
            const tableSummaries = Array.from(this.tables.keys()).map(name => ({
                table: name,
                records: this.tables.get(name).size,
                status: 'HEALTHY'
            }));
            return {
                query: q,
                success: true,
                type: 'SCHEMA',
                tables: tableSummaries,
                latencyMs: 0.03
            };
        }

        return {
            query: q,
            success: false,
            error: 'Unsupported syntax. Try: SELECT * FROM users, SELECT * FROM user_case_files WHERE status = \'ACTIVE\', SHOW TABLES, or CACHE STATS.'
        };
    }

    getStats() {
        let totalRecords = 0;
        const tableSizes = {};
        this.tables.forEach((map, name) => {
            totalRecords += map.size;
            tableSizes[name] = map.size;
        });
        const hitRatio = this.cacheStats.totalQueries > 0
            ? Math.round((this.cacheStats.hits / this.cacheStats.totalQueries) * 1000) / 10
            : 100.0;
        return {
            status: 'ONLINE',
            engine: 'AegisDB v2.4 Enterprise In-Memory Hybrid',
            totalTables: this.tables.size,
            totalRecords,
            tableSizes,
            cache: {
                hits: this.cacheStats.hits,
                misses: this.cacheStats.misses,
                totalQueries: this.cacheStats.totalQueries,
                hitRatioPercent: hitRatio,
                avgQueryLatencyMs: Math.round(this.cacheStats.avgLatencyMs * 1000) / 1000
            }
        };
    }
}

module.exports = AegisDB;
