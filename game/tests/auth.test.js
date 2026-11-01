'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const AuthManager = require('../auth/AuthManager');

test('player profiles store password hashes and issue an authenticated session', (t) => {
  const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'aegis-auth-'));
  t.after(() => fs.rmSync(directory, { recursive: true, force: true }));
  const auth = new AuthManager(directory);
  const created = auth.createAccount({ username: 'operator_one', displayName: 'Operator One', password: 'correct-horse-42' }, '127.0.0.1');
  assert.equal(created.user.username, 'operator_one');
  assert.equal(auth.hasAccounts(), true);
  const stored = JSON.parse(fs.readFileSync(path.join(directory, 'accounts.json'), 'utf8'));
  assert.equal(stored.accounts[0].passwordHash.includes('correct-horse-42'), false);
  assert.notEqual(stored.accounts[0].passwordHash, 'correct-horse-42');

  const restartedAuth = new AuthManager(directory);
  const signedIn = restartedAuth.login({ username: 'operator_one', password: 'correct-horse-42' }, '127.0.0.1');
  assert.equal(signedIn.user.id, created.user.id);
  const request = { headers: { cookie: 'aegis_session=' + created.token }, socket: { remoteAddress: '127.0.0.1' } };
  assert.equal(auth.getSession(request).user.id, created.user.id);
  assert.equal(auth.unlock(created.user.id, 'correct-horse-42', '127.0.0.1'), true);
  assert.equal(auth.unlock(created.user.id, 'wrong-password', '127.0.0.1'), false);
  auth.logout(request);
  assert.equal(auth.getSession(request), null);
});

test('duplicate profiles and weak passwords are rejected', (t) => {
  const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'aegis-auth-'));
  t.after(() => fs.rmSync(directory, { recursive: true, force: true }));
  const auth = new AuthManager(directory);
  assert.throws(() => auth.createAccount({ username: 'ab', displayName: 'Operator', password: 'short' }), /Username/);
  auth.createAccount({ username: 'operator', displayName: 'Operator', password: 'long-enough-password' });
  assert.throws(() => auth.createAccount({ username: 'operator', displayName: 'Second Operator', password: 'long-enough-password' }), /already registered/);
  assert.throws(() => auth.login({ username: 'operator', password: 'wrong-password' }, '127.0.0.1'), /incorrect/);
});
