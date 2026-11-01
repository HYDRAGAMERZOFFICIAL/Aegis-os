const fs = require('fs');
const path = require('path');

const appFile = path.resolve('simulator/web/app.js');
let code = fs.readFileSync(appFile, 'utf8');

// 1. Replace confirm in NotepadApp with AegisDialog.confirm
code = code.replace(
    "if (confirm('Clear notepad content?')) {",
    "const ok = window.AegisDialog ? await window.AegisDialog.confirm('Clear all notepad content from editor?', 'CLEAR CASE NOTES') : true;\r\n                if (ok) {"
);

// 2. Enhance StoreApp to support category filtering and "Coming Soon" banners
const oldStoreCode = `    const StoreApp = {
        init(win) {
            const catalog = win.querySelector('#store-catalog');
            const apps = [
                { id: 'devstudio', name: 'Aegis DevStudio', author: 'Aegis Core Team', desc: 'Integrated Rust & C development environment with live microkernel debugger.', installed: true },
                { id: 'cyberdeck', name: 'CyberDeck IDS', author: 'Quantum Security Labs', desc: 'Advanced packet graph visualizer and stateful intrusion forensics console.', installed: false },
                { id: 'browser', name: 'Quantum Browser', author: 'Mozilla / Aegis Port', desc: 'Sandboxed Zero-Trust browser with per-tab micro-virtualization memory isolation.', installed: false },
                { id: 'cryptsafe', name: 'CryptSafe Vault', author: 'Shield Foundation', desc: 'Hardware-backed AES-256 password & cryptographic identity manager.', installed: true }
            ];

            catalog.innerHTML = apps.map(app => \`
                <div class="app-card">
                    <div class="app-card-top">
                        <div class="app-card-icon">
                            <svg viewBox="0 0 24 24"><polygon points="12 2 2 7 12 12 22 7 12 2"/><polyline points="2 17 12 22 22 17"/><polyline points="2 12 12 17 22 12"/></svg>
                        </div>
                        <div>
                            <div class="app-card-title">\${app.name}</div>
                            <div class="app-card-author">\${app.author}</div>
                        </div>
                    </div>
                    <div class="app-card-desc">\${app.desc}</div>
                    <div class="app-card-footer">
                        <span class="app-badge-verified">
                            <svg viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg> Verified
                        </span>
                        <button class="btn-action-small btn-install" data-id="\${app.id}">
                            \${app.installed ? 'Installed' : 'Install'}
                        </button>
                    </div>
                </div>
            \`).join('');

            catalog.querySelectorAll('.btn-install').forEach(btn => {
                btn.addEventListener('click', () => {
                    const id = btn.dataset.id;
                    const target = apps.find(a => a.id === id);
                    if (target) {
                        target.installed = !target.installed;
                        btn.textContent = target.installed ? 'Installed' : 'Install';
                        btn.classList.toggle('installed', target.installed);
                        Notifications.show(target.installed ? 'Package Installed' : 'Package Removed',
                            \`\${target.name} has been \${target.installed ? 'isolated in Ring-3 sandbox' : 'uninstalled'}.\`,
                            target.installed ? 'success' : 'info');
                    }
                });
            });
        }
    };`;

