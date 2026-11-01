'use strict';

const GameManager = require('../game/core/GameManager');

const COOKIE_ACCESS = 'aegis_access';
const COOKIE_REFRESH = 'aegis_refresh';
const MAX_BODY = 64 * 1024;

function send(res, status, payload, headers = {}) {
  res.statusCode = status;
  res.setHeader('Content-Type', 'application/json; charset=utf-8');
  res.setHeader('Cache-Control', 'no-store');
  for (const [key, value] of Object.entries(headers)) res.setHeader(key, value);
  res.end(JSON.stringify(payload));
}

function cookies(req) {
  return String(req.headers.cookie || '').split(';').reduce((result, part) => {
    const index = part.indexOf('=');
    if (index > 0) result[part.slice(0, index).trim()] = decodeURIComponent(part.slice(index + 1).trim());
    return result;
  }, {});
}

function cookieHeader(name, value, maxAge) {
  const secure = process.env.NODE_ENV === 'production' || process.env.VERCEL === '1';
  return name + '=' + encodeURIComponent(value || '') + '; Path=/; HttpOnly; SameSite=Lax; Max-Age=' + maxAge + (secure ? '; Secure' : '');
}

function setSessionCookies(res, session) {
  const maxAge = Math.max(60, Math.min(Number(session.expires_in) || 3600, 604800));
  res.setHeader('Set-Cookie', [
    cookieHeader(COOKIE_ACCESS, session.access_token, maxAge),
    cookieHeader(COOKIE_REFRESH, session.refresh_token, 60 * 60 * 24 * 30)
  ]);
}

function clearSessionCookies(res) {
  const secure = process.env.NODE_ENV === 'production' || process.env.VERCEL === '1';
  const suffix = '; Path=/; HttpOnly; SameSite=Lax; Max-Age=0' + (secure ? '; Secure' : '');
  res.setHeader('Set-Cookie', [COOKIE_ACCESS + '=' + suffix, COOKIE_REFRESH + '=' + suffix]);
}

function config() {
  const url = String(process.env.SUPABASE_URL || '').replace(/\/$/, '');
  const key = process.env.SUPABASE_ANON_KEY || process.env.SUPABASE_PUBLISHABLE_KEY;
  if (!url || !key) throw new Error('Cloud account service is not configured. Add SUPABASE_URL and SUPABASE_ANON_KEY to the Vercel environment.');
  return { url, key };
}

async function supabase(path, options = {}) {
  const cfg = config();
  const response = await fetch(cfg.url + path, {
    method: options.method || 'GET',
    headers: {
      apikey: cfg.key,
      Authorization: 'Bearer ' + (options.token || cfg.key),
      ...(options.body ? { 'Content-Type': 'application/json' } : {}),
      ...(options.headers || {})
    },
    body: options.body ? JSON.stringify(options.body) : undefined
  });
  const raw = await response.text();
  let data = {};
  try { data = raw ? JSON.parse(raw) : {}; } catch (_) { data = { message: raw.slice(0, 300) }; }
  if (!response.ok) {
    const message = data.msg || data.message || data.error_description || data.error || 'Cloud service request failed.';
    const error = new Error(message);
    error.status = response.status;
    throw error;
  }
  return data;
}

async function readBody(req) {
  if (req.body && typeof req.body === 'object') return req.body;
  if (typeof req.body === 'string') return JSON.parse(req.body || '{}');
  let raw = '';
  for await (const chunk of req) {
    raw += chunk;
    if (raw.length > MAX_BODY) throw new Error('Request body is too large.');
  }
  return raw ? JSON.parse(raw) : {};
}

async function profileFor(user, token) {
  const rows = await supabase('/rest/v1/player_profiles?select=id,username,display_name,created_at&id=eq.' + encodeURIComponent(user.id) + '&limit=1', { token });
  const profile = rows[0] || {};
  return {
    id: user.id,
    email: user.email,
    username: profile.username || user.user_metadata?.username || 'operator',
    displayName: profile.display_name || user.user_metadata?.display_name || user.email?.split('@')[0] || 'Operator',
    createdAt: profile.created_at || user.created_at
  };
}

