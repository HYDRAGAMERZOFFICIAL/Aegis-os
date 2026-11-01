const fs = require('fs');
const path = require('path');

const htmlFile = path.resolve('simulator/web/index.html');
let html = fs.readFileSync(htmlFile, 'utf8');

const modals = `    <!-- AEGIS ACTION & CONFIRMATION POPUP MODAL (Replaces browser alert/confirm) -->
    <div id="aegis-dialog-modal" class="modal-overlay hidden" aria-live="assertive">
        <div class="aegis-dialog-box glass-panel">
            <div class="aegis-dialog-header">
                <div class="aegis-dialog-brand">
                    <span class="aegis-dialog-icon">🛡️</span>
                    <span class="aegis-dialog-title" id="aegis-dialog-title">SYSTEM ACTION REQUIRED</span>
                </div>
                <button class="aegis-dialog-close" id="aegis-dialog-close" type="button">✕</button>
            </div>
            <div class="aegis-dialog-body" id="aegis-dialog-body">
                <p id="aegis-dialog-message" class="aegis-dialog-message">Action confirmation required.</p>
            </div>
            <div class="aegis-dialog-footer" id="aegis-dialog-footer">
                <button class="btn-action-small" id="aegis-dialog-cancel" type="button">CANCEL</button>
                <button class="btn-action-small btn-primary" id="aegis-dialog-confirm" type="button" style="color: var(--accent); border-color: var(--accent);">CONFIRM ACTION</button>
            </div>
        </div>
    </div>

    <!-- AEGIS CLUE CARD DETAIL INSPECTOR MODAL -->
    <div id="aegis-clue-card-modal" class="modal-overlay hidden" aria-live="polite">
        <div class="aegis-clue-detail-box glass-panel">
            <div class="aegis-dialog-header">
                <div class="aegis-dialog-brand">
                    <span class="aegis-dialog-icon">🎴</span>
                    <span class="aegis-dialog-title" id="clue-modal-title">FORENSIC CLUE CARD</span>
                </div>
                <button class="aegis-dialog-close" id="clue-modal-close" type="button">✕</button>
            </div>
            <div class="aegis-clue-detail-body">
                <div class="clue-modal-badge-row">
                    <span class="clue-modal-type-badge" id="clue-modal-type">NETWORK_TRACE</span>
                    <span class="clue-modal-status-badge" id="clue-modal-status">UNCOVERED ✓</span>
                </div>
                <h3 id="clue-modal-heading" class="clue-modal-heading">Clue Description</h3>
                <div class="clue-modal-ioc-box">
                    <label>INDICATOR OF COMPROMISE (IOC):</label>
                    <code id="clue-modal-ioc">185.220.101.5:8443</code>
                </div>
                <div class="clue-modal-snippet-box">
                    <label>CRYPTOGRAPHIC / HEX TELEMETRY:</label>
                    <pre id="clue-modal-hex">0x004018A0: CALL ring3_trampoline</pre>
                </div>
                <div class="clue-modal-guide-box">
                    <label>TACTICAL INVESTIGATOR'S GUIDE:</label>
                    <p id="clue-modal-guide">Detailed instructions on how to corroborate this clue and which tool to deploy next.</p>
                </div>
            </div>
            <div class="aegis-dialog-footer">
                <button class="btn-action-small" id="btn-clue-insert-notes" type="button">📝 Copy to Case Notes</button>
                <button class="btn-action-small btn-primary" id="btn-clue-jump-tool" type="button" style="color: var(--accent); border-color: var(--accent);">🚀 Open Required Tool</button>
            </div>
        </div>
    </div>
`;

if (!html.includes('id="aegis-dialog-modal"')) {
    html = html.replace(
        '<div id="aegis-tutorial-overlay"',
        modals + '\n    <div id="aegis-tutorial-overlay"'
    );
    fs.writeFileSync(htmlFile, html, 'utf8');
    console.log('Successfully added aegis-dialog-modal and aegis-clue-card-modal to index.html');
} else {
    console.log('Modals already present in index.html');
}

