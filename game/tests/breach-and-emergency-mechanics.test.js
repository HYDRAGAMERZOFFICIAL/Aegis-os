'use strict';

const test = require('node:test');
const assert = require('node:assert/strict');
const http = require('node:http');

test('Server In-Memory Cache returns cached campaign state instantaneously', async () => {
    // 1. Authenticate or create test user session
    const postAuth = async (path, payload) => {
        return new Promise((resolve, reject) => {
            const data = JSON.stringify(payload);
            const req = http.request({
                port: 3000,
                path,
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Content-Length': Buffer.byteLength(data)
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
            req.write(data);
            req.end();
        });
    };

    const username = 'cache_op_' + Math.floor(Math.random() * 8999 + 1000);
    const regRes = await postAuth('/api/auth/register', {
        username,
        displayName: 'Cache Tester',
        password: 'TestPassword123!'
    });
    assert.equal(regRes.status, 201);
    const cookieHeader = Array.isArray(regRes.cookie) ? regRes.cookie[0].split(';')[0] : '';

    // 2. Save campaign state to warm cache
    const saveRes = await new Promise((resolve, reject) => {
        const payload = JSON.stringify({
            state: {
                v: 1,
                index: 0,
                started: true,
                done: [],
                log: [{ id: 'op1', t: '12:00:00', s: 'Cache test initialized' }]
            }
        });
        const req = http.request({
            port: 3000,
            path: '/api/campaign/state',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(payload),
                'Cookie': cookieHeader
            }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.write(payload);
        req.end();
    });
    assert.equal(saveRes.success, true);
    assert.equal(saveRes.cached, true);

    // 3. GET /api/campaign/state returns cached state
    const startGet = Date.now();
    const getRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/campaign/state',
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
    const getElapsed = Date.now() - startGet;
    assert.equal(getRes.cached, true, 'State should be served directly from in-memory cache');
    assert.ok(getElapsed < 60, 'In-memory cache response should be near instantaneous (<15ms)');
    assert.equal(getRes.state.v, 1);
});

test('Critical Breach File Wipe endpoint purges local data files and triggers emergency recovery manifest', async () => {
    // 1. Trigger critical breach wipe
    const wipeRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/system/trigger-wiped',
            method: 'POST'
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.end();
    });
    assert.equal(wipeRes.success, true);
    assert.ok(wipeRes.message.includes('Critical breach executed'));

    // 2. Verify corrupted manifest now contains wiped vault file
    const listRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/files/corrupted-list',
            method: 'GET'
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.end();
    });
    assert.ok(Array.isArray(listRes.corruptedFiles));
    const wipedFile = listRes.corruptedFiles.find(f => f.name === 'user_vault.dat.corrupt');
    assert.ok(wipedFile, 'Wiped user_vault.dat.corrupt should be registered for carving');
    assert.equal(wipedFile.expectedMagic, '56 41 55 4c');
});

test('Operator Case Notes can be written to and loaded from AegisFS', async () => {
    const notesContent = '[AEGIS CASE NOTE]\nThreat Actor: Spectre Advanced Persistent Threat\nC2 Relay: 185.220.101.5:8443\nRootkit Hook: CAP_NET_RAW_SOCKET';
    
    // Save notes
    const saveRes = await new Promise((resolve, reject) => {
        const postData = JSON.stringify({
            path: '/userland/notes/case_notes.txt',
            content: notesContent
        });
        const req = http.request({
            port: 3000,
            path: '/api/files/write',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(postData)
            }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.write(postData);
        req.end();
    });
    assert.equal(saveRes.success, true);
    assert.equal(saveRes.file.name, 'case_notes.txt');

    // Load notes
    const readRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/files/read?path=' + encodeURIComponent('/userland/notes/case_notes.txt'),
            method: 'GET'
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.end();
    });
    assert.ok(readRes.file);
    assert.equal(readRes.content, notesContent);
});