async function currentSession(req, res) {
  const jar = cookies(req);
  const access = jar[COOKIE_ACCESS];
  const refresh = jar[COOKIE_REFRESH];
  if (access) {
    try {
      const user = await supabase('/auth/v1/user', { token: access });
      return { user, access };
    } catch (_) {
      // Expired access tokens are renewed below using the HttpOnly refresh cookie.
    }
  }
  if (!refresh) return null;
  try {
    const session = await supabase('/auth/v1/token?grant_type=refresh_token', { method: 'POST', body: { refresh_token: refresh } });
    if (!session.access_token || !session.refresh_token) return null;
    setSessionCookies(res, session);
    return { user: session.user, access: session.access_token };
  } catch (_) {
    clearSessionCookies(res);
    return null;
  }
}

async function readSave(user, token) {
  const rows = await supabase('/rest/v1/player_game_saves?select=save_data,updated_at&user_id=eq.' + encodeURIComponent(user.id) + '&limit=1', { token });
  return rows[0] || null;
}

async function writeSave(user, token, game) {
  const payload = {
    user_id: user.id,
    save_data: { schemaVersion: 1, game: game.serialize() },
    updated_at: new Date().toISOString()
  };
  await supabase('/rest/v1/player_game_saves?on_conflict=user_id', {
    method: 'POST',
    token,
    body: payload,
    headers: { Prefer: 'resolution=merge-duplicates,return=minimal' }
  });
  return payload.updated_at;
}

async function gameFor(user, token) {
  const game = new GameManager();
  const saved = await readSave(user, token);
  if (saved?.save_data?.game) game.restore(saved.save_data.game);
  return { game, saved };
}

function validateRegistration(data) {
  const displayName = String(data.displayName || '').trim();
  const username = String(data.username || '').trim().toLowerCase();
  const email = String(data.email || data.username || '').trim().toLowerCase();
  const password = String(data.password || '');
  if (!/^[a-z0-9_-]{3,20}$/.test(username)) throw new Error('Operator ID must be 3–20 characters using letters, numbers, _ or -.');
  if (displayName.length < 2 || displayName.length > 32) throw new Error('Display name must be 2–32 characters.');
  if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email) || email.length > 254) throw new Error('Enter a valid email address.');
  if (password.length < 10 || password.length > 128) throw new Error('Password must be 10–128 characters.');
  return { displayName, username, email, password };
}

