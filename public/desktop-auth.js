'use strict';
(() => {
  const $ = (selector) => document.querySelector(selector);
  const root = $('#aegis-root');
  const gate = $('#aegis-auth-gate');
  const loginForm = $('#aegis-login-form');
  const registerForm = $('#aegis-register-form');
  let mode = 'login';
  let user = null;

  // Universal Custom Cyberpunk Dialog
  window.AegisDialog = {
    alert(message, title = 'SYSTEM NOTIFICATION') {
      return new Promise((resolve) => {
        const modal = $('#aegis-dialog-modal');
        const titleEl = $('#aegis-dialog-title');
        const msgEl = $('#aegis-dialog-message');
        const cancelBtn = $('#aegis-dialog-cancel');
        const confirmBtn = $('#aegis-dialog-confirm');
        const closeBtn = $('#aegis-dialog-close');

        if (!modal) {
          resolve(true);
          return;
        }

        if (titleEl) titleEl.textContent = title;
        if (msgEl) msgEl.textContent = message;
        if (cancelBtn) cancelBtn.classList.add('hidden');
        if (confirmBtn) confirmBtn.textContent = 'ACKNOWLEDGE (OK)';
        modal.classList.remove('hidden');

        const onDone = () => {
          modal.classList.add('hidden');
          confirmBtn?.removeEventListener('click', onDone);
          closeBtn?.removeEventListener('click', onDone);
          resolve(true);
        };

        confirmBtn?.addEventListener('click', onDone);
        closeBtn?.addEventListener('click', onDone);
      });
    },

    confirm(message, title = 'SECURITY CONFIRMATION') {
      return new Promise((resolve) => {
        const modal = $('#aegis-dialog-modal');
        const titleEl = $('#aegis-dialog-title');
        const msgEl = $('#aegis-dialog-message');
        const cancelBtn = $('#aegis-dialog-cancel');
        const confirmBtn = $('#aegis-dialog-confirm');
        const closeBtn = $('#aegis-dialog-close');

        if (!modal) {
          resolve(true);
          return;
        }

        if (titleEl) titleEl.textContent = title;
        if (msgEl) msgEl.textContent = message;
        if (cancelBtn) {
          cancelBtn.classList.remove('hidden');
          cancelBtn.textContent = 'CANCEL';
        }
        if (confirmBtn) confirmBtn.textContent = 'CONFIRM ACTION';
        modal.classList.remove('hidden');

        const onConfirm = () => {
          modal.classList.add('hidden');
          cleanup();
          resolve(true);
        };

        const onCancel = () => {
          modal.classList.add('hidden');
          cleanup();
          resolve(false);
        };

        const cleanup = () => {
          confirmBtn?.removeEventListener('click', onConfirm);
          cancelBtn?.removeEventListener('click', onCancel);
          closeBtn?.removeEventListener('click', onCancel);
        };

        confirmBtn?.addEventListener('click', onConfirm);
        cancelBtn?.addEventListener('click', onCancel);
        closeBtn?.addEventListener('click', onCancel);
      });
    }
  };

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

  const enter = (account, shouldAnimateBoot = false) => {
    user = account;
    gate.hidden = true;

    const bootScreen = $('#aegis-boot-screen');
    const bootConsole = $('#boot-console');
    const bootBar = $('#boot-bar-fill');
    const bootStatus = $('#boot-status');

    const finalizeDesktop = () => {
      root.classList.remove('aegis-locked');
      if (bootScreen) bootScreen.classList.add('hidden');

      let badge = $('#aegis-operator-badge');
      if (!badge) {
        badge = document.createElement('button');
        badge.id = 'aegis-operator-badge';
        badge.type = 'button';
        badge.className = 'aegis-operator-badge';
        $('#top-bar .top-right')?.appendChild(badge);
        badge.addEventListener('click', async () => {
          const ok = await window.AegisDialog.confirm('Sign out of this AegisOS operator profile?', 'OPERATOR SIGN-OUT');
          if (!ok) return;
          try {
            await request('/api/auth/logout', {});
          } catch (error) {
            await window.AegisDialog.alert(error.message, 'SIGN-OUT ERROR');
            return;
          }
          user = null;
          root.classList.add('aegis-locked');
          gate.hidden = false;
          showMode('login');
        });
      }
      badge.textContent = 'OPERATOR // ' + (account.displayName || account.username) + '  ↗';
      window.AegisAuth = { get user() { return user; }, logout: () => badge.click() };
      document.dispatchEvent(new CustomEvent('aegis:authenticated', { detail: { user: account } }));
    };

    if (shouldAnimateBoot && bootScreen && bootConsole && bootBar) {
      bootScreen.classList.remove('hidden');
      bootBar.style.width = '0%';
      bootConsole.innerHTML = '<div class="boot-line">&gt; [  0.000000] Initializing Dual-Kernel Hybrid Architecture v2.0-Alpha...</div>';

      const bootLines = [
        '[  0.035000] Loading Microkernel ASLR & Page Table Isolation...',
        '[  0.082000] Mounting AegisDB In-Memory Relational Engine...',
        '[  0.145000] Initializing High-Speed Memory Cache Subsystem (100% Hit Target)...',
        '[  0.210000] Establishing Zero-Trust Capability Token Registry...',
        '[  0.280000] Mounting Cryptographic Merkle Root Filesystem (Self-Healing)...',
        '[  0.355000] Welcome to your virtual system, Operator ' + (account.displayName || account.username).toUpperCase() + '!'
      ];

      let idx = 0;
      const interval = setInterval(() => {
        if (idx < bootLines.length) {
          const div = document.createElement('div');
          div.className = 'boot-line';
          div.textContent = '> ' + bootLines[idx];
          bootConsole.appendChild(div);
          bootConsole.scrollTop = bootConsole.scrollHeight;
          const pct = Math.round(((idx + 1) / bootLines.length) * 100);
          bootBar.style.width = pct + '%';
          if (bootStatus) bootStatus.textContent = 'INITIALIZING SYSTEM // ' + pct + '%';
          idx++;
        } else {
          clearInterval(interval);
          setTimeout(finalizeDesktop, 350);
        }
      }, 150);
    } else {
      finalizeDesktop();
    }
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
      enter(result.user, true); // Trigger boot screen animation on login and signup!
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
      if (session.authenticated) enter(session.user, false);
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
