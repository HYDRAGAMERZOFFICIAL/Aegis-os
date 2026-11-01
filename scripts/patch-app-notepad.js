const fs = require('fs');
const path = require('path');

const appFile = path.join(__dirname, '..', 'simulator', 'web', 'app.js');
let content = fs.readFileSync(appFile, 'utf8');

// 1. Add NotepadApp definition before Palette
const notepadAppCode = `
    // ==========================================
    // 14.5. OPERATOR CASE NOTEPAD CONTROLLER
    // ==========================================
    const NotepadApp = {
        init(win) {
            const textarea = win.querySelector('#notepad-textarea');
            const btnInsertIoc = win.querySelector('#btn-notes-insert-ioc');
            const btnSave = win.querySelector('#btn-notes-save');
            const btnLoad = win.querySelector('#btn-notes-load');
            const btnClear = win.querySelector('#btn-notes-clear');
            const stats = win.querySelector('#notepad-stats');
            const status = win.querySelector('#notepad-status');

            const updateStats = () => {
                if (!textarea) return;
                const val = textarea.value;
                const lines = val ? val.split('\\n').length : 1;
                const words = val.trim() ? val.trim().split(/\\s+/).length : 0;
                const chars = val.length;
                if (stats) stats.textContent = \`Lines: \${lines} | Words: \${words} | Characters: \${chars}\`;
            };
            textarea?.addEventListener('input', updateStats);

            btnInsertIoc?.addEventListener('click', () => {
                const state = window.AegisCampaign ? window.AegisCampaign.state : null;
                const missionTitle = window.AegisCampaign && window.AegisCampaign.mission ? window.AegisCampaign.mission.title : 'Active Operation';
                const iocSnippet = \`\\n[AEGIS CASE EVIDENCE RECORD]\\nTimestamp: \${new Date().toISOString()}\\nOperation: \${missionTitle}\\nHostile Endpoint: 185.220.101.5:8443 (C2 Beacon)\\nCompromised Token: TOK-KERNEL-ESCALATED (CAP_NET_RAW_SOCKET)\\nRogue Gateway PID: 4819 (rogue_c2_proxy on port 8443)\\nClean Rollback Point: chkpt-0001 (Root: 0x8f4d92a15c8e...)\\nNote: Verify all evidence milestones before sealing incident report.\\n\`;
                textarea.value += iocSnippet;
                updateStats();
                Notifications.show('IOCs Appended', 'Active case indicators inserted into notepad.', 'info');
            });

            btnSave?.addEventListener('click', async () => {
                try {
                    const res = await fetch('/api/files/write', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({
                            path: '/userland/notes/case_notes.txt',
                            content: textarea.value
                        })
                    });
                    const data = await res.json();
                    if (data.success) {
                        Notifications.show('Notes Saved', 'Saved case notes to /userland/notes/case_notes.txt', 'success');
                        if (status) status.textContent = 'Saved to AegisFS ✓';
                    }
                } catch (e) {
                    Notifications.show('Save Error', e.message, 'error');
                }
            });

            btnLoad?.addEventListener('click', async () => {
                try {
                    const res = await fetch('/api/files/read?path=/userland/notes/case_notes.txt');
                    const data = await res.json();
                    if (data.file) {
                        textarea.value = data.content || '';
                        updateStats();
                        Notifications.show('Notes Loaded', 'Loaded notes from /userland/notes/case_notes.txt', 'info');
                        if (status) status.textContent = 'Loaded from AegisFS ✓';
                    } else {
                        Notifications.show('File Not Found', 'No case_notes.txt found yet in AegisFS. Type your notes and click Save.', 'info');
                    }
                } catch (e) { }
            });

            btnClear?.addEventListener('click', () => {
                if (confirm('Clear notepad content?')) {
                    textarea.value = '';
                    updateStats();
                }
            });
            updateStats();
        }
    };
`;

if (!content.includes('const NotepadApp = {')) {
    content = content.replace('const Palette = {', `${notepadAppCode}\n    const Palette = {`);
}

// 2. Add notepad to WM.initAppLogic
if (!content.includes("case 'notepad':")) {
    content = content.replace(
        "case 'voice': VoiceApp.init(win); break;",
        "case 'voice': VoiceApp.init(win); break;\n                case 'notepad': NotepadApp.init(win); break;"
    );
}

// 3. Add notepad to labels in initSystem
if (!content.includes("notepad: 'Operator Case Notes")) {
    content = content.replace(
        "voice: 'Aegis Voice Assistant'",
        "voice: 'Aegis Voice Assistant',\n                    notepad: 'Operator Case Notes & Dossier Journal'"
    );
}

// 4. Remove automatic window popups upon login / entry
const oldAutoOpen = `        // Open Aegis Mind AI & Storage Manager on first load
        setTimeout(() => {
            WM.open('ai', 'Aegis Mind AI Controller', 840, 560);
            WM.open('files', 'Files & Storage Manager', 800, 510);
            Notifications.show('AegisOS 2.0 Ready', 'Aegis Mind AI online. Tiered storage & Cybersecurity Arsenal active.', 'success', 5000);
        }, 400);`;

const newAutoOpen = `        // Zero automatic window popups upon login - operator desktop remains clean
        Notifications.show('AegisOS 2.0 Ready', 'Virtual microkernel online. Zero-Trust defense active.', 'success', 4000);`;

content = content.replace(oldAutoOpen, newAutoOpen);

fs.writeFileSync(appFile, content, 'utf8');
console.log('Successfully updated simulator/web/app.js with NotepadApp and removed auto window popups.');
