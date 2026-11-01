'use strict';
const fs = require('fs');
const path = require('path');

const indexPath = path.resolve('simulator/web/index.html');
let html = fs.readFileSync(indexPath, 'utf8');

const oldSidebar = '<button class="settings-nav-btn" data-tab="set-about">About AegisOS</button>';
const newSidebar = '<button class="settings-nav-btn" data-tab="set-vault">Operator Database & Vault</button>\r\n                <button class="settings-nav-btn" data-tab="set-about">About AegisOS</button>';

if (html.includes(oldSidebar) && !html.includes('set-vault')) {
    html = html.replace(oldSidebar, newSidebar);
}

const oldAbout = '<div class="settings-panel hidden" id="set-about">';
const vaultPanel = `<div class="settings-panel hidden" id="set-vault">
                    <h3>Operator Database & Persistent Vault</h3>
                    <p class="text-muted">AegisDB in-memory relational engine stores your campaign state, case notes, rank, and achievements, persisted to disk and restored automatically upon login.</p>
                    <div class="setting-row">
                        <div>
                            <strong>AegisDB In-Memory Storage</strong>
                            <div class="text-muted">Status: ACTIVE • Latency: &lt;0.05ms • In-Memory Relational Cache</div>
                        </div>
                        <button type="button" class="btn-action-small" id="btn-settings-open-vault" style="color: #00f3ff; border-color: #00f3ff;">Open Operator Data Vault</button>
                    </div>
                    <div class="setting-row">
                        <div>
                            <strong>Automatic Session Synchronization</strong>
                            <div class="text-muted">Continuously preserves case dossier, journal notes, and investigative milestones</div>
                        </div>
                        <span class="badge" style="background: rgba(0, 243, 255, 0.2); color: #00f3ff;">ENABLED</span>
                    </div>
                </div>\r\n\r\n                <div class="settings-panel hidden" id="set-about">`;

if (html.includes(oldAbout) && !html.includes('id="set-vault"')) {
    html = html.replace(oldAbout, vaultPanel);
}

fs.writeFileSync(indexPath, html, 'utf8');
console.log('Successfully patched index.html');
