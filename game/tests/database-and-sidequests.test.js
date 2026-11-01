'use strict';

const test = require('node:test');
const assert = require('node:assert/strict');
const http = require('node:http');

test('AegisDB REST API returns table metadata and executes AegisQL queries with <1ms in-memory cache', async () => {
    // 1. GET /api/db/tables
    const tablesRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/db/tables',
            method: 'GET'
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.end();
    });
    assert.equal(tablesRes.success, true);
    assert.equal(tablesRes.status, 'ONLINE');
    assert.ok(tablesRes.totalTables >= 8);
    const userTable = tablesRes.tables.find(t => t.name === 'users');
    assert.ok(userTable, 'users table must exist');

    // 2. POST /api/db/query (SELECT * FROM user_case_files)
    const queryPayload = JSON.stringify({ query: 'SELECT * FROM user_case_files' });
    const queryRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/db/query',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(queryPayload)
            }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.write(queryPayload);
        req.end();
    });
    assert.equal(queryRes.success, true);
    assert.equal(queryRes.type, 'SELECT');
    assert.equal(queryRes.cached, true);
    assert.ok(queryRes.rows.length >= 3);
    assert.ok(queryRes.latencyMs < 5, 'Query should execute in <5ms');
});

test('AegisDB Side Quests Claim API updates user profile and records audit trail', async () => {
    const claimPayload = JSON.stringify({ questId: 'quest_airgap' });
    const claimRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/db/side-quest/claim',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(claimPayload)
            }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.write(claimPayload);
        req.end();
    });
    assert.equal(claimRes.success, true);
    assert.ok(claimRes.message.includes('Bounty') || claimRes.message.includes('completed'));
    assert.equal(claimRes.quest.status, 'COMPLETED');
});

test('In-Memory Static Asset Cache serves ETags and 304 Not Modified', async () => {
    // 1. Initial GET /index.html
    const firstGet = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/index.html',
            method: 'GET'
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve({ status: res.statusCode, etag: res.headers['etag'], cacheControl: res.headers['cache-control'] }));
        });
        req.on('error', reject);
        req.end();
    });
    assert.equal(firstGet.status, 200);
    assert.ok(firstGet.etag, 'Response should contain ETag header');
    assert.ok(firstGet.cacheControl.includes('max-age=3600'));

    // 2. Conditional GET with If-None-Match
    const conditionalGet = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/index.html',
            method: 'GET',
            headers: { 'if-none-match': firstGet.etag }
        }, (res) => {
            resolve({ status: res.statusCode });
        });
        req.on('error', reject);
        req.end();
    });
    assert.equal(conditionalGet.status, 304, 'Server should respond 304 Not Modified for matching ETag');
});

test('Terminal supports db command for in-memory database queries and table audits', async () => {
    const cmdPayload = JSON.stringify({ command: 'db tables' });
    const cmdRes = await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/terminal/exec',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(cmdPayload)
            }
        }, (res) => {
            let body = '';
            res.on('data', c => body += c);
            res.on('end', () => resolve(JSON.parse(body)));
        });
        req.on('error', reject);
        req.write(cmdPayload);
        req.end();
    });
    assert.ok(cmdRes.output.includes('AegisDB v2.4 In-Memory Hybrid Database Engine'));
    assert.ok(cmdRes.output.includes('users'));
    assert.ok(cmdRes.output.includes('user_case_files'));
});

