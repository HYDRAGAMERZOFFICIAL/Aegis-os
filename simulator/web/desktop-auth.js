'use strict';
(() => {
  const $ = (selector) => document.querySelector(selector);
  const root = $('#aegis-root');
  const gate = $('#aegis-auth-gate');
  const loginForm = $('#aegis-login-form');
  const registerForm = $('#aegis-register-form');
  let mode = 'login';
  let user = null;
  let vaultData = null;

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

  // ----------------------------------------------------
  // OPERATOR DATA VAULT & USER DATABASE SYSTEM
  // ----------------------------------------------------
  const loadUserVault = async () => {
    try {
      const res = await request('/api/user/vault');
      if (res && res.success) {
        vaultData = res.vault;
        if (vaultData && vaultData.notes) {
          try {
            localStorage.setItem('aegis_case_notes', vaultData.notes);
            const textarea = document.querySelector('#notepad-textarea');
            if (textarea && (!textarea.value || textarea.value.trim() === '')) {
              textarea.value = vaultData.notes;
            }
          } catch (_) {}
        }
        return vaultData;
      }
    } catch (err) {
      console.warn('Could not load user database vault:', err);
    }
    return null;
  };

  const syncUserVault = async () => {
    try {
      const textarea = document.querySelector('#notepad-textarea');
      const currentNotes = textarea ? textarea.value : (localStorage.getItem('aegis_case_notes') || '');
      let campaignState = null;
      try {
        const raw = localStorage.getItem('aegis.nativeCampaign.v1');
        if (raw) campaignState = JSON.parse(raw);
      } catch (_) {}

      const syncPayload = {
        notes: currentNotes,
        campaign: campaignState,
        profile: {
          xp: campaignState && campaignState.stats ? campaignState.stats.xp : 0,
          credits: campaignState && campaignState.stats ? campaignState.stats.credits : 0,
          completedMissions: campaignState && campaignState.done ? campaignState.done.length : 0
        }
      };

      const res = await request('/api/user/vault/sync', syncPayload);
      if (res && res.success) {
        await window.AegisDialog.alert(res.message || 'Operator database synchronized successfully.', 'DATABASE SYNC COMPLETE');
        await loadUserVault();
        updateVaultUI();
      }
    } catch (err) {
      await window.AegisDialog.alert(err.message, 'DATABASE SYNC ERROR');
    }
  };

  const updateVaultUI = () => {
    if (!vaultData) return;
    const { user, profile, campaign, notes, auditLogs } = vaultData;

    // Profile Tab
    if ($('#vault-op-name')) $('#vault-op-name').textContent = 'OPERATOR // ' + (user.displayName || user.username).toUpperCase();
    if ($('#vault-op-clearance')) $('#vault-op-clearance').textContent = user.clearanceLevel || 'LEVEL_5_COSMIC';
    if ($('#vault-op-role')) $('#vault-op-role').textContent = user.role || 'SECURITY_DIRECTOR';
    if ($('#vault-op-rank')) $('#vault-op-rank').textContent = profile ? profile.rank : 'Field Specialist';
    if ($('#vault-op-meta')) $('#vault-op-meta').textContent = 'Account provisioned: ' + new Date(user.createdAt || Date.now()).toLocaleDateString() + ' • Status: ACTIVE / ENCLAVE-BACKED';

    const score = campaign && campaign.stats ? campaign.stats.score : (profile ? profile.score || 0 : 0);
    const xp = campaign && campaign.stats ? campaign.stats.xp : (profile ? profile.xp || 0 : 0);
    const credits = campaign && campaign.stats ? campaign.stats.credits : (profile ? profile.credits || 0 : 0);
    const opsDone = campaign && campaign.done ? campaign.done.length : (profile ? profile.completedMissions || 0 : 0);

    if ($('#vault-stat-score')) $('#vault-stat-score').textContent = score;
    if ($('#vault-stat-xp')) $('#vault-stat-xp').textContent = xp;
    if ($('#vault-stat-credits')) $('#vault-stat-credits').textContent = credits;
    if ($('#vault-stat-ops')) $('#vault-stat-ops').textContent = opsDone + ' / 8';

    // Campaign Tab
    const campEl = $('#vault-campaign-details');
    if (campEl) {
      if (campaign) {
        campEl.innerHTML = `
          <div style="padding: 6px 0; border-bottom: 1px solid rgba(255,255,255,0.08);"><strong>Active Theater:</strong> Operation #${campaign.index + 1}</div>
          <div style="padding: 6px 0; border-bottom: 1px solid rgba(255,255,255,0.08);"><strong>Secured Cases:</strong> ${campaign.done && campaign.done.length > 0 ? campaign.done.join(', ') : 'None yet (Active case ongoing)'}</div>
          <div style="padding: 6px 0; border-bottom: 1px solid rgba(255,255,255,0.08);"><strong>Recorded Forensics:</strong> ${campaign.log ? campaign.log.length : 0} events in session log</div>
          <div style="padding: 6px 0;"><strong>Earned Achievements:</strong> ${campaign.achievements ? campaign.achievements.length : 0} badges recorded</div>
        `;
      } else {
        campEl.innerHTML = '<div style="color: var(--text-muted);">No active campaign save found. Launch a case from the top bar!</div>';
      }
    }

    // Notes Tab
    const notesEl = $('#vault-notes-display');
    if (notesEl) {
      notesEl.textContent = notes || 'No case notes currently recorded in database. Open Case Notes (dock/desktop) to record forensic indicators!';
    }

    // Tables Tab
    const tablesEl = $('#vault-tables-container');
    if (tablesEl) {
      const tableNames = ['users', 'user_profiles', 'user_sessions', 'user_case_files', 'user_achievements', 'user_audit_logs', 'user_side_quests', 'system_cache'];
      tablesEl.innerHTML = tableNames.map(name => `
        <div class="vault-table-item">
          <strong>${name}</strong>
          <span>SYNCHRONIZED (IN-MEMORY)</span>
        </div>
      `).join('');
    }

    // Audit Tab
    const auditEl = $('#vault-audit-container');
    if (auditEl) {
      if (auditLogs && auditLogs.length > 0) {
        auditEl.innerHTML = auditLogs.map(log => `
          <div class="vault-audit-row">
            <span>[${new Date(log.timestamp).toLocaleTimeString()}] ${log.action} - ${log.details}</span>
            <span style="color: #00ff80;">${log.status}</span>
          </div>
        `).join('');
      } else {
        auditEl.innerHTML = '<div class="vault-audit-row"><span>No security audit entries recorded yet.</span></div>';
      }
    }

    if ($('#vault-sync-label')) $('#vault-sync-label').textContent = 'Last Synced: ' + new Date().toLocaleTimeString();
  };

  const openUserVault = async () => {
    const modal = $('#aegis-user-vault-modal');
    if (!modal) return;
    await loadUserVault();
    updateVaultUI();
    modal.classList.remove('hidden');
  };

  const closeUserVault = () => {
    const modal = $('#aegis-user-vault-modal');
    if (modal) modal.classList.add('hidden');
  };

  const initVaultModalEvents = () => {
    $('#vault-modal-close')?.addEventListener('click', closeUserVault);

    document.querySelectorAll('.vault-tab-btn').forEach(btn => {
      btn.addEventListener('click', () => {
        document.querySelectorAll('.vault-tab-btn').forEach(b => b.classList.remove('active'));
        document.querySelectorAll('.vault-tab-content').forEach(c => c.classList.add('hidden'));
        btn.classList.add('active');
        const targetId = btn.dataset.vtab;
        if (targetId) $(`#${targetId}`)?.classList.remove('hidden');
      });
    });

    $('#vault-btn-sync')?.addEventListener('click', syncUserVault);

    $('#vault-btn-restore')?.addEventListener('click', async () => {
      const ok = await window.AegisDialog.confirm('Restore all campaign progress and case notes from AegisDB database?', 'RESTORE FROM DATABASE');
      if (!ok) return;
      await loadUserVault();
      updateVaultUI();
      await window.AegisDialog.alert('User database records re-synchronized successfully.', 'RESTORE COMPLETE');
    });

    $('#vault-btn-open-notepad')?.addEventListener('click', () => {
      closeUserVault();
      window.dispatchEvent(new CustomEvent('aegis:wm-open', { detail: { app: 'notepad' } }));
    });

    $('#vault-btn-export')?.addEventListener('click', () => {
      if (!vaultData) return;
      const json = JSON.stringify(vaultData, null, 2);
      const blob = new Blob([json], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `aegis_user_vault_${vaultData.user.username}_${Date.now()}.json`;
      a.click();
      URL.revokeObjectURL(url);
    });

    $('#vault-btn-signout')?.addEventListener('click', async () => {
      closeUserVault();
      const ok = await window.AegisDialog.confirm('Sign out of this AegisOS operator profile?', 'OPERATOR SIGN-OUT');
      if (!ok) return;
      try {
        await request('/api/auth/logout', {});
      } catch (error) {
        await window.AegisDialog.alert(error.message, 'SIGN-OUT ERROR');
        return;
      }
      user = null;
      vaultData = null;
      root.classList.add('aegis-locked');
      gate.hidden = false;
      showMode('login');
    });

    // Delegate open vault from Settings Center
    document.addEventListener('click', (e) => {
      if (e.target && e.target.id === 'btn-settings-open-vault') {
        openUserVault();
      }
    });
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
        badge.title = 'Open Operator Profile & User Database Vault';
        $('#top-bar .top-right')?.appendChild(badge);
        badge.addEventListener('click', openUserVault);
      }
      badge.textContent = '💾 OPERATOR VAULT // ' + (account.displayName || account.username).toUpperCase();
      window.AegisAuth = {
        get user() { return user; },
        logout: () => $('#vault-btn-signout')?.click()
      };
      window.AegisVault = {
        open: openUserVault,
        sync: syncUserVault,
        restore: loadUserVault
      };

      // Automatically hydrate user data from database upon login!
      loadUserVault();
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
        '[  0.210000] Restoring Operator Database Vault & Session Credentials...',
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

  initVaultModalEvents();

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
