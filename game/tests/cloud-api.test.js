'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const handler = require('../../api/[...route].js');

async function request(method, url, headers = {}, body) {
  const responseHeaders = {};
  const res = {
    statusCode: 200,
    setHeader(name, value) { responseHeaders[name.toLowerCase()] = value; },
    end(value) { this.body = value; }
  };
  const req = {
    method,
    url,
    headers,
    body,
    async *[Symbol.asyncIterator]() {}
  };
  await handler(req, res);
  return { statusCode: res.statusCode, headers: responseHeaders, body: JSON.parse(res.body || '{}') };
}

test('cloud auth bootstrap never opens a game session without a player cookie', async () => {
  const result = await request('GET', '/api/auth/me');
  assert.equal(result.statusCode, 200);
  assert.equal(result.body.authenticated, false);
  assert.equal(result.body.storageMode, 'cloud');
});

test('campaign state is protected by mandatory player authentication', async () => {
  const result = await request('GET', '/api/game/state');
  assert.equal(result.statusCode, 401);
  assert.match(result.body.error, /Sign in/);
});

test('campaign actions are protected before the request body can mutate state', async () => {
  const result = await request('POST', '/api/game/action', { 'content-type': 'application/json' }, { action: 'inspect_process' });
  assert.equal(result.statusCode, 401);
  assert.match(result.body.error, /Sign in/);
});

test('unknown cloud API routes fail closed', async () => {
  const result = await request('GET', '/api/admin/export');
  assert.equal(result.statusCode, 404);
});
