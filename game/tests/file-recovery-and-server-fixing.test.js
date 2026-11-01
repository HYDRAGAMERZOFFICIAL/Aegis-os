'use strict';

const test = require('node:test');
const assert = require('node:assert/strict');
const http = require('node:http');

test('File recovery & carving API returns corrupted files and recovers clean inode with valid magic bytes', async () => {
    // Test GET /api/files/corrupted-list
    await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/files/corrupted-list',
            method: 'GET'
        }, (res) => {
            let body = '';
            res.on('data', chunk => body += chunk);
            res.on('end', () => {
                const data = JSON.parse(body);
                assert.ok(Array.isArray(data.corruptedFiles), 'corruptedFiles should be an array');
                assert.ok(data.corruptedFiles.length >= 3, 'Should have at least 3 corrupted file candidates');
                
                const auditFile = data.corruptedFiles.find(f => f.name === 'financial_audit.db.corrupt');
                assert.ok(auditFile, 'financial_audit.db.corrupt should exist in corrupted manifest');
                assert.equal(auditFile.damageType, 'HEADER_CORRUPTED');
                assert.ok(['UNRECOVERED', 'RECOVERED'].includes(auditFile.status));
                resolve();
            });
        });
        req.on('error', reject);
        req.end();
    });

    // Test POST /api/files/recover with valid magic bytes
    await new Promise((resolve, reject) => {
        const postData = JSON.stringify({
            fileId: 'corrupt-1',
            hexOrKey: '41 45 47 49'
        });
        const req = http.request({
            port: 3000,
            path: '/api/files/recover',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(postData)
            }
        }, (res) => {
            let body = '';
            res.on('data', chunk => body += chunk);
            res.on('end', () => {
                const data = JSON.parse(body);
                assert.equal(res.statusCode, 200);
                assert.equal(data.success, true);
                assert.ok(data.message.includes('financial_audit.db'));
                assert.ok(data.recoveredFile);
                assert.equal(data.recoveredFile.name, 'financial_audit.db');
                resolve();
            });
        });
        req.on('error', reject);
        req.write(postData);
        req.end();
    });
});

test('Server fixing API identifies rogue port conflict and restores degraded service', async () => {
    // Test GET /api/troubleshoot/services
    await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/troubleshoot/services',
            method: 'GET'
        }, (res) => {
            let body = '';
            res.on('data', chunk => body += chunk);
            res.on('end', () => {
                const data = JSON.parse(body);
                assert.ok(Array.isArray(data.services));
                const apiGw = data.services.find(s => s.id === 'svc-apigateway');
                assert.ok(apiGw);
                assert.equal(apiGw.port, 8443);
                assert.ok(['PORT_CONFLICT', 'ACTIVE'].includes(apiGw.status));
                resolve();
            });
        });
        req.on('error', reject);
        req.end();
    });

    // Test POST /api/troubleshoot/fix-service (kill-rogue-pid)
    await new Promise((resolve, reject) => {
        const postData = JSON.stringify({
            serviceId: 'svc-apigateway',
            action: 'kill-rogue-pid'
        });
        const req = http.request({
            port: 3000,
            path: '/api/troubleshoot/fix-service',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(postData)
            }
        }, (res) => {
            let body = '';
            res.on('data', chunk => body += chunk);
            res.on('end', () => {
                const data = JSON.parse(body);
                assert.equal(res.statusCode, 200);
                assert.equal(data.success, true);
                assert.equal(data.service.roguePid, null);
                resolve();
            });
        });
        req.on('error', reject);
        req.write(postData);
        req.end();
    });

    // Test POST /api/troubleshoot/fix-service (restart-service)
    await new Promise((resolve, reject) => {
        const postData = JSON.stringify({
            serviceId: 'svc-apigateway',
            action: 'restart-service'
        });
        const req = http.request({
            port: 3000,
            path: '/api/troubleshoot/fix-service',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(postData)
            }
        }, (res) => {
            let body = '';
            res.on('data', chunk => body += chunk);
            res.on('end', () => {
                const data = JSON.parse(body);
                assert.equal(res.statusCode, 200);
                assert.equal(data.success, true);
                assert.equal(data.service.status, 'ACTIVE');
                resolve();
            });
        });
        req.on('error', reject);
        req.write(postData);
        req.end();
    });
});

test('Zero-Trust capability token registry audits anomalous grants and revokes them', async () => {
    // Test GET /api/shield/tokens
    await new Promise((resolve, reject) => {
        const req = http.request({
            port: 3000,
            path: '/api/shield/tokens',
            method: 'GET'
        }, (res) => {
            let body = '';
            res.on('data', chunk => body += chunk);
            res.on('end', () => {
                const data = JSON.parse(body);
                assert.ok(Array.isArray(data.tokens));
                const escToken = data.tokens.find(t => t.id === 'TOK-KERNEL-ESCALATED');
                assert.ok(escToken);
                assert.equal(escToken.subject, 'kernel_sync_worker');
                assert.equal(escToken.cap, 'CAP_NET_RAW_SOCKET');
                resolve();
            });
        });
        req.on('error', reject);
        req.end();
    });

    // Test POST /api/shield/revoke-token
    await new Promise((resolve, reject) => {
        const postData = JSON.stringify({
            tokenId: 'TOK-KERNEL-ESCALATED'
        });
        const req = http.request({
            port: 3000,
            path: '/api/shield/revoke-token',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': Buffer.byteLength(postData)
            }
        }, (res) => {
            let body = '';
            res.on('data', chunk => body += chunk);
            res.on('end', () => {
                const data = JSON.parse(body);
                assert.equal(res.statusCode, 200);
                assert.equal(data.success, true);
                assert.equal(data.token.status, 'REVOKED');
                resolve();
            });
        });
        req.on('error', reject);
        req.write(postData);
        req.end();
    });
});

test('Shell terminal commands execute file carving, server triage and token revocation', async () => {
    const runCmd = async (command) => {
        return new Promise((resolve, reject) => {
            const postData = JSON.stringify({ command });
            const req = http.request({
                port: 3000,
                path: '/api/terminal/exec',
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Content-Length': Buffer.byteLength(postData)
                }
            }, (res) => {
                let body = '';
                res.on('data', chunk => body += chunk);
                res.on('end', () => {
                    const data = JSON.parse(body);
                    resolve(data.output);
                });
            });
            req.on('error', reject);
            req.write(postData);
            req.end();
        });
    };

    const hexOut = await runCmd('inspect-hex financial_audit.db.corrupt');
    assert.ok(hexOut.includes('HEX DUMP & CORRUPTION ANALYSIS'));
    assert.ok(hexOut.includes('00 00 00 00'));

    const carveOut = await runCmd('carve-file financial_audit.db.corrupt 41 45 47 49');
    assert.ok(carveOut.includes('carved and restored'));

    const killOut = await runCmd('kill 4819');
    assert.ok(killOut.includes('PID 4819'));

    const patchOut = await runCmd('patch-config aegis-edgerouter');
    assert.ok(patchOut.includes('patched successfully'));

    const restartOut = await runCmd('service-restart aegis-apigateway');
    assert.ok(restartOut.includes('ACTIVE (200 OK)'));

    const revokeOut = await runCmd('revoke-token TOK-KERNEL-ESCALATED');
    assert.ok(revokeOut.includes('REVOKED'));
});
