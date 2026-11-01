'use strict';

const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

const SESSION_TTL_MS = 7 * 24 * 60 * 60 * 1000;
const KEY_LENGTH = 64;

class AuthManager {
  constructor(directory) {
    this.directory = path.resolve(directory);
    this.accountFile = path.join(this.directory, 'accounts.json');
    this.sessions = new Map();
    this.failedAttempts = new Map();
    fs.mkdirSync(this.directory, { recursive: true });
    this.accounts = this._readAccounts();
  }

  _readAccounts() {
    try {
      const parsed = JSON.parse(fs.readFileSync(this.accountFile, 'utf8'));
      return Array.isArray(parsed.accounts) ? parsed.accounts : [];
    } catch (error) {
      if (error.code === 'ENOENT') return [];
      throw new Error('Player account store is unreadable.');
    }
  }

  _persist() {
    const temporary = this.accountFile + '.tmp';
    fs.writeFileSync(temporary, JSON.stringify({ version: 1, accounts: this.accounts }, null, 2), { encoding: 'utf8', mode: 0o600 });
    fs.renameSync(temporary, this.accountFile);
  }

  hasAccounts() { return this.accounts.length > 0; }

  _publicUser(account) {
    return { id: account.id, username: account.username, displayName: account.displayName, createdAt: account.createdAt };
  }

  _derive(password, salt) {
    return crypto.scryptSync(password, salt, KEY_LENGTH, { N: 16384, r: 8, p: 1, maxmem: 64 * 1024 * 1024 }).toString('hex');
  }

  _verify(account, password) {
    const actual = Buffer.from(this._derive(password, account.salt), 'hex');
    const expected = Buffer.from(account.passwordHash, 'hex');
    return actual.length === expected.length && crypto.timingSafeEqual(actual, expected);
  }

  _rateLimitKey(username, address) { return String(address || 'local') + ':' + String(username || '').toLowerCase(); }

  _checkRateLimit(username, address) {
    const key = this._rateLimitKey(username, address);
    const now = Date.now();
    const entry = this.failedAttempts.get(key);
    if (entry && entry.blockedUntil > now) throw new Error('Too many attempts. Wait a few minutes and try again.');
    return key;
  }

  _recordFailure(key) {
    const now = Date.now();
    const entry = this.failedAttempts.get(key) || { count: 0, blockedUntil: 0 };
    entry.count += 1;
    if (entry.count >= 8) { entry.blockedUntil = now + 5 * 60 * 1000; entry.count = 0; }
    this.failedAttempts.set(key, entry);
  }

  createAccount(input, address) {
    const username = String(input.username || '').trim().toLowerCase();
    const displayName = String(input.displayName || '').trim();
    const password = String(input.password || '');
    if (!/^[a-z0-9_-]{3,20}$/.test(username)) throw new Error('Username must be 3–20 characters using letters, numbers, _ or -.');
    if (displayName.length < 2 || displayName.length > 32) throw new Error('Display name must be 2–32 characters.');
    if (password.length < 10 || password.length > 128) throw new Error('Password must be 10–128 characters.');
    if (this.accounts.some(account => account.username === username)) throw new Error('That username is already registered.');
    const salt = crypto.randomBytes(16).toString('hex');
    const account = {
      id: crypto.randomBytes(12).toString('hex'),
      username,
      displayName,
      salt,
      passwordHash: this._derive(password, salt),
      createdAt: new Date().toISOString()
    };
    this.accounts.push(account);
    this._persist();
    return { user: this._publicUser(account), token: this._newSession(account, address) };
  }

  login(input, address) {
    const username = String(input.username || '').trim().toLowerCase();
    const password = String(input.password || '');
    const key = this._checkRateLimit(username, address);
    const account = this.accounts.find(item => item.username === username);
    // A fixed dummy derivation keeps unknown usernames from being an instant timing oracle.
    const valid = account ? this._verify(account, password) : this._derive(password || 'invalid-password', 'aegis-dummy-salt');
    if (!account || !valid) {
      this._recordFailure(key);
      throw new Error('Username or password is incorrect.');
    }
    this.failedAttempts.delete(key);
    return { user: this._publicUser(account), token: this._newSession(account, address) };
  }

  verifyPassword(userId, password) {
    const account = this.accounts.find(item => item.id === userId);
    return !!account && typeof password === 'string' && this._verify(account, password);
  }

  _newSession(account, address) {
    const token = crypto.randomBytes(32).toString('hex');
    this.sessions.set(token, { user: this._publicUser(account), address: String(address || ''), expiresAt: Date.now() + SESSION_TTL_MS });
    return token;
  }

  getSession(req) {
    const header = req.headers.cookie || '';
    const match = header.match(/(?:^|;\s*)aegis_session=([a-f0-9]{64})(?:;|$)/);
    if (!match) return null;
    const session = this.sessions.get(match[1]);
    if (!session) return null;
    if (session.expiresAt <= Date.now()) { this.sessions.delete(match[1]); return null; }
    session.expiresAt = Date.now() + SESSION_TTL_MS;
    return { token: match[1], user: session.user };
  }

  logout(req) {
    const session = this.getSession(req);
    if (session) this.sessions.delete(session.token);
  }

  unlock(userId, password) { return this.verifyPassword(userId, password); }
}

module.exports = AuthManager;
