'use strict';
(() => {
  const $ = (selector) => document.querySelector(selector);
  const root = $('#aegis-root');
  const gate = $('#aegis-auth-gate');
  const loginForm = $('#aegis-login-form');
  const registerForm = $('#aegis-register-form');
  let mode = 'login';
  let user = null;

  const request = async (path, body) => {
    const response = await fetch(path, {
      method: body ? 'POST' : 'GET',
      credentials: 'same-origin',
      headers: body ? { 'Content-Type': 'application/json' } : {},
      body: body ? JSON.stringify(body) : undefined
    });
    const data = await response.json().catch(() => ({}));
    if (!response.ok) throw new Error(data.error || 'Authentication service unavailable.');
    return data;
  };
  const showMode = (next) => {
    mode = next;
    loginForm.hidden = mode !== 'login';
    registerForm.hidden = mode !== 'register';
    $('#aegis-auth-title').textContent = mode === 'register' ? 'Create your operator profile.' : 'Authenticate, Operator.';
    $('#aegis-auth-description').textContent = mode === 'register' ? 'Create an identity to begin a persistent AegisOS campaign.' : 'Sign in to restore your personal AegisOS campaign.';
    $('#aegis-auth-switch-copy').textContent = mode === 'register' ? 'Already have a profile?' : 'New to AegisOS?';
    $('#aegis-auth-switch').textContent = mode === 'register' ? 'Sign in' : 'Create player profile';
    $('#aegis-login-error').textContent = '';
    $('#aegis-register-error').textContent = '';
  };
  const enter = (account) => {
    user = account;
    root.classList.remove('aegis-locked');
    gate.hidden = true;
    let badge = $('#aegis-operator-badge');
    if (!badge) {
      badge = document.createElement('button');
      badge.id = 'aegis-operator-badge';
      badge.type = 'button';
      badge.className = 'aegis-operator-badge';
      $('#top-bar .top-right')?.appendChild(badge);
      badge.addEventListener('click', async () => {
        if (!confirm('Sign out of this AegisOS operator profile?')) return;
        try {
          const campaignState = window.AegisCampaign && window.AegisCampaign.state;
          if (campaignState) await request('/api/campaign/state', { state: campaignState });
          await request('/api/auth/logout', {});
        } catch (error) { alert(error.message); return; }
        user = null;
        document.dispatchEvent(new Event('aegis:logged-out'));
        root.classList.add('aegis-locked');
        gate.hidden = false;
        showMode('login');
      });
    }
    badge.textContent = 'OPERATOR // ' + (account.displayName || account.username) + '  ↗';
    window.AegisAuth = { get user() { return user; }, logout: () => badge.click() };
    document.dispatchEvent(new CustomEvent('aegis:authenticated', { detail: { user: account } }));
  };
  const submit = async (form, endpoint, errorSelector) => {
    const button = form.querySelector('button[type="submit"]');
    const label = button.textContent;
    button.disabled = true;
    button.textContent = 'VERIFYING...';
    $(errorSelector).textContent = '';
    try {
      const values = Object.fromEntries(new FormData(form).entries());
      const result = await request(endpoint, values);
      form.reset();
      enter(result.user);
    } catch (error) {
      $(errorSelector).textContent = error.message;
    } finally {
      button.disabled = false;
      button.textContent = label;
    }
  };
  $('#aegis-auth-switch').addEventListener('click', () => showMode(mode === 'login' ? 'register' : 'login'));
  loginForm.addEventListener('submit', (event) => {
    event.preventDefault();
    submit(loginForm, '/api/auth/login', '#aegis-login-error');
  });
  registerForm.addEventListener('submit', (event) => {
    event.preventDefault();
    submit(registerForm, '/api/auth/register', '#aegis-register-error');
  });
  (async () => {
    try {
      const session = await request('/api/auth/me');
      if (session.authenticated) enter(session.user);
      else {
        gate.hidden = false;
        showMode(session.needsProfile ? 'register' : 'login');
      }
    } catch (error) {
      gate.hidden = false;
      showMode('login');
      $('#aegis-login-error').textContent = error.message;
    }
  })();
})();
