const fs = require('fs');
const path = require('path');

const htmlFile = path.join(__dirname, '..', 'simulator', 'web', 'index.html');
let content = fs.readFileSync(htmlFile, 'utf8');

// 1. Add Boot Screen, Critical Breach Overlay, and Tutorial Overlay before desktop-surface
const overlaysHtml = `
    <!-- 1. VIRTUAL MICROKERNEL BOOT SCREEN -->
    <div id="aegis-boot-screen" class="boot-screen hidden" aria-live="polite">
        <div class="boot-content">
            <div class="boot-crest">
                <svg viewBox="0 0 100 100">
                    <polygon points="50,5 95,25 95,75 50,95 5,75 5,25" fill="#080c14" stroke="#00f3ff" stroke-width="6"/>
                    <path d="M50,20 L80,45 L50,80 L20,45 Z" fill="#00f3ff" opacity="0.8"/>
                </svg>
            </div>
            <h1 class="boot-title">AEGIS // OPERATOR STATION</h1>
            <div class="boot-console" id="boot-console">
                <div class="boot-line">> [  0.000000] Initializing Dual-Kernel Hybrid Architecture v2.0-Alpha...</div>
            </div>
            <div class="boot-bar-wrap">
                <div class="boot-bar-fill" id="boot-bar-fill"></div>
            </div>
            <div class="boot-status" id="boot-status">BOOTING VIRTUAL MICROKERNEL ENVIRONMENT...</div>
        </div>
    </div>

    <!-- 2. CRITICAL SYSTEM BREACH & 30-SECOND SHUTDOWN OVERLAY -->
    <div id="aegis-critical-breach-overlay" class="breach-overlay hidden" aria-live="assertive">
        <div class="breach-panel">
            <div class="breach-header">
                <span class="breach-skull">☠️</span>
                <span class="breach-title">CRITICAL ZERO-DAY BREACH // HOSTILE TAKEOVER</span>
            </div>
            <div class="breach-subtitle">YOU HAVE BEEN HACKED! HOSTILE CIPHER HAS COMPROMISED RING 0.</div>
            <div class="breach-warning">
                UNAUTHORIZED CIPHER WIPING INODE TABLES. ALL LOCAL FILES WILL BE PURGED AND SYSTEM WILL SHUT DOWN IN:
            </div>
            <div class="breach-timer-wrap">
                <span class="breach-clock" id="breach-timer-clock">00:30.00</span>
            </div>
            <div class="breach-puzzle-box" id="breach-puzzle-box">
                <div class="breach-hint" id="breach-hint">
                    <strong>DEFENSE OVERRIDE:</strong> Deploy emergency microkernel bypass cipher or inject security override key (<code>AEGIS-OVERRIDE-SEC99</code>).
                </div>
                <div class="breach-input-row">
                    <input type="text" id="breach-override-input" placeholder="Enter override cipher (e.g. AEGIS-OVERRIDE-SEC99)" autocomplete="off">
                    <button type="button" class="btn-breach-fix" id="btn-breach-fix">⚡ FIX THE SYSTEM</button>
                </div>
            </div>
            <div class="breach-status" id="breach-status">DEFENSE PROTOCOL ARMED. TIME IS RUNNING OUT.</div>
        </div>
    </div>

    <!-- 3. INTERACTIVE DEFENSE & BREACH RECOVERY TUTORIAL MODAL -->
    <div id="aegis-tutorial-overlay" class="modal-overlay hidden" aria-live="polite">
        <div class="tutorial-window glass-panel">
            <div class="tutorial-header">
                <div style="display: flex; align-items: center; gap: 8px;">
                    <span style="font-size: 18px;">🛡️</span>
                    <h3>TACTICAL CYBER DEFENSE & BREACH RECOVERY TUTORIAL</h3>
                </div>
                <button type="button" class="win-btn win-btn-close" id="btn-close-tutorial" title="Close Tutorial"></button>
            </div>
            <div class="tutorial-tabs">
                <button type="button" class="tutorial-tab active" data-tab="tut-threats">1. Threat Detection</button>
                <button type="button" class="tutorial-tab" data-tab="tut-breach">2. 30s Red Alert Lockdown</button>
                <button type="button" class="tutorial-tab" data-tab="tut-carve">3. File Inode Carving</button>
                <button type="button" class="tutorial-tab" data-tab="tut-rollback">4. Merkle Rollback</button>
                <button type="button" class="tutorial-tab" data-tab="tut-tools">5. Investigation Tools</button>
            </div>
            <div class="tutorial-body">
                <div class="tutorial-panel" id="tut-threats">
                    <h4>How Adversaries Attack AegisOS</h4>
                    <p>Adversaries deploy multi-stage attacks against the system:</p>
                    <ul>
                        <li><strong>Port Hijacking:</strong> Rogue daemons hijack ports (e.g. port 8443) to intercept ingress traffic. Use <em>Diagnostics & Triage</em> to kill rogue PIDs.</li>
                        <li><strong>Capability Escalation:</strong> Signed binaries request unauthorized capabilities like <code>CAP_NET_RAW_SOCKET</code>. Use <em>Aegis Shield</em> to revoke tokens.</li>
                        <li><strong>Ransomware Inode Corruption:</strong> Attackers corrupt file headers or encrypt vault data. Use <em>File Recovery & Hex Carver</em> to reconstruct them.</li>
                    </ul>
                </div>
                <div class="tutorial-panel hidden" id="tut-breach">
                    <h4>The 30-Second Critical Red Alert Lockdown</h4>
                    <p>When an active emergency attack fails or a critical zero-day strikes, the system enters <strong>Critical Lockdown</strong>:</p>
                    <div class="tutorial-alert-sample">
                        <strong>⚠️ YOU HAVE BEEN HACKED // SYSTEM SHUTS DOWN IN 30 SECONDS</strong>
                        <p>A flashing red screen will count down. Click <strong>[⚡ FIX THE SYSTEM]</strong> and enter the emergency override cipher: <code>AEGIS-OVERRIDE-SEC99</code> to defuse it!</p>
                    </div>
                    <p><strong>Warning:</strong> If the 30-second timer hits zero, all local files are wiped and you must carve and recover them from memory!</p>
                </div>
                <div class="tutorial-panel hidden" id="tut-carve">
                    <h4>How to Recover & Carve Corrupted Files</h4>
                    <p>In <strong>Files & Storage</strong>, open the <code>[🔬 File Recovery & Hex Carver]</code> tab:</p>
                    <ul>
                        <li>Select any corrupted file (e.g. <code>financial_audit.db.corrupt</code>).</li>
                        <li>Inspect the raw hex bytes and check the <strong>Forensic Clue</strong>.</li>
                        <li>Enter the repair magic header (e.g. <code>41 45 47 49</code> for AEGIS DB, or <code>7F 45 4C 46</code> for ELF drivers) or the decryption key (e.g. <code>0xAE915_KRN_SEC</code>).</li>
                        <li>Click <strong>[Carve & Restore Inode]</strong> to restore the file cleanly!</li>
                    </ul>
                </div>
                <div class="tutorial-panel hidden" id="tut-rollback">
                    <h4>Self-Healing Merkle Rollback</h4>
                    <p>When multiple blocks are scrambled by ransomware, open <strong>Self-Healing Filesystem</strong>:</p>
                    <ul>
                        <li>Click <strong>[Run Merkle Integrity Scan]</strong> to identify the diverged block.</li>
                        <li>Click <strong>[⚡ Rollback Checkpoint]</strong> to execute an atomic sub-10 second restoration to clean baseline <code>chkpt-0001</code>.</li>
                        <li>Or run <code>checkpoint rollback</code> in Terminal!</li>
                    </ul>
                </div>
                <div class="tutorial-panel hidden" id="tut-tools">
                    <h4>Operator Investigation Tools</h4>
                    <ul>
                        <li><strong>Ghost Browser:</strong> Air-gapped disposable web sandbox. All web memory is incinerated upon closing with 0x00 bytes.</li>
                        <li><strong>Operator Case Notes:</strong> Jot down critical case IOCs, hashes, and PIDs. Click [Insert IOCs] and save directly to AegisFS.</li>
                        <li><strong>Case Dossier (F4):</strong> Press F4 or click the Case Dossier pill to view objectives, review clues, and take on Live Drills!</li>
                    </ul>
                </div>
            </div>
            <div class="tutorial-footer">
                <span id="tut-page-indicator">Step 1 of 5</span>
                <div style="display: flex; gap: 8px;">
                    <button type="button" class="btn-action-small" id="btn-tut-prev" disabled>Previous</button>
                    <button type="button" class="btn-primary" id="btn-tut-next">Next Step →</button>
                    <button type="button" class="btn-primary-small hidden" id="btn-tut-complete">Complete Tutorial ✓</button>
                </div>
            </div>
        </div>
    </div>
`;

