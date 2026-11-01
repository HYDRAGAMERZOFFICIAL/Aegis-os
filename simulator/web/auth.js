'use strict';
(() => {
  const $ = (selector) => document.querySelector(selector);
  const gate = $('#auth-gate');
  const boot = $('#boot-screen');
  const authScreen = $('#auth-screen');
  const lockScreen = $('#lock-screen');
  const app = $('#game-app');
  let currentUser = null;
  let authMode = 'login';
  let storageMode = 'local';

  const request = async (path, body) => {
    const response = await fetch(path, {
      method: body ? 'POST' : 'GET',
      credentials: 'same-origin',
      headers: body ? { 'Content-Type': 'application/json' } : {},
      body: body ? JSON.stringify(body) : undefined
    });
    const data = await response.json().catch(() => ({}));
    if (!response.ok) throw new Error(data.error || 'Authentication request failed.');
    return data;
  };

  const initials = (name) => String(name || 'OP').trim().split(/\s+/).slice(0, 2).map(part => part[0] || '').join('').toUpperCase();
  const showOnly = (screen) => {
    [boot, authScreen, lockScreen].forEach(item => item.classList.add('hidden'));
    if (screen) screen.classList.remove('hidden');
    gate.hidden = !screen;
  };
  const enterGame = (user) => {
    currentUser = user;
    $('#operator-name').textContent = user.displayName || user.username || 'Operator';
    $('#operator-avatar').textContent = initials(user.displayName || user.username);
    $('#lock-display-name').textContent = user.displayName || user.username || 'Operator';
    $('#lock-username').textContent = '@' + (user.username || 'operator');
    $('#lock-avatar').textContent = initials(user.displayName || user.username);
    app.hidden = false;
    showOnly(null);
    window.dispatchEvent(new CustomEvent('aegis:authenticated', { detail: { user, storageMode } }));
  };
  const showAuth = (needsProfile) => {
    app.hidden = true;
    showOnly(authScreen);
    setMode(needsProfile ? 'register' : 'login');
  };
  const setMode = (mode) => {
    authMode = mode;
    const registering = mode === 'register';
    $('#login-form').classList.toggle('hidden', registering);
    $('#register-form').classList.toggle('hidden', !registering);
    $('#auth-title').textContent = registering ? 'Create your operator profile.' : 'Welcome back, Operator.';
    $('#auth-description').textContent = registering
      ? 'Create your Aegis identity. Your campaign progress will follow your account across devices.'
      : 'Authenticate to restore your cloud campaign and return to operations.';
    $('#auth-switch-copy').textContent = registering ? 'Already have a profile?' : 'New to Aegis?';
    $('#auth-switch').textContent = registering ? 'Sign in' : 'Create player profile';
    $('#login-error').textContent = '';
    $('#register-error').textContent = '';
  };
  const setBusy = (form, busy) => {
    const button = form.querySelector('button[type="submit"]');
    button.disabled = busy;
    button.textContent = busy ? 'VERIFYING...' : (form.id === 'login-form' ? 'Authenticate →' : form.id === 'register-form' ? 'Create player profile →' : 'Unlock session →');
  };
  const values = (form) => Object.fromEntries(new FormData(form).entries());

  const bootSequence = async () => {
    const stages = [
      [8, 'INITIALISING SECURE KERNEL'],
      [24, 'VERIFYING AEGIS CORE'],
      [43, 'CONNECTING PLAYER IDENTITY'],
      [67, 'RESTORING CLOUD CAMPAIGN'],
      [86, 'STARTING ZERO-TRUST SESSION'],
      [100, 'SYSTEM READY']
    ];
    for (const [progress, label] of stages) {
      $('#boot-progress-fill').style.width = progress + '%';
      $('#boot-percent').textContent = String(progress).padStart(2, '0') + '%';
      $('#boot-status').textContent = label;
      await new Promise(resolve => setTimeout(resolve, 220));
    }
    try {
      const session = await request('/api/auth/me');
      storageMode = session.storageMode || 'local';
      if (session.authenticated) enterGame(session.user);
      else showAuth(!!session.needsProfile);
    } catch (error) {
      $('#boot-status').textContent = 'CLOUD AUTHENTICATION UNAVAILABLE';
      showAuth(false);
      $('#login-error').textContent = error.message;
    }
  };

  $('#auth-switch').addEventListener('click', () => setMode(authMode === 'login' ? 'register' : 'login'));

  $('#login-form').addEventListener('submit', async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    setBusy(form, true);
    $('#login-error').textContent = '';
    try {
      const result = await request('/api/auth/login', values(form));
      form.reset();
      enterGame(result.user);
    } catch (error) {
      $('#login-error').textContent = error.message;
    } finally { setBusy(form, false); }
  });

  $('#register-form').addEventListener('submit', async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    setBusy(form, true);
    $('#register-error').textContent = '';
    try {
      const result = await request('/api/auth/register', values(form));
      form.reset();
      if (result.confirmationRequired) {
        setMode('login');
        $('#login-error').textContent = result.message || 'Verify your email, then sign in.';
        return;
      }
      enterGame(result.user);
    } catch (error) {
      $('#register-error').textContent = error.message;
    } finally { setBusy(form, false); }
  });

  $('#lock-session').addEventListener('click', () => {
    if (!currentUser) return;
    $('#unlock-error').textContent = '';
    $('#unlock-form').reset();
    app.hidden = true;
    showOnly(lockScreen);
    $('#unlock-form input[name="password"]').focus();
  });

  $('#unlock-form').addEventListener('submit', async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    setBusy(form, true);
    $('#unlock-error').textContent = '';
    try {
      await request('/api/auth/unlock', values(form));
      form.reset();
      enterGame(currentUser);
    } catch (error) {
      $('#unlock-error').textContent = error.message;
    } finally { setBusy(form, false); }
  });

  $('#logout').addEventListener('click', async () => {
    try { await request('/api/auth/logout', {}); }
    catch (error) { $('#unlock-error').textContent = error.message; return; }
    currentUser = null;
    showAuth(false);
  });

  window.AegisAuth = {
    get user() { return currentUser; },
    get storageMode() { return storageMode; },
    lock() { $('#lock-session').click(); }
  };

  bootSequence();
})();