module.exports = async function handler(req, res) {
  try {
    const pathname = new URL(req.url, 'https://aegis.invalid').pathname.replace(/\/$/, '') || '/';
    if (req.method === 'OPTIONS') return send(res, 204, {});
    if (!pathname.startsWith('/api/')) return send(res, 404, { error: 'API route not found.' });

    if (pathname === '/api/auth/me' && req.method === 'GET') {
      const session = await currentSession(req, res);
      if (!session) return send(res, 200, { authenticated: false, user: null, needsProfile: false, storageMode: 'cloud' });
      const user = await profileFor(session.user, session.access);
      return send(res, 200, { authenticated: true, user, storageMode: 'cloud' });
    }

    if (pathname === '/api/auth/register' && req.method === 'POST') {
      const data = validateRegistration(await readBody(req));
      const result = await supabase('/auth/v1/signup', {
        method: 'POST',
        body: {
          email: data.email,
          password: data.password,
          data: { username: data.username, display_name: data.displayName }
        }
      });
      if (!result.session?.access_token) {
        return send(res, 202, { success: true, confirmationRequired: true, message: 'Check your email to verify your account, then sign in to AegisOS.' });
      }
      setSessionCookies(res, result.session);
      const user = await profileFor(result.user, result.session.access_token);
      return send(res, 201, { success: true, user });
    }

    if (pathname === '/api/auth/login' && req.method === 'POST') {
      const data = await readBody(req);
      const email = String(data.email || data.username || '').trim().toLowerCase();
      const password = String(data.password || '');
      if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email) || password.length < 1 || password.length > 128) {
        return send(res, 400, { error: 'Enter your registered email and password.' });
      }
      const session = await supabase('/auth/v1/token?grant_type=password', { method: 'POST', body: { email, password } });
      setSessionCookies(res, session);
      const user = await profileFor(session.user, session.access_token);
      return send(res, 200, { success: true, user });
    }

    if (pathname === '/api/auth/unlock' && req.method === 'POST') {
      const session = await currentSession(req, res);
      if (!session) return send(res, 401, { error: 'Your player session expired. Sign in again.' });
      const data = await readBody(req);
      const password = String(data.password || '');
      if (!password || password.length > 128) return send(res, 400, { error: 'Enter your password to unlock.' });
      await supabase('/auth/v1/token?grant_type=password', { method: 'POST', body: { email: session.user.email, password } });
      return send(res, 200, { success: true });
    }

    if (pathname === '/api/auth/logout' && req.method === 'POST') {
      const session = await currentSession(req, res);
      if (session) {
        try { await supabase('/auth/v1/logout', { method: 'POST', token: session.access }); } catch (_) {}
      }
      clearSessionCookies(res);
      return send(res, 200, { success: true });
    }

    if (pathname.startsWith('/api/game/')) {
      const session = await currentSession(req, res);
      if (!session) return send(res, 401, { error: 'Sign in to access your campaign.' });
      const user = await profileFor(session.user, session.access);
      const { game, saved } = await gameFor(session.user, session.access);

      if (pathname === '/api/game/state' && req.method === 'GET') {
        return send(res, 200, { ...game.getState(), player: user, saveUpdatedAt: saved?.updated_at || null });
      }
      if (pathname === '/api/game/missions' && req.method === 'GET') {
        return send(res, 200, { missions: game.missions.list() });
      }
      if (pathname === '/api/game/start' && req.method === 'POST') {
        const data = await readBody(req);
        const mission = game.startMission(data.missionId || 'mission-01');
        const savedAt = await writeSave(session.user, session.access, game);
        return send(res, 200, { success: true, mission, state: game.getState(), savedAt });
      }
      if (pathname === '/api/game/action' && req.method === 'POST') {
        const data = await readBody(req);
        if (!data || typeof data.action !== 'string') return send(res, 400, { error: 'A valid tactical action is required.' });
        const result = game.performAction(data.action, data.payload || {});
        if (!result.ok) return send(res, 409, result);
        const savedAt = await writeSave(session.user, session.access, game);
        return send(res, 200, { ...result, savedAt });
      }
      if (pathname === '/api/game/research' && req.method === 'POST') {
        const data = await readBody(req);
        if (!data || typeof data.nodeId !== 'string') return send(res, 400, { error: 'Research node ID is required.' });
        const result = game.unlockResearch(data.nodeId);
        if (!result.ok) return send(res, 409, result);
        const savedAt = await writeSave(session.user, session.access, game);
        return send(res, 200, { ...result, state: game.getState(), savedAt });
      }
      if (pathname === '/api/game/save' && req.method === 'POST') {
        const savedAt = await writeSave(session.user, session.access, game);
        return send(res, 200, { success: true, slot: 'cloud-profile', savedAt });
      }
      if (pathname === '/api/game/load' && req.method === 'POST') {
        if (!saved?.save_data?.game) return send(res, 404, { error: 'No cloud campaign save exists yet.' });
        return send(res, 200, { success: true, state: game.getState(), savedAt: saved.updated_at });
      }
    }

    return send(res, 404, { error: 'API route not found.' });
  } catch (error) {
    const status = error.status && error.status >= 400 && error.status < 500 ? error.status : 500;
    const message = status === 500 && !String(error.message || '').includes('SUPABASE_URL')
      ? 'Aegis cloud service is temporarily unavailable. Check your configuration and try again.'
      : error.message;
    return send(res, status, { error: message });
  }
};
