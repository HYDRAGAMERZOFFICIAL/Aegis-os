const fs = require('fs');
const path = require('path');

const appFile = path.resolve('simulator/web/app.js');
let code = fs.readFileSync(appFile, 'utf8');

// 1. Add case 'database' to WM.initAppLogic
if (!code.includes("case 'database': DatabaseApp.init(win); break;")) {
    code = code.replace(
        "case 'notepad': NotepadApp.init(win); break;",
        "case 'notepad': NotepadApp.init(win); break;\r\n                case 'database': DatabaseApp.init(win); break;"
    );
}

// 2. Add database label in labels object
if (!code.includes("database: 'AegisDB Studio (In-Memory System Database)'")) {
    code = code.replace(
        "notepad: 'Operator Case Notes & Dossier Journal'",
        "notepad: 'Operator Case Notes & Dossier Journal',\r\n                    database: 'AegisDB Studio (In-Memory System Database)'"
    );
}

// 3. Add DatabaseApp implementation
if (!code.includes('const DatabaseApp = {')) {
    const dbAppCode = `
    // ==========================================
    // 14.6. AEGISDB IN-MEMORY DATABASE CONTROLLER
    // ==========================================
    const DatabaseApp = {
        currentTable: 'user_case_files',
        cachedData: new Map(),

        async init(win) {
            const tableTabs = win.querySelectorAll('.db-tab-btn');
            const tableHead = win.querySelector('#db-table-head');
            const tableBody = win.querySelector('#db-table-body');
            const searchInput = win.querySelector('#db-search-input');
            const countLabel = win.querySelector('#db-record-count');
            const perfBadge = win.querySelector('#db-perf-badge');
            const queryTag = win.querySelector('#db-query-tag');
            const queryInput = win.querySelector('#db-query-input');
            const btnExec = win.querySelector('#btn-db-exec-query');
            const btnRefresh = win.querySelector('#btn-db-refresh');
            const btnFlush = win.querySelector('#btn-db-flush');
            const btnExport = win.querySelector('#btn-db-export');
            const detailPanel = win.querySelector('#db-record-detail');
            const detailJson = win.querySelector('#db-detail-json');
            const btnCloseDetail = win.querySelector('#btn-db-close-detail');

            const renderTable = (rows, isCached = true, latency = 0.04) => {
                if (!tableHead || !tableBody) return;
                tableHead.innerHTML = '';
                tableBody.innerHTML = '';
                if (!rows || rows.length === 0) {
                    tableBody.innerHTML = '<tr><td colspan="6" style="text-align:center; padding: 24px; color: var(--text-muted);">No records found in table \\'' + this.currentTable + '\\'.</td></tr>';
                    if (countLabel) countLabel.textContent = 'Records: 0';
                    return;
                }
                if (countLabel) countLabel.textContent = 'Records: ' + rows.length;
                if (queryTag) queryTag.textContent = '⚡ ' + (isCached ? 'In-Memory Hit' : 'Disk Read') + ' (' + latency + 'ms)';

                const cols = Object.keys(rows[0]);
                const trHead = document.createElement('tr');
                cols.forEach(col => {
                    const th = document.createElement('th');
                    th.textContent = col;
                    trHead.appendChild(th);
                });
                tableHead.appendChild(trHead);

                const q = (searchInput?.value || '').toLowerCase().trim();
                const filtered = q ? rows.filter(r => JSON.stringify(r).toLowerCase().includes(q)) : rows;

                filtered.forEach(row => {
                    const tr = document.createElement('tr');
                    cols.forEach(col => {
                        const td = document.createElement('td');
                        const val = row[col];
                        td.textContent = (typeof val === 'object' && val !== null) ? JSON.stringify(val) : String(val ?? '');
                        tr.appendChild(td);
                    });
                    tr.addEventListener('click', () => {
                        if (detailPanel && detailJson) {
                            detailPanel.style.display = 'block';
                            detailJson.textContent = JSON.stringify(row, null, 2);
                        }
                    });
                    tableBody.appendChild(tr);
                });
            };

            const loadTable = async (tableName, bypassCache = false) => {
                this.currentTable = tableName;
                tableTabs.forEach(t => t.classList.toggle('active', t.dataset.table === tableName));

                if (!bypassCache && this.cachedData.has(tableName)) {
                    renderTable(this.cachedData.get(tableName), true, 0.02);
                    return;
                }

                try {
                    const start = performance.now();
                    const res = await fetch('/api/db/table?name=' + encodeURIComponent(tableName));
                    const data = await res.json();
                    const elapsed = (performance.now() - start).toFixed(2);
                    if (data && data.rows) {
                        this.cachedData.set(tableName, data.rows);
                        renderTable(data.rows, data.cached, data.latencyMs || elapsed);
                    }
                } catch (err) {
                    Notifications.show('Database Read Error', err.message, 'error');
                }
            };

            const updatePerfStats = async () => {
                try {
                    const res = await fetch('/api/db/stats');
                    const stats = await res.json();
                    if (stats && stats.cache && perfBadge) {
                        perfBadge.textContent = 'LATENCY: ' + stats.cache.avgQueryLatencyMs + 'ms · HIT RATE: ' + stats.cache.hitRatioPercent + '%';
                    }
                } catch (_) {}
            };

            tableTabs.forEach(btn => {
                btn.addEventListener('click', () => {
                    loadTable(btn.dataset.table);
                });
            });

            btnExec?.addEventListener('click', async () => {
                const query = (queryInput?.value || '').trim();
                if (!query) return;
                try {
                    const start = performance.now();
                    const res = await fetch('/api/db/query', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ query })
                    });
                    const data = await res.json();
                    const elapsed = (performance.now() - start).toFixed(2);
                    if (data.success && data.rows) {
                        renderTable(data.rows, data.cached, data.latencyMs || elapsed);
                        Notifications.show('AegisQL Execution', 'Returned ' + data.count + ' rows in ' + (data.latencyMs || elapsed) + 'ms', 'success');
                    } else if (data.success && (data.data || data.tables)) {
                        if (detailPanel && detailJson) {
                            detailPanel.style.display = 'block';
                            detailJson.textContent = JSON.stringify(data.data || data.tables, null, 2);
                        }
                    } else if (!data.success) {
                        Notifications.show('Query Syntax Error', data.error || 'Failed to execute query.', 'error');
                    }
                    updatePerfStats();
                } catch (err) {
                    Notifications.show('Query Error', err.message, 'error');
                }
            });

            win.querySelectorAll('.db-chip-btn').forEach(chip => {
                chip.addEventListener('click', () => {
                    if (queryInput) queryInput.value = chip.dataset.q;
                    btnExec?.click();
                });
            });

            searchInput?.addEventListener('input', () => {
                const currentRows = this.cachedData.get(this.currentTable) || [];
                renderTable(currentRows, true, 0.01);
            });

            btnRefresh?.addEventListener('click', () => {
                loadTable(this.currentTable, true);
                updatePerfStats();
            });

            btnFlush?.addEventListener('click', async () => {
                this.cachedData.clear();
                await fetch('/api/db/cache/flush', { method: 'POST' });
                Notifications.show('Memory Cache Flushed', 'AegisDB in-memory tables re-indexed.', 'info');
                loadTable(this.currentTable, true);
                updatePerfStats();
            });

            btnExport?.addEventListener('click', () => {
                const rows = this.cachedData.get(this.currentTable) || [];
                const blob = new Blob([JSON.stringify(rows, null, 2)], { type: 'application/json' });
                const url = URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.href = url;
                a.download = 'aegis_' + this.currentTable + '_dump.json';
                a.click();
                Notifications.show('Snapshot Exported', 'Table \\'' + this.currentTable + '\\' saved to local storage.', 'success');
            });

            btnCloseDetail?.addEventListener('click', () => {
                if (detailPanel) detailPanel.style.display = 'none';
            });

            loadTable(this.currentTable);
            updatePerfStats();
        }
    };
`;
    code = code.replace(
        '    // 14.5. OPERATOR CASE NOTEPAD CONTROLLER',
        dbAppCode + '\r\n    // 14.5. OPERATOR CASE NOTEPAD CONTROLLER'
    );
}

fs.writeFileSync(appFile, code, 'utf8');
console.log('Successfully patched simulator/web/app.js with DatabaseApp controller!');
