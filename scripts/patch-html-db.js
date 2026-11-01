const fs = require('fs');
const path = require('path');

const htmlFile = path.resolve('simulator/web/index.html');
let html = fs.readFileSync(htmlFile, 'utf8');

// 1. Add Database desktop icon
const dbDesktopIcon = `            <div class="desktop-icon" data-app="database">
                <div class="icon-badge icon-database" style="background: linear-gradient(135deg, rgba(0, 243, 255, 0.2), rgba(157, 0, 255, 0.2)); border-color: #00f3ff;">
                    <svg viewBox="0 0 24 24" fill="none" stroke="#00f3ff" stroke-width="2">
                        <ellipse cx="12" cy="5" rx="9" ry="3"/>
                        <path d="M21 12c0 1.66-4 3-9 3s-9-1.34-9-3"/>
                        <path d="M3 5v14c0 1.66 4 3 9 3s9-1.34 9-3V5"/>
                    </svg>
                </div>
                <span class="icon-label">AegisDB</span>
            </div>
`;

if (!html.includes('data-app="database"')) {
    html = html.replace(
        '<div class="desktop-icon" data-app="notepad">',
        dbDesktopIcon + '            <div class="desktop-icon" data-app="notepad">'
    );
}

// 2. Add Database dock button
const dbDockBtn = `            <button class="dock-app-btn" data-app="database" title="AegisDB Studio (In-Memory System Database)">
                <svg viewBox="0 0 24 24"><ellipse cx="12" cy="5" rx="9" ry="3"/><path d="M21 12c0 1.66-4 3-9 3s-9-1.34-9-3"/><path d="M3 5v14c0 1.66 4 3 9 3s9-1.34 9-3V5"/></svg>
            </button>
`;

if (!html.includes('data-app="database" title="AegisDB')) {
    html = html.replace(
        '<button class="dock-app-btn" data-app="notepad"',
        dbDockBtn + '            <button class="dock-app-btn" data-app="notepad"'
    );
}

// 3. Add Database template tmpl-database
const dbTemplate = `    <!-- 16. AEGISDB STUDIO & IN-MEMORY DATABASE MANAGER -->
    <template id="tmpl-database">
        <div class="app-database">
            <div class="db-header-bar">
                <div class="db-brand">
                    <span class="db-logo">⚡ AegisDB</span>
                    <span class="db-badge">IN-MEMORY HYBRID</span>
                    <span class="db-badge db-badge-perf" id="db-perf-badge">LATENCY: 0.04ms · HIT RATE: 99.8%</span>
                </div>
                <div class="db-actions">
                    <button class="btn-action-small" id="btn-db-refresh">🔄 Refresh</button>
                    <button class="btn-action-small" id="btn-db-flush" style="color: #ff9d00; border-color: #ff9d00;">⚡ Flush Cache</button>
                    <button class="btn-action-small" id="btn-db-export">💾 Export</button>
                </div>
            </div>

            <!-- Table Navigation Tabs -->
            <div class="db-nav-tabs" id="db-table-tabs">
                <button class="db-tab-btn active" data-table="users">👤 users</button>
                <button class="db-tab-btn" data-table="user_case_files">📁 user_case_files</button>
                <button class="db-tab-btn" data-table="user_side_quests">🎯 user_side_quests</button>
                <button class="db-tab-btn" data-table="user_audit_logs">🛡️ user_audit_logs</button>
                <button class="db-tab-btn" data-table="user_profiles">🎖️ user_profiles</button>
                <button class="db-tab-btn" data-table="user_sessions">🔑 user_sessions</button>
                <button class="db-tab-btn" data-table="system_cache">⚡ system_cache</button>
            </div>

            <!-- AegisQL Interactive Query Box -->
            <div class="db-query-panel">
                <div class="db-query-input-row">
                    <span class="db-prompt">aegis_ql&gt;</span>
                    <input type="text" class="db-query-input" id="db-query-input" placeholder="SELECT * FROM user_case_files WHERE status = 'ACTIVE'..." value="SELECT * FROM user_case_files" spellcheck="false" />
                    <button class="btn-action-small" id="btn-db-exec-query" style="color: var(--accent); border-color: var(--accent);">EXECUTE</button>
                </div>
                <div class="db-quick-queries">
                    <span class="db-quick-label">Quick Queries:</span>
                    <button class="db-chip-btn" data-q="SELECT * FROM users">Users</button>
                    <button class="db-chip-btn" data-q="SELECT * FROM user_case_files">Active Cases</button>
                    <button class="db-chip-btn" data-q="SELECT * FROM user_side_quests">Side Quests</button>
                    <button class="db-chip-btn" data-q="SELECT * FROM user_audit_logs">Audit Logs</button>
                    <button class="db-chip-btn" data-q="CACHE STATS">Cache Stats</button>
                </div>
            </div>

            <!-- Table Search & Filter Bar -->
            <div class="db-filter-bar">
                <input type="text" class="db-search-input" id="db-search-input" placeholder="🔍 Live filter records in memory..." />
                <span class="db-record-count" id="db-record-count">Records: 0</span>
                <span class="db-cache-tag" id="db-query-tag">⚡ Memory Hit (0.03ms)</span>
            </div>

            <!-- Data Grid View -->
            <div class="db-grid-container" id="db-grid-container">
                <table class="db-table" id="db-data-table">
                    <thead id="db-table-head"></thead>
                    <tbody id="db-table-body"></tbody>
                </table>
            </div>

            <!-- Record Detail Preview Panel -->
            <div class="db-record-detail" id="db-record-detail" style="display: none;">
                <div class="db-detail-header">
                    <strong id="db-detail-title">Record Details</strong>
                    <button class="btn-action-small" id="btn-db-close-detail">✕ Close</button>
                </div>
                <pre class="db-detail-json" id="db-detail-json"></pre>
            </div>
        </div>
    </template>
`;

if (!html.includes('id="tmpl-database"')) {
    html = html.replace('</body>', dbTemplate + '\n</body>');
}

fs.writeFileSync(htmlFile, html, 'utf8');
console.log('Successfully patched index.html with AegisDB desktop icon and template!');