const newStoreCode = `    const StoreApp = {
        init(win) {
            const catalog = win.querySelector('#store-catalog');
            const searchInput = win.querySelector('#store-search');
            const tagBtns = win.querySelectorAll('.store-filter-tags .tag-btn');

            let currentCategory = 'all';
            let searchQuery = '';

            const apps = [
                { id: 'devstudio', name: 'Aegis DevStudio', author: 'Aegis Core Team', cat: 'developer', desc: 'Integrated Rust & C development environment with live microkernel debugger.', installed: true },
                { id: 'cyberdeck', name: 'CyberDeck IDS', author: 'Quantum Security Labs', cat: 'security', desc: 'Advanced packet graph visualizer and stateful intrusion forensics console.', installed: false },
                { id: 'browser', name: 'Quantum Browser', author: 'Mozilla / Aegis Port', cat: 'security', desc: 'Sandboxed Zero-Trust browser with per-tab micro-virtualization memory isolation.', installed: false },
                { id: 'cryptsafe', name: 'CryptSafe Vault', author: 'Shield Foundation', cat: 'security', desc: 'Hardware-backed AES-256 password & cryptographic identity manager.', installed: true },
                { id: 'hexsuite', name: 'HexCarver Pro', author: 'Forensic Tools Foundation', cat: 'utilities', desc: 'Low-level inode carver, raw sector scanner, and file magic byte repair.', installed: false },
                { id: 'aegisdb_cli', name: 'AegisDB Studio CLI', author: 'Database Working Group', cat: 'developer', desc: 'Terminal bindings and query visualizer for in-memory hybrid tables.', installed: true }
            ];

            const renderCatalog = () => {
                let filtered = apps;
                if (currentCategory !== 'all') {
                    filtered = filtered.filter(a => a.cat === currentCategory);
                }
                if (searchQuery) {
                    filtered = filtered.filter(a => a.name.toLowerCase().includes(searchQuery) || a.desc.toLowerCase().includes(searchQuery));
                }

                if (filtered.length === 0) {
                    catalog.innerHTML = \`
                        <div class="coming-soon-banner" style="grid-column: 1 / -1;">
                            <span class="cs-icon">⚡</span>
                            <h4>FEATURE COMING SOON // AEGIS LABS REPOSITORY</h4>
                            <p>Applications in this category are currently undergoing sandbox verification for Kernel v2.1 Alpha. Check back during the next synchronization.</p>
                        </div>
                    \`;
                    return;
                }

                catalog.innerHTML = filtered.map(app => \`
                    <div class="app-card">
                        <div class="app-card-top">
                            <div class="app-card-icon">
                                <svg viewBox="0 0 24 24"><polygon points="12 2 2 7 12 12 22 7 12 2"/><polyline points="2 17 12 22 22 17"/><polyline points="2 12 12 17 22 12"/></svg>
                            </div>
                            <div>
                                <div class="app-card-title">\${app.name}</div>
                                <div class="app-card-author">\${app.author}</div>
                            </div>
                        </div>
                        <div class="app-card-desc">\${app.desc}</div>
                        <div class="app-card-footer">
                            <span class="app-badge-verified">
                                <svg viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg> Verified
                            </span>
                            <button class="btn-action-small btn-install" data-id="\${app.id}">
                                \${app.installed ? 'Installed' : 'Install'}
                            </button>
                        </div>
                    </div>
                \`).join('');

                catalog.querySelectorAll('.btn-install').forEach(btn => {
                    btn.addEventListener('click', () => {
                        const id = btn.dataset.id;
                        const target = apps.find(a => a.id === id);
                        if (target) {
                            target.installed = !target.installed;
                            btn.textContent = target.installed ? 'Installed' : 'Install';
                            btn.classList.toggle('installed', target.installed);
                            Notifications.show(target.installed ? 'Package Installed' : 'Package Removed',
                                \`\${target.name} has been \${target.installed ? 'isolated in Ring-3 sandbox' : 'uninstalled'}.\`,
                                target.installed ? 'success' : 'info');
                        }
                    });
                });
            };

            tagBtns.forEach(btn => {
                btn.addEventListener('click', () => {
                    tagBtns.forEach(b => b.classList.remove('active'));
                    btn.classList.add('active');
                    currentCategory = btn.textContent.toLowerCase().trim();
                    renderCatalog();
                });
            });

            searchInput?.addEventListener('input', (e) => {
                searchQuery = e.target.value.toLowerCase().trim();
                renderCatalog();
            });

            renderCatalog();
        }
    };`;

code = code.replace(oldStoreCode.replace(/\n/g, '\r\n'), newStoreCode.replace(/\n/g, '\r\n'));

// 3. Polyfill window.alert to use AegisDialog
if (!code.includes('window.alert = function(')) {
    const alertPolyfill = `
    // Ensure all alert calls use custom OS AegisDialog popup
    window.alert = function(msg) {
        if (window.AegisDialog) {
            window.AegisDialog.alert(String(msg || ''), 'SYSTEM NOTIFICATION');
        } else {
            Notifications.show('System Alert', String(msg || ''), 'info');
        }
    };
`;
    code = code.replace(
        'window.AegisSystemNotify =',
        alertPolyfill + '\r\n    window.AegisSystemNotify ='
    );
}

fs.writeFileSync(appFile, code, 'utf8');
console.log('Successfully updated app.js with category filters, Coming Soon banners, and AegisDialog polyfill!');
