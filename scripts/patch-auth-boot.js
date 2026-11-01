const fs = require('fs');
const path = require('path');

const authFile = path.join(__dirname, '..', 'simulator', 'web', 'desktop-auth.js');
let content = fs.readFileSync(authFile, 'utf8');

const oldEnter = `  const enter = (account) => {
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
        try { await request('/api/auth/logout', {}); } catch (error) { alert(error.message); return; }
        user = null;
        root.classList.add('aegis-locked');
        gate.hidden = false;
        showMode('login');
      });
    }
    badge.textContent = 'OPERATOR // ' + (account.displayName || account.username) + '  ↗';
    window.AegisAuth = { get user() { return user; }, logout: () => badge.click() };
    document.dispatchEvent(new CustomEvent('aegis:authenticated', { detail: { user: account } }));
  };`;

const newEnter = `  const enter = (account) => {
    user = account;
    gate.hidden = true;

    const bootScreen = $('#aegis-boot-screen');
    const bootConsole = $('#boot-console');
    const bootBarFill = $('#boot-bar-fill');
    const bootStatus = $('#boot-status');

    const finalizeLogin = () => {
      root.classList.remove('aegis-locked');
      if (bootScreen) {
        bootScreen.classList.add('fade-out');
        setTimeout(() => { bootScreen.classList.add('hidden'); bootScreen.classList.remove('fade-out'); }, 400);
      }
      let badge = $('#aegis-operator-badge');
      if (!badge) {
        badge = document.createElement('button');
        badge.id = 'aegis-operator-badge';
        badge.type = 'button';
        badge.className = 'aegis-operator-badge';
        $('#top-bar .top-right')?.appendChild(badge);
        badge.addEventListener('click', async () => {
          if (!confirm('Sign out of this AegisOS operator profile?')) return;
          try { await request('/api/auth/logout', {}); } catch (error) { alert(error.message); return; }
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

    if (bootScreen) {
      bootScreen.classList.remove('hidden');
      bootScreen.classList.remove('fade-out');
      if (bootBarFill) bootBarFill.style.width = '0%';
      if (bootConsole) {
        bootConsole.innerHTML = '<div class="boot-line">> [  0.000000] AEGIS DUAL-KERNEL 2.0-ALPHA (x86_64_MICROKERNEL)</div>';
      }
      const lines = [
        '> [  0.014200] CPU 8-Core context switch scheduler calibrated [OK]',
        '> [  0.038100] ASLR 64-bit entropy verified [OK]',
        '> [  0.062000] Mounting AegisFS cryptographic Merkle block inodes [OK]',
        '> [  0.091000] Enforcing Zero-Trust microkernel capability gates [OK]',
        '> [  0.124000] Loading Operator Enclave Profile: ' + (account.displayName || account.username),
        '>> Welcome to your virtual system.'
      ];
      let step = 0;
      const interval = setInterval(() => {
        step++;
        if (bootBarFill) bootBarFill.style.width = Math.min(100, Math.round((step / lines.length) * 100)) + '%';
        if (bootConsole && lines[step - 1]) {
          const div = document.createElement('div');
          div.className = 'boot-line';
          div.textContent = lines[step - 1];
          bootConsole.appendChild(div);
          bootConsole.scrollTop = bootConsole.scrollHeight;
        }
        if (bootStatus) {
          bootStatus.textContent = step === lines.length ? "WELCOME TO YOUR VIRTUAL SYSTEM." : "CALIBRATING ZERO-TRUST GATES...";
        }
        if (step >= lines.length) {
          clearInterval(interval);
          setTimeout(finalizeLogin, 350);
        }
      }, 180);
    } else {
      finalizeLogin();
    }
  };`;

content = content.replace(oldEnter, newEnter);

fs.writeFileSync(authFile, content, 'utf8');
console.log('Successfully updated simulator/web/desktop-auth.js with boot screen flow.');