if (!content.includes('id="aegis-boot-screen"')) {
    content = content.replace('<!-- Global Desktop Wrapper -->', `${overlaysHtml}\n    <!-- Global Desktop Wrapper -->`);
}

// 2. Add Notepad Icon to Desktop Grid
const notepadDesktopIcon = `            <div class="desktop-icon" data-app="notepad">
                <div class="icon-badge icon-notepad"><svg viewBox="0 0 24 24"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/><polyline points="10 9 9 9 8 9"/></svg></div>
                <span class="icon-label">Case Notes</span>
            </div>`;

if (!content.includes('data-app="notepad"')) {
    content = content.replace('<div class="desktop-icon" data-app="voice">', `${notepadDesktopIcon}\n            <div class="desktop-icon" data-app="voice">`);
}

// 3. Add Notepad Button to Dock
const notepadDockBtn = `            <!-- Operator Case Notepad -->
            <button class="dock-app-btn" data-app="notepad" title="Operator Case Notes & Dossier Journal">
                <svg viewBox="0 0 24 24"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg>
                <span class="dock-dot"></span>
            </button>`;

if (!content.includes('title="Operator Case Notes & Dossier Journal"')) {
    content = content.replace('<button class="dock-app-btn" data-app="voice" title="Aegis Voice Assistant">', `${notepadDockBtn}\n        <button class="dock-app-btn" data-app="voice" title="Aegis Voice Assistant">`);
}

