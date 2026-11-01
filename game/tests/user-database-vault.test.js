'use strict';

const test = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const http = require('node:http');

const web = path.join(__dirname, '..', '..', 'simulator', 'web');
const desktopHtml = fs.readFileSync(path.join(web, 'index.html'), 'utf8');
const campaignJs = fs.readFileSync(path.join(web, 'native-campaign.js'), 'utf8');
const desktopAuthJs = fs.readFileSync(path.join(web, 'desktop-auth.js'), 'utf8');

test('AegisDB desktop icon is removed from the desktop grid and dock', () => {
    // Must NOT have desktop-icon data-app="database"
    assert.doesNotMatch(desktopHtml, /<div class="desktop-icon" data-app="database">/);
    // Must NOT have dock button data-app="database"
    assert.doesNotMatch(desktopHtml, /<button class="dock-app-btn" data-app="database"/);
});

test('Dedicated Operator Data Vault & User Database UI exists in index.html', () => {
    assert.match(desktopHtml, /id="aegis-user-vault-modal"/);
    assert.match(desktopHtml, /AEGIS-OS OPERATOR DATA VAULT & USER DATABASE/);
    assert.match(desktopHtml, /id="vault-btn-sync"/);
    assert.match(desktopHtml, /id="vault-btn-restore"/);
    assert.match(desktopHtml, /id="vault-btn-export"/);
    assert.match(desktopHtml, /id="set-vault"/);
    assert.match(desktopHtml, /btn-settings-open-vault/);
});

test('User Database Vault REST API returns user data and synchronizes progress across logins', async () => {
    // 1. Register a dedicated test user
    const username = 'vault_op_' + Math.floor(Math.random() * 8999 + 1000);
    const regPayload = JSON.stringify({
        username,
        displayName: 'Vault Commander',
        password: 'Password999!'
    });

    const regRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/auth/register',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(regPayload)
            }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => {
                const cookie = res.headers['set-cookie'];
                resolve({ status: res.statusCode, body: JSON.parse(body || '{}'), cookie });
            });
        });
        req.on('error', reject);
        req.write(regPayload);
        req.end();
    });
    assert.equal(regRes.status, 201);
    const cookieHeader = Array.isArray(regRes.cookie) ? regRes.cookie[0].split(';')[0] : '';

    // 2. Sync user data to database vault
    const syncPayload = JSON.stringify({
        notes: '[CONFIDENTIAL DOSSIER]\nTarget: SATCOM Orbital Repeater\nMagic Header: 53 41 54 43',
        campaign: {
            v: 1,
            index: 6,
            started: true,
            done: ['op1', 'op2', 'op3', 'op4', 'op5', 'op6'],
            log: [{ id: 'op7', t: '14:20:00', s: 'Satcom telemetry anomaly identified' }]
        },
        profile: {
            xp: 3500,
            credits: 2200,
            rank: 'Senior Cyber Specialist',
            completedMissions: 6
        }
    });

    const syncRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/user/vault/sync',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(syncPayload),
                'Cookie': cookieHeader
            }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.write(syncPayload);
        req.end();
    });
    assert.equal(syncRes.success, true);
    assert.ok(syncRes.message.includes('synchronized to AegisDB'));

    // 3. GET /api/user/vault returns restored user database state
    const vaultRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/user/vault',
            method: 'GET',
            headers: { 'Cookie': cookieHeader }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.end();
    });

    assert.equal(vaultRes.success, true);
    assert.equal(vaultRes.status, 'ONLINE');
    assert.equal(vaultRes.vault.user.username, username);
    assert.equal(vaultRes.vault.profile.xp, 3500);
    assert.equal(vaultRes.vault.profile.credits, 2200);
    assert.ok(vaultRes.vault.notes.includes('SATCOM Orbital Repeater'));
    assert.equal(vaultRes.vault.campaign.index, 6);
    assert.equal(vaultRes.vault.campaign.done.length, 6);
});

test('Campaign defines at least 8 major operations with multi-stage progression and elaborated hints', () => {
    // 8 Operations check
    const requiredOps = [
        'THE SILENT INTRUSION',
        'GHOST IN THE KERNEL',
        'BLACKOUT PROTOCOL',
        'ZERO-DAY RANSOMWARE HEIST',
        'QUANTUM CIPHER EXFILTRATION',
        'FIRMWARE SUPPLY-CHAIN POISONING',
        'SATCOM TELEMETRY INTERCEPT & FIRMWARE HIJACK',
        'NEURAL BIOMETRIC BYPASS & DEEPFAKE MODEL INJECTION'
    ];

    for (const title of requiredOps) {
        assert.ok(campaignJs.includes(title), 'Missing operational mission: ' + title);
    }

    // Multi-stage progression requirements
    assert.match(campaignJs, /progress\.length===s\[3\]\.length/);
    assert.match(campaignJs, /step-stage-track/);
    assert.match(campaignJs, /is-stage-done/);
    assert.match(campaignJs, /is-stage-active/);

    // Elaborated hints and guides
    assert.match(campaignJs, /STAGE_GUIDES/);
    assert.match(campaignJs, /step-elaborated-box/);
    assert.match(campaignJs, /elab-ioc/);
    assert.match(campaignJs, /elab-proc/);
    assert.match(campaignJs, /elab-bg/);
});