// 4. Add Notepad to Start Menu
const notepadStartItem = `            <div class="start-item" data-app="notepad">
                <div class="start-icon" style="color: #00f3ff;"><svg viewBox="0 0 24 24"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg></div>
                <span>Case Notes</span>
            </div>`;

if (!content.includes('<span>Case Notes</span>')) {
    content = content.replace('</div>\n        </div>\n        <div class="start-footer">', `${notepadStartItem}\n        </div>\n        <div class="start-footer">`);
}

// 5. Add tmpl-notepad template
const notepadTemplate = `
    <!-- 15. OPERATOR CASE NOTEPAD & DOSSIER JOURNAL -->
    <template id="tmpl-notepad">
        <div class="app-notepad">
            <div class="notepad-toolbar">
                <button class="btn-action-small" id="btn-notes-insert-ioc">📋 Insert Case IOCs</button>
                <button class="btn-action-small" id="btn-notes-load">📂 Open from AegisFS</button>
                <button class="btn-action-small" id="btn-notes-save" style="color: var(--accent); border-color: var(--accent);">💾 Save to AegisFS</button>
                <button class="btn-action-small" id="btn-notes-clear">✖ Clear</button>
                <span class="notepad-path" id="notepad-path-label">/userland/notes/case_notes.txt</span>
            </div>
            <div class="notepad-body">
                <textarea class="notepad-textarea" id="notepad-textarea" placeholder="Jot down forensic observations, IP addresses, suspicious PIDs, token IDs, and attack timelines..." spellcheck="false"></textarea>
            </div>
            <div class="notepad-footer">
                <span id="notepad-stats">Lines: 1 | Words: 0 | Characters: 0</span>
                <span id="notepad-status" style="color: var(--status-ok);">Ready</span>
            </div>
        </div>
    </template>
`;

if (!content.includes('id="tmpl-notepad"')) {
    content = content.replace('</body>', `${notepadTemplate}\n</body>`);
}

fs.writeFileSync(htmlFile, content, 'utf8');
console.log('Successfully updated simulator/web/index.html with overlays, notepad, and templates.');
