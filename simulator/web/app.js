/**
 * AegisOS - Interactive Web Desktop Simulator Controller
 * Full-fidelity client-side environment for testing and demonstrating AegisOS architecture.
 */

(function () {
    'use strict';

    // ==========================================
    // 1. SOUND SYNTHESIS ENGINE (Web Audio API)
    // ==========================================
    const AudioEngine = {
        ctx: null,
        init() {
            if (!this.ctx && (window.AudioContext || window.webkitAudioContext)) {
                this.ctx = new (window.AudioContext || window.webkitAudioContext)();
            }
        },
        playChime(type = 'info') {
            try {
                this.init();
                if (!this.ctx) return;
                const now = this.ctx.currentTime;
                const osc = this.ctx.createOscillator();
                const gain = this.ctx.createGain();

                osc.connect(gain);
                gain.connect(this.ctx.destination);

                if (type === 'alert') {
                    osc.type = 'sawtooth';
                    osc.frequency.setValueAtTime(440, now);
                    osc.frequency.setValueAtTime(220, now + 0.1);
                    gain.gain.setValueAtTime(0.12, now);
                    gain.gain.exponentialRampToValueAtTime(0.01, now + 0.3);
                    osc.start(now);
                    osc.stop(now + 0.3);
                } else if (type === 'success') {
                    osc.type = 'sine';
                    osc.frequency.setValueAtTime(523.25, now); // C5
                    osc.frequency.setValueAtTime(659.25, now + 0.08); // E5
                    osc.frequency.setValueAtTime(783.99, now + 0.16); // G5
                    gain.gain.setValueAtTime(0.1, now);
                    gain.gain.exponentialRampToValueAtTime(0.01, now + 0.4);
                    osc.start(now);
                    osc.stop(now + 0.4);
                } else {
                    osc.type = 'sine';
                    osc.frequency.setValueAtTime(800, now);
                    gain.gain.setValueAtTime(0.08, now);
                    gain.gain.exponentialRampToValueAtTime(0.01, now + 0.15);
                    osc.start(now);
                    osc.stop(now + 0.15);
                }
            } catch (e) {
                // Ignore audio restriction errors
            }
        }
    };

    // ==========================================
    // 2. WALLPAPER AMBIENT CANVAS
    // ==========================================
    const Wallpaper = {
        canvas: null,
        ctx: null,
        particles: [],
        width: 0,
        height: 0,

        init() {
            this.canvas = document.getElementById('wallpaper-canvas');
            if (!this.canvas) return;
            this.ctx = this.canvas.getContext('2d');
            this.resize();
            window.addEventListener('resize', () => this.resize());

            // Initialize subtle cyber particles
            const count = Math.floor((this.width * this.height) / 25000);
            for (let i = 0; i < count; i++) {
                this.particles.push({
                    x: Math.random() * this.width,
                    y: Math.random() * this.height,
                    vx: (Math.random() - 0.5) * 0.4,
                    vy: (Math.random() - 0.5) * 0.4,
                    radius: Math.random() * 1.5 + 0.8
                });
            }

            this.animate();
        },

        resize() {
            this.width = this.canvas.width = window.innerWidth;
            this.height = this.canvas.height = window.innerHeight;
        },

        animate() {
            requestAnimationFrame(() => this.animate());
            const ctx = this.ctx;
            ctx.clearRect(0, 0, this.width, this.height);

            // Subtle cyber grid
            const isDark = !document.body.classList.contains('theme-crystal-light');
            ctx.strokeStyle = isDark ? 'rgba(0, 243, 255, 0.03)' : 'rgba(0, 120, 215, 0.05)';
            ctx.lineWidth = 1;

            const gridSize = 48;
            for (let x = 0; x < this.width; x += gridSize) {
                ctx.beginPath();
                ctx.moveTo(x, 0);
                ctx.lineTo(x, this.height);
                ctx.stroke();
            }
            for (let y = 0; y < this.height; y += gridSize) {
                ctx.beginPath();
                ctx.moveTo(0, y);
                ctx.lineTo(this.width, y);
                ctx.stroke();
            }

            // Draw neural floating particles
            ctx.fillStyle = isDark ? 'rgba(0, 243, 255, 0.35)' : 'rgba(0, 120, 215, 0.35)';
            for (let i = 0; i < this.particles.length; i++) {
                const p = this.particles[i];
                p.x += p.vx;
                p.y += p.vy;

                if (p.x < 0) p.x = this.width;
                if (p.x > this.width) p.x = 0;
                if (p.y < 0) p.y = this.height;
                if (p.y > this.height) p.y = 0;

                ctx.beginPath();
                ctx.arc(p.x, p.y, p.radius, 0, Math.PI * 2);
                ctx.fill();

                // Draw connecting links between nearby nodes
                for (let j = i + 1; j < this.particles.length; j++) {
                    const p2 = this.particles[j];
                    const dx = p.x - p2.x;
                    const dy = p.y - p2.y;
                    const dist = Math.sqrt(dx * dx + dy * dy);
                    if (dist < 100) {
                        ctx.strokeStyle = isDark ? `rgba(0, 243, 255, ${0.12 * (1 - dist / 100)})` : `rgba(0, 120, 215, ${0.1 * (1 - dist / 100)})`;
                        ctx.beginPath();
                        ctx.moveTo(p.x, p.y);
                        ctx.lineTo(p2.x, p2.y);
                        ctx.stroke();
                    }
                }
            }
        }
    };

    // ==========================================
    // 3. TOAST NOTIFICATION SYSTEM
    // ==========================================
    const Notifications = {
        container: null,
        init() {
            this.container = document.getElementById('toast-container');
        },
        show(title, desc, type = 'info', duration = 4000) {
            AudioEngine.playChime(type === 'error' ? 'alert' : type === 'success' ? 'success' : 'info');

            const toast = document.createElement('div');
            toast.className = `toast toast-${type}`;
            toast.innerHTML = `
                <div class="toast-icon">
                    <svg viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>
                </div>
                <div class="toast-content">
                    <div class="toast-title">${title}</div>
                    <div class="toast-desc">${desc}</div>
                </div>
            `;

            this.container.appendChild(toast);
            setTimeout(() => {
                toast.style.opacity = '0';
                toast.style.transform = 'translateX(50px)';
                toast.style.transition = 'all 0.3s ease';
                setTimeout(() => toast.remove(), 300);
            }, duration);
        }
    };

    // ==========================================
    // 4. WINDOW MANAGER (WM)
    // ==========================================
    const WM = {
        container: null,
        windows: new Map(),
        zIndexCounter: 100,

        init() {
            this.container = document.getElementById('wm-container');
        },

        open(appId, title, width = 720, height = 480) {
            if (this.windows.has(appId)) {
                const win = this.windows.get(appId);
                win.classList.remove('minimized');
                this.bringToFront(win);
                return;
            }

            const template = document.getElementById(`tmpl-${appId}`);
            if (!template) {
                Notifications.show('Launch Error', `Application '${appId}' template not found.`, 'error');
                return;
            }

            // Create window element
            const win = document.createElement('div');
            win.className = 'aegis-window active';
            win.dataset.appId = appId;
            win.style.width = `${Math.min(width, window.innerWidth - 40)}px`;
            win.style.height = `${Math.min(height, window.innerHeight - 100)}px`;

            // Stagger position
            const offset = (this.windows.size * 28) % 140;
            win.style.top = `${70 + offset}px`;
            win.style.left = `${Math.max(20, (window.innerWidth - width) / 2 + offset)}px`;
            win.style.zIndex = ++this.zIndexCounter;

            win.innerHTML = `
                <div class="win-header">
                    <div class="win-title">
                        <svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="10"/></svg>
                        <span>${title}</span>
                    </div>
                    <div class="win-controls">
                        <button class="win-btn win-btn-min" title="Minimize"></button>
                        <button class="win-btn win-btn-max" title="Maximize"></button>
                        <button class="win-btn win-btn-close" title="Close"></button>
                    </div>
                </div>
                <div class="win-body"></div>
            `;

            const winBody = win.querySelector('.win-body');
            winBody.appendChild(template.content.cloneNode(true));
            this.container.appendChild(win);
            this.windows.set(appId, win);

            // Update dock icon running indicator
            this.updateDockState(appId, true);

            // Setup Window Controls
            win.querySelector('.win-btn-close').addEventListener('click', (e) => {
                e.stopPropagation();
                this.close(appId);
            });
            win.querySelector('.win-btn-min').addEventListener('click', (e) => {
                e.stopPropagation();
                win.classList.add('minimized');
            });
            win.querySelector('.win-btn-max').addEventListener('click', (e) => {
                e.stopPropagation();
                win.classList.toggle('maximized');
            });

            // Focus on click
            win.addEventListener('mousedown', () => this.bringToFront(win));

            // Dragging behavior
            this.setupDrag(win);

            // Initialize app logic
            this.initAppLogic(appId, win);
            AudioEngine.playChime('info');
        },

        close(appId) {
            if (this.windows.has(appId)) {
                const win = this.windows.get(appId);
                win.remove();
                this.windows.delete(appId);
                this.updateDockState(appId, false);
            }
        },

        bringToFront(win) {
            document.querySelectorAll('.aegis-window').forEach(w => w.classList.remove('active'));
            win.classList.add('active');
            win.style.zIndex = ++this.zIndexCounter;
        },

        setupDrag(win) {
            const header = win.querySelector('.win-header');
            let isDragging = false;
            let startX, startY, initialLeft, initialTop;

            header.addEventListener('mousedown', (e) => {
                if (win.classList.contains('maximized')) return;
                isDragging = true;
                startX = e.clientX;
                startY = e.clientY;
                initialLeft = win.offsetLeft;
                initialTop = win.offsetTop;
                this.bringToFront(win);
                e.preventDefault();
            });

            document.addEventListener('mousemove', (e) => {
                if (!isDragging) return;
                const dx = e.clientX - startX;
                const dy = e.clientY - startY;
                win.style.left = `${Math.max(0, Math.min(window.innerWidth - win.offsetWidth, initialLeft + dx))}px`;
                win.style.top = `${Math.max(44, Math.min(window.innerHeight - 80, initialTop + dy))}px`;
            });

            document.addEventListener('mouseup', () => {
                isDragging = false;
            });
        },

        updateDockState(appId, isRunning) {
            document.querySelectorAll(`.dock-app-btn[data-app="${appId}"]`).forEach(btn => {
                if (isRunning) btn.classList.add('running');
                else btn.classList.remove('running');
            });
        },

        initAppLogic(appId, win) {
            switch (appId) {
                case 'terminal': TerminalApp.init(win); break;
                case 'neural': NeuralApp.init(win); break;
                case 'shield': ShieldApp.init(win); break;
                case 'merkle': MerkleApp.init(win); break;
                case 'optimizer': OptimizerApp.init(win); break;
                case 'troubleshoot': TroubleshootApp.init(win); break;
                case 'store': StoreApp.init(win); break;
                case 'settings': SettingsApp.init(win); break;
                case 'voice': VoiceApp.init(win); break;
            }
        }
    };

    // ==========================================
    // 5. TERMINAL APPLICATION CONTROLLER
    // ==========================================
    const TerminalApp = {
        init(win) {
            const input = win.querySelector('.term-input');
            const history = win.querySelector('.term-history');
            const screen = win.querySelector('.terminal-screen');
            const commandHistory = [];
            let historyIndex = -1;

            input.focus();

            input.addEventListener('keydown', async (e) => {
                if (e.key === 'Enter') {
                    const cmd = input.value.trim();
                    if (!cmd) return;

                    commandHistory.push(cmd);
                    historyIndex = commandHistory.length;

                    // Append user command
                    const line = document.createElement('div');
                    line.className = 'term-line';
                    line.innerHTML = `<span class="prompt-prefix">aegis@kernel-ring0:~$</span> ${escapeHtml(cmd)}`;
                    history.appendChild(line);

                    input.value = '';

                    // Handle 'clear' locally
                    if (cmd.toLowerCase() === 'clear') {
                        history.innerHTML = '';
                        return;
                    }

                    // Send to backend shell or fallback emulator
                    try {
                        const res = await fetch('/api/terminal/exec', {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify({ command: cmd })
                        });
                        const data = await res.json();
                        const outLine = document.createElement('div');
                        outLine.className = 'term-line';
                        outLine.style.color = '#e6f8ff';
                        outLine.textContent = data.output || 'Execution complete.';
                        history.appendChild(outLine);
                    } catch (err) {
                        const errLine = document.createElement('div');
                        errLine.className = 'term-line';
                        errLine.style.color = 'var(--status-err)';
                        errLine.textContent = `Shell bridge error: ${err.message}`;
                        history.appendChild(errLine);
                    }

                    screen.scrollTop = screen.scrollHeight;
                } else if (e.key === 'ArrowUp') {
                    if (historyIndex > 0) {
                        historyIndex--;
                        input.value = commandHistory[historyIndex] || '';
                    }
                    e.preventDefault();
                } else if (e.key === 'ArrowDown') {
                    if (historyIndex < commandHistory.length - 1) {
                        historyIndex++;
                        input.value = commandHistory[historyIndex] || '';
                    } else {
                        historyIndex = commandHistory.length;
                        input.value = '';
                    }
                    e.preventDefault();
                } else if (e.key === 'Tab') {
                    e.preventDefault();
                    const list = ['help', 'sysinfo', 'ps', 'shield', 'guard', 'ztac', 'troubleshoot', 'opt', 'checkpoint', 'neural', 'fs', 'theme', 'layout', 'clear'];
                    const val = input.value.toLowerCase();
                    const match = list.find(c => c.startsWith(val));
                    if (match) input.value = match;
                }
            });
        }
    };

    // ==========================================
    // 6. NEURAL ACTIVITY MONITOR CONTROLLER
    // ==========================================
    const NeuralApp = {
        init(win) {
            const canvas = win.querySelector('#neural-wave-canvas');
            if (!canvas) return;
            const ctx = canvas.getContext('2d');
            const eventsList = win.querySelector('#neural-events-list');

            // Render live dynamic waveforms
            let step = 0;
            const drawWave = () => {
                if (!document.body.contains(win)) return;
                requestAnimationFrame(drawWave);
                step += 0.05;

                ctx.fillStyle = 'rgba(6, 11, 20, 0.25)';
                ctx.fillRect(0, 0, canvas.width, canvas.height);

                // Wave 1: CPU Entropy (Cyan)
                ctx.beginPath();
                ctx.strokeStyle = '#00f3ff';
                ctx.lineWidth = 2;
                for (let x = 0; x < canvas.width; x++) {
                    const y = (canvas.height / 2) + Math.sin(x * 0.03 + step) * 25 + Math.cos(x * 0.015 - step * 1.5) * 15;
                    if (x === 0) ctx.moveTo(x, y);
                    else ctx.lineTo(x, y);
                }
                ctx.stroke();

                // Wave 2: Memory Page Fault Noise (Secondary)
                ctx.beginPath();
                ctx.strokeStyle = 'rgba(255, 170, 0, 0.45)';
                ctx.lineWidth = 1.5;
                for (let x = 0; x < canvas.width; x += 3) {
                    const y = (canvas.height / 2) + Math.sin(x * 0.05 - step * 2) * 18 + (Math.random() - 0.5) * 6;
                    if (x === 0) ctx.moveTo(x, y);
                    else ctx.lineTo(x, y);
                }
                ctx.stroke();
            };
            drawWave();

            // Seed event stream
            const eventPool = [
                { type: 'FS_READ', desc: '/system/lib/libaegis.so (Page #402)', status: 'ALLOW' },
                { type: 'MEM_VERIFY', desc: 'Canary checked on PID 24 (Stack OK)', status: 'ALLOW' },
                { type: 'NET_FILTER', desc: 'Stateful inspection TLS 1.3 frame', status: 'ALLOW' },
                { type: 'ZT_GATE', desc: 'Kernel broker verified ephemeral token', status: 'ALLOW' },
                { type: 'HEURISTIC', desc: 'Guard scan: Entropy score 0.89 normal', status: 'ALLOW' }
            ];

            const addEvent = () => {
                if (!document.body.contains(win)) return;
                const ev = eventPool[Math.floor(Math.random() * eventPool.length)];
                const row = document.createElement('div');
                row.className = 'event-row';
                row.innerHTML = `
                    <span>${new Date().toLocaleTimeString()}</span>
                    <strong style="color: var(--accent);">${ev.type}</strong>
                    <span>${ev.desc}</span>
                    <span style="color: var(--status-ok);">${ev.status}</span>
                `;
                eventsList.prepend(row);
                if (eventsList.children.length > 25) eventsList.lastChild.remove();
            };

            for (let i = 0; i < 5; i++) addEvent();
            const timer = setInterval(addEvent, 1200);
            win.addEventListener('remove', () => clearInterval(timer));
        }
    };

    // ==========================================
    // 7. AEGIS SHIELD & ZERO-TRUST CONTROLLER
    // ==========================================
    const ShieldApp = {
        init(win) {
            const tabs = win.querySelectorAll('.tab-btn');
            const contents = win.querySelectorAll('.tab-content');
            tabs.forEach(btn => {
                btn.addEventListener('click', () => {
                    tabs.forEach(t => t.classList.remove('active'));
                    contents.forEach(c => c.classList.add('hidden'));
                    btn.classList.add('active');
                    win.querySelector(`#${btn.dataset.tab}`).classList.remove('hidden');
                });
            });

            // Populate Firewall rules
            const tbody = win.querySelector('#firewall-rules-body');
            const rules = [
                { id: 1, name: "Block Inbound Telnet", proto: "TCP", port: 23, dir: "INBOUND", action: "DENY", enabled: true },
                { id: 2, name: "Allow Secure Shell (SSH)", proto: "TCP", port: 22, dir: "INBOUND", action: "ALLOW", enabled: true },
                { id: 3, name: "Allow HTTPS Web Traffic", proto: "TCP", port: 443, dir: "OUTBOUND", action: "ALLOW", enabled: true },
                { id: 4, name: "Block Outbound Tor Relays", proto: "TCP", port: 9001, dir: "OUTBOUND", action: "DENY", enabled: true },
                { id: 5, name: "Zero-Trust Kernel IPC Gate", proto: "IPC", port: 0, dir: "INTERNAL", action: "ALLOW", enabled: true }
            ];

            tbody.innerHTML = rules.map(r => `
                <tr>
                    <td><strong>${r.name}</strong></td>
                    <td><code>${r.proto}</code></td>
                    <td>${r.port}</td>
                    <td>${r.dir}</td>
                    <td><span class="status-pill ${r.action === 'ALLOW' ? 'pill-secure' : 'pill-blocked'}">${r.action}</span></td>
                    <td>
                        <input type="checkbox" ${r.enabled ? 'checked' : ''} class="aegis-switch rule-toggle" data-id="${r.id}">
                    </td>
                </tr>
            `).join('');

            tbody.querySelectorAll('.rule-toggle').forEach(t => {
                t.addEventListener('change', () => {
                    Notifications.show('Shield Policy Updated', `Firewall rule #${t.dataset.id} state toggled.`, 'info');
                });
            });

            // Guard Scan Button
            win.querySelector('#btn-scan-guard').addEventListener('click', async () => {
                Notifications.show('Aegis Guard', 'Executing AI heuristic intrusion detection scan across all processes...', 'info');
                try {
                    const res = await fetch('/api/guard/scan', { method: 'POST' });
                    const data = await res.json();
                    Notifications.show('Guard Scan Complete', `Scanned ${data.scannedProcesses} processes. 0 unmitigated intrusions. ML Accuracy: ${data.accuracy}%.`, 'success');
                } catch (e) {
                    Notifications.show('Guard Scan Complete', '68 processes scanned. AI model accuracy: 98.7%. Zero-Day traps operational.', 'success');
                }
            });

            // Populate Quarantined files
            const qList = win.querySelector('#quarantine-items');
            qList.innerHTML = `
                <div class="zt-item">
                    <div>
                        <strong style="color: var(--status-err);">THR-8821: ransom_injector.bin</strong>
                        <div class="zt-desc">Ransomware canary file tripwire triggered in /tmp. Blocked by Ring 0 shield.</div>
                    </div>
                    <span class="status-pill pill-blocked">QUARANTINED</span>
                </div>
                <div class="zt-item">
                    <div>
                        <strong style="color: var(--status-warn);">THR-8822: mem_escalate_poc</strong>
                        <div class="zt-desc">Ring 3 binary attempted privilege escalation to Ring 0. Blocked by Zero-Trust AC.</div>
                    </div>
                    <span class="status-pill pill-blocked">QUARANTINED</span>
                </div>
            `;
        }
    };

    // ==========================================
    // 8. MERKLE SELF-HEALING & ROLLBACK CONTROLLER
    // ==========================================
    const MerkleApp = {
        init(win) {
            const blocksContainer = win.querySelector('#merkle-blocks-container');
            const statusCard = win.querySelector('#merkle-status-card');
            const statusLabel = win.querySelector('#merkle-status-label');
            const indicator = win.querySelector('#merkle-indicator');
            const btnCorrupt = win.querySelector('#btn-corrupt-merkle');
            const btnRollback = win.querySelector('#btn-rollback-merkle');
            const timerBox = win.querySelector('#rollback-timer-box');
            const timerVal = win.querySelector('#rollback-timer-val');
            const timerBar = win.querySelector('#rollback-bar-fill');

            const blocks = [
                { id: "blk-0", path: "/boot/vmlinuz-aegis", hash: "a4f912c...", size: "14.2 MB", status: "HEALTHY" },
                { id: "blk-1", path: "/boot/initramfs-aegis.img", hash: "6b2e98d...", size: "28.5 MB", status: "HEALTHY" },
                { id: "blk-2", path: "/system/lib/libaegis_kernel.so", hash: "3e1a77f...", size: "8.4 MB", status: "HEALTHY" },
                { id: "blk-3", path: "/system/security/zero_trust.db", hash: "9d4c21b...", size: "1.2 MB", status: "HEALTHY" },
                { id: "blk-4", path: "/userland/data/user_vault.dat", hash: "8f4d92a...", size: "54.1 MB", status: "HEALTHY" },
                { id: "blk-5", path: "/filesystem/merkle_journal.log", hash: "7c5e31a...", size: "512 KB", status: "HEALTHY" }
            ];

            const renderBlocks = () => {
                blocksContainer.innerHTML = blocks.map(b => `
                    <div class="merkle-block-card ${b.status === 'CORRUPTED' ? 'corrupted' : ''}" id="card-${b.id}">
                        <div class="block-path">${b.path}</div>
                        <div class="block-hash">SHA-256: ${b.hash}</div>
                        <div class="block-status" style="color: ${b.status === 'CORRUPTED' ? 'var(--status-err)' : 'var(--status-ok)'};">
                            ● ${b.status}
                        </div>
                    </div>
                `).join('');
            };
            renderBlocks();

            // Populate Snapshots
            const snapList = win.querySelector('#snapshots-list');
            snapList.innerHTML = `
                <div class="snapshot-item">
                    <span><strong>chkpt-0001:</strong> Clean Boot Baseline (482 MB)</span>
                    <span class="status-pill pill-secure">BOOTABLE RESTORE POINT</span>
                </div>
                <div class="snapshot-item">
                    <span><strong>chkpt-0002:</strong> Pre-Update Snapshot (512 MB)</span>
                    <span class="status-pill pill-secure">BOOTABLE RESTORE POINT</span>
                </div>
                <div class="snapshot-item">
                    <span><strong>chkpt-0003:</strong> System Restore Point Alpha (515 MB)</span>
                    <span class="status-pill pill-secure">LATEST SNAPSHOT</span>
                </div>
            `;

            // 1. Simulate Ransomware Attack
            btnCorrupt.addEventListener('click', async () => {
                try {
                    await fetch('/api/merkle/corrupt', { method: 'POST' });
                } catch (e) { }

                blocks[4].status = 'CORRUPTED';
                blocks[4].hash = '0xBAD_ENCRYPTED_RANSOM';
                renderBlocks();

                indicator.className = 'status-indicator-ring red';
                statusLabel.textContent = 'MERKLE ROOT COMPROMISED: SHA-256 HASH DIVERGENCE DETECTED!';
                statusLabel.style.color = 'var(--status-err)';

                Notifications.show('CRITICAL: RANSOMWARE ATTACK DETECTED', 'Aegis Guard identified file corruption in block /userland/data/user_vault.dat. Automatic rollback recommended.', 'error', 6000);
            });

            // 2. Execute 10s Atomic Rollback
            btnRollback.addEventListener('click', async () => {
                timerBox.classList.remove('hidden');
                timerBar.style.width = '0%';
                setTimeout(() => timerBar.style.width = '100%', 50);

                let countdown = 1.42;
                const interval = setInterval(() => {
                    countdown -= 0.1;
                    if (countdown <= 0) {
                        clearInterval(interval);
                        timerVal.textContent = '00.00s';
                        timerBox.classList.add('hidden');

                        // Restore blocks
                        blocks.forEach(b => {
                            b.status = 'HEALTHY';
                            if (b.id === 'blk-4') b.hash = '8f4d92a...';
                        });
                        renderBlocks();

                        indicator.className = 'status-indicator-ring green';
                        statusLabel.textContent = 'MERKLE ROOT STATUS: INTEGRITY RESTORED & VERIFIED';
                        statusLabel.style.color = 'var(--text-primary)';

                        Notifications.show('Self-Healing Completed', 'Sub-10s Atomic Rollback restored filesystem to baseline in 1.42 seconds. Zero data loss.', 'success', 5000);

                        fetch('/api/merkle/rollback', { method: 'POST' }).catch(() => {});
                    } else {
                        timerVal.textContent = `0${countdown.toFixed(2)}s`;
                    }
                }, 100);
            });
        }
    };

    // ==========================================
    // 9. TASK OPTIMIZER CONTROLLER
    // ==========================================
    const OptimizerApp = {
        init(win) {
            const cards = win.querySelectorAll('.mode-card');
            const badge = win.querySelector('#opt-current-badge');
            const freqVal = win.querySelector('#metric-cpu-freq');
            const freqFill = win.querySelector('#fill-cpu-freq');
            const gpuVal = win.querySelector('#metric-gpu-val');
            const gpuFill = win.querySelector('#fill-gpu-freq');
            const tdpVal = win.querySelector('#metric-tdp-val');
            const tdpFill = win.querySelector('#fill-tdp-freq');

            const setMode = async (mode) => {
                cards.forEach(c => c.classList.remove('active'));
                const activeCard = win.querySelector(`.mode-card[data-mode="${mode}"]`);
                if (activeCard) activeCard.classList.add('active');

                badge.textContent = `MODE: ${mode}`;

                if (mode === 'GAME') {
                    freqVal.textContent = '4.85 GHz (Boost)';
                    freqFill.style.width = '95%';
                    gpuVal.textContent = '+30% (Overclock)';
                    gpuFill.style.width = '90%';
                    tdpVal.textContent = '65 Watts';
                    tdpFill.style.width = '85%';
                    Notifications.show('Game Mode Engaged', '+25% CPU Priority, +30% GPU boost. Background indexing paused.', 'success');
                } else if (mode === 'WORK') {
                    freqVal.textContent = '3.85 GHz (Balanced)';
                    freqFill.style.width = '75%';
                    gpuVal.textContent = '+0% (Standard)';
                    gpuFill.style.width = '50%';
                    tdpVal.textContent = '28 Watts';
                    tdpFill.style.width = '45%';
                    Notifications.show('Work Mode Engaged', 'CFS scheduler set to balanced multitasking.', 'info');
                } else if (mode === 'STREAMING') {
                    freqVal.textContent = '4.20 GHz';
                    freqFill.style.width = '82%';
                    gpuVal.textContent = '+15% (NVENC Priority)';
                    gpuFill.style.width = '70%';
                    tdpVal.textContent = '45 Watts';
                    tdpFill.style.width = '60%';
                    Notifications.show('Streaming Mode Engaged', 'QoS network queue active to prevent dropped frames.', 'info');
                } else if (mode === 'ECO') {
                    freqVal.textContent = '1.80 GHz (Eco)';
                    freqFill.style.width = '35%';
                    gpuVal.textContent = '-40% (Power Save)';
                    gpuFill.style.width = '20%';
                    tdpVal.textContent = '9 Watts';
                    tdpFill.style.width = '15%';
                    Notifications.show('Eco Mode Engaged', 'Package TDP locked to 9W for maximum battery longevity.', 'info');
                }

                try {
                    await fetch('/api/optimizer/set-mode', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ mode })
                    });
                } catch (e) { }
            };

            win.querySelectorAll('.btn-opt-select').forEach(b => {
                b.addEventListener('click', () => setMode(b.dataset.mode));
            });
        }
    };

    // ==========================================
    // 10. SYSTEM TROUBLESHOOTER CONTROLLER
    // ==========================================
    const TroubleshootApp = {
        init(win) {
            const list = win.querySelector('#diag-list-items');
            const btnRun = win.querySelector('#btn-run-diagnostics');

            const tests = [
                { id: 1, name: "Kernel ASLR & Microkernel Page Table Isolation", msg: "Randomized with 64-bit entropy. Zero overlapping rings detected." },
                { id: 2, name: "Zero-Trust Driver Sandboxes (Ring 3 Isolation)", msg: "12 registered user-space drivers isolated without direct DMA." },
                { id: 3, name: "Merkle Tree Filesystem Hash Consistency", msg: "Cryptographic root integrity verified across all allocation blocks." },
                { id: 4, name: "Dynamic Memory Leak & Double-Free Heuristics", msg: "Heap canary markers intact. 0 unfreed chunks older than TTL." },
                { id: 5, name: "Aegis Shield Stateful Packet Inspection & TLS 1.3", msg: "Firewall rules enforced. Zero open debug ports." },
                { id: 6, name: "System Checkpoint & Snapshot Storage State", msg: "3 valid restore points found. Sub-10s rollback ready." },
                { id: 7, name: "CFS Scheduler & Task Optimizer Affinity", msg: "Thread scheduling heuristics verified against active CPU cores." }
            ];

            const render = () => {
                list.innerHTML = tests.map(t => `
                    <div class="diag-item">
                        <div class="status-pill pill-secure">PASS</div>
                        <div>
                            <div class="diag-name">${t.name}</div>
                            <div class="diag-msg">${t.msg}</div>
                        </div>
                    </div>
                `).join('');
            };
            render();

            btnRun.addEventListener('click', async () => {
                list.innerHTML = '<div style="padding: 20px; text-align: center; color: var(--accent);">Running 7-Point Diagnostic Scan...</div>';
                setTimeout(() => {
                    render();
                    Notifications.show('Diagnostics Complete', 'All 7 AegisOS core subsystems passed integrity and security verification.', 'success');
                }, 800);
            });
        }
    };

    // ==========================================
    // 11. APP STORE CONTROLLER
    // ==========================================
    const StoreApp = {
        init(win) {
            const catalog = win.querySelector('#store-catalog');
            const apps = [
                { id: 'devstudio', name: 'Aegis DevStudio', author: 'Aegis Core Team', desc: 'Integrated Rust & C development environment with live microkernel debugger.', installed: true },
                { id: 'cyberdeck', name: 'CyberDeck IDS', author: 'Quantum Security Labs', desc: 'Advanced packet graph visualizer and stateful intrusion forensics console.', installed: false },
                { id: 'browser', name: 'Quantum Browser', author: 'Mozilla / Aegis Port', desc: 'Sandboxed Zero-Trust browser with per-tab micro-virtualization memory isolation.', installed: false },
                { id: 'cryptsafe', name: 'CryptSafe Vault', author: 'Shield Foundation', desc: 'Hardware-backed AES-256 password & cryptographic identity manager.', installed: true },
                { id: 'calc', name: 'Matrix Scientific Calc', author: 'Aegis Community', desc: 'High-precision tensor & floating point calculation workstation tool.', installed: false },
                { id: 'monitor', name: 'SysProfiler Pro', author: 'Kernel Tools', desc: 'Flamegraph generator and CFS scheduler latency analysis suite.', installed: false }
            ];

            const render = () => {
                catalog.innerHTML = apps.map(app => `
                    <div class="app-card">
                        <div class="app-card-top">
                            <div class="app-card-icon">
                                <svg viewBox="0 0 24 24"><polygon points="12 2 2 7 12 12 22 7 12 2"/><polyline points="2 17 12 22 22 17"/><polyline points="2 12 12 17 22 12"/></svg>
                            </div>
                            <div>
                                <div class="app-card-title">${app.name}</div>
                                <div class="app-card-author">${app.author}</div>
                            </div>
                        </div>
                        <div class="app-card-desc">${app.desc}</div>
                        <div class="app-card-footer">
                            <span class="app-badge-verified">
                                <svg viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg> Verified
                            </span>
                            <button class="btn-action-small btn-install" data-id="${app.id}">
                                ${app.installed ? 'Installed' : 'Install'}
                            </button>
                        </div>
                    </div>
                `).join('');

                catalog.querySelectorAll('.btn-install').forEach(btn => {
                    btn.addEventListener('click', () => {
                        const target = apps.find(a => a.id === btn.dataset.id);
                        if (target) {
                            target.installed = true;
                            btn.textContent = 'Installed';
                            Notifications.show('App Installed', `${target.name} has been provisioned into a Ring 3 sandbox.`, 'success');
                        }
                    });
                });
            };
            render();
        }
    };

    // ==========================================
    // 12. SETTINGS APP CONTROLLER
    // ==========================================
    const SettingsApp = {
        init(win) {
            const tabs = win.querySelectorAll('.settings-nav-btn');
            const panels = win.querySelectorAll('.settings-panel');
            tabs.forEach(btn => {
                btn.addEventListener('click', () => {
                    tabs.forEach(b => b.classList.remove('active'));
                    panels.forEach(p => p.classList.add('hidden'));
                    btn.classList.add('active');
                    win.querySelector(`#${btn.dataset.tab}`).classList.remove('hidden');
                });
            });

            // Layout selector in settings
            const layoutSel = win.querySelector('#settings-layout-select');
            if (layoutSel) {
                layoutSel.value = document.body.className.match(/layout-(\w+)/)?.[1] || 'hybrid';
                layoutSel.addEventListener('change', () => {
                    setLayout(layoutSel.value);
                });
            }

            // Theme selector in settings
            const themeSel = win.querySelector('#settings-theme-select');
            if (themeSel) {
                themeSel.value = document.body.className.match(/theme-([\w-]+)/)?.[1] || 'quantum-cyan';
                themeSel.addEventListener('change', () => {
                    setTheme(themeSel.value);
                });
            }
        }
    };

    // ==========================================
    // 13. VOICE ASSISTANT SIMULATOR
    // ==========================================
    const VoiceApp = {
        init(win) {
            const status = win.querySelector('#voice-status');
            const response = win.querySelector('#voice-response');

            const handleVoice = (cmd) => {
                status.textContent = `Aegis Voice: Processing "${cmd}"...`;
                AudioEngine.playChime('info');

                setTimeout(() => {
                    const low = cmd.toLowerCase();
                    let reply = `Executing voice instruction: "${cmd}"`;

                    if (low.includes('game') || low.includes('gaming')) {
                        reply = "Aegis Task Optimizer configured for Game Mode. Clocks boosted.";
                        WM.open('optimizer', 'Task Optimizer');
                        document.querySelector('.btn-opt-select[data-mode="GAME"]')?.click();
                    } else if (low.includes('security') || low.includes('scan')) {
                        reply = "Starting Aegis Guard AI heuristic sweep and diagnostic scan.";
                        WM.open('shield', 'Aegis Shield');
                    } else if (low.includes('integrity') || low.includes('filesystem')) {
                        reply = "AegisFS Merkle tree integrity is verified. 0 corrupted blocks.";
                        WM.open('merkle', 'Self-Healing Merkle Filesystem');
                    } else if (low.includes('amber')) {
                        reply = "Switching adaptive theme to Cyberpunk Amber.";
                        setTheme('cyberpunk-amber');
                    } else if (low.includes('checkpoint')) {
                        reply = "System restore point snapshot created successfully.";
                        Notifications.show('Checkpoint Created', 'Snapshot chkpt-0004 written to Merkle journal.', 'success');
                    }

                    response.textContent = `"${reply}"`;
                    status.textContent = "Aegis Voice: Ready for next command.";

                    // Web Speech Synthesis if available
                    if ('speechSynthesis' in window) {
                        try {
                            const utter = new SpeechSynthesisUtterance(reply);
                            utter.rate = 1.05;
                            window.speechSynthesis.speak(utter);
                        } catch (e) { }
                    }
                }, 500);
            };

            win.querySelectorAll('.voice-btn').forEach(btn => {
                btn.addEventListener('click', () => handleVoice(btn.dataset.cmd));
            });
        }
    };

    // ==========================================
    // 14. GLOBAL COMMAND PALETTE (CTRL+SPACE)
    // ==========================================
    const Palette = {
        overlay: null,
        input: null,
        results: null,
        selectedIndex: 0,
        items: [
            { title: "Terminal (aegis_shell)", sub: "Interactive microkernel shell and command console", app: "terminal", icon: "terminal" },
            { title: "Neural Activity Monitor", sub: "Live entropy waveforms, telemetry and thread graphs", app: "neural", icon: "neural" },
            { title: "Aegis Shield & Zero-Trust", sub: "Stateful packet firewall and threat quarantine vault", app: "shield", icon: "shield" },
            { title: "Self-Healing & Rollback Engine", sub: "Merkle tree integrity inspector and atomic rollback", app: "merkle", icon: "merkle" },
            { title: "Task Optimizer", sub: "Dynamic scheduler tuning (Game, Work, Stream, Eco)", app: "optimizer", icon: "optimizer" },
            { title: "7-Point System Diagnostics", sub: "Automated scan across memory, drivers, and network", app: "troubleshoot", icon: "troubleshoot" },
            { title: "Aegis App Store", sub: "Curated ring-3 sandboxed application repository", app: "store", icon: "store" },
            { title: "Settings Center", sub: "Configure ASLR, display layouts, and theme palettes", app: "settings", icon: "settings" },
            { title: "Aegis Voice Assistant", sub: "Voice command interpreter and audio agent", app: "voice", icon: "voice" },
            { title: "Switch Theme: Cyberpunk Amber", sub: "High-contrast glowing amber interface", action: () => setTheme('cyberpunk-amber'), icon: "theme" },
            { title: "Switch Theme: Quantum Cyan", sub: "Default futuristic cyan glass interface", action: () => setTheme('quantum-cyan'), icon: "theme" },
            { title: "Switch Layout: Windows Mode", sub: "Bottom taskbar with start menu and system tray", action: () => setLayout('windows'), icon: "layout" },
            { title: "Switch Layout: Aegis Hybrid", sub: "Floating dock with universal top HUD bar", action: () => setLayout('hybrid'), icon: "layout" }
        ],

        init() {
            this.overlay = document.getElementById('palette-overlay');
            this.input = document.getElementById('palette-input');
            this.results = document.getElementById('palette-results');

            // Keyboard trigger Ctrl+Space
            window.addEventListener('keydown', (e) => {
                if ((e.ctrlKey || e.metaKey) && e.code === 'Space') {
                    e.preventDefault();
                    this.toggle();
                } else if (e.key === 'Escape' && !this.overlay.classList.contains('hidden')) {
                    this.close();
                }
            });

            // Search input filtering
            this.input.addEventListener('input', () => this.render());

            // Arrow key navigation
            this.input.addEventListener('keydown', (e) => {
                const filtered = this.getFiltered();
                if (e.key === 'ArrowDown') {
                    this.selectedIndex = (this.selectedIndex + 1) % filtered.length;
                    this.render(false);
                    e.preventDefault();
                } else if (e.key === 'ArrowUp') {
                    this.selectedIndex = (this.selectedIndex - 1 + filtered.length) % filtered.length;
                    this.render(false);
                    e.preventDefault();
                } else if (e.key === 'Enter') {
                    if (filtered[this.selectedIndex]) {
                        this.execute(filtered[this.selectedIndex]);
                    }
                    e.preventDefault();
                }
            });

            // Click outside to close
            this.overlay.addEventListener('click', (e) => {
                if (e.target === this.overlay) this.close();
            });

            // Top bar search button
            document.getElementById('btn-palette')?.addEventListener('click', () => this.open());
        },

        open() {
            this.overlay.classList.remove('hidden');
            this.input.value = '';
            this.selectedIndex = 0;
            this.render();
            this.input.focus();
        },

        close() {
            this.overlay.classList.add('hidden');
        },

        toggle() {
            if (this.overlay.classList.contains('hidden')) this.open();
            else this.close();
        },

        getFiltered() {
            const query = this.input.value.toLowerCase().trim();
            if (!query) return this.items;
            return this.items.filter(item =>
                item.title.toLowerCase().includes(query) ||
                item.sub.toLowerCase().includes(query)
            );
        },

        render(resetIndex = true) {
            const filtered = this.getFiltered();
            if (resetIndex) this.selectedIndex = 0;

            if (filtered.length === 0) {
                this.results.innerHTML = '<div style="padding: 16px; text-align: center; color: var(--text-muted); font-size: 12px;">No matching system commands found.</div>';
                return;
            }

            this.results.innerHTML = filtered.map((item, idx) => `
                <div class="palette-item ${idx === this.selectedIndex ? 'selected' : ''}" data-idx="${idx}">
                    <div class="palette-icon">
                        <svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="10"/></svg>
                    </div>
                    <div class="palette-item-text">
                        <div class="palette-item-title">${item.title}</div>
                        <div class="palette-item-sub">${item.sub}</div>
                    </div>
                </div>
            `).join('');

            this.results.querySelectorAll('.palette-item').forEach(el => {
                el.addEventListener('click', () => {
                    const idx = parseInt(el.dataset.idx, 10);
                    if (filtered[idx]) this.execute(filtered[idx]);
                });
            });
        },

        execute(item) {
            this.close();
            if (item.action) {
                item.action();
            } else if (item.app) {
                WM.open(item.app, item.title);
            }
        }
    };

    // ==========================================
    // 15. THEME & LAYOUT DISPATCHERS
    // ==========================================
    function setTheme(theme) {
        document.body.classList.remove('theme-quantum-cyan', 'theme-cyberpunk-amber', 'theme-deep-space-dark', 'theme-crystal-light');
        document.body.classList.add(`theme-${theme}`);

        // Sync dropdowns
        const topSel = document.getElementById('theme-select');
        if (topSel) topSel.value = theme;
        const setSel = document.getElementById('settings-theme-select');
        if (setSel) setSel.value = theme;

        Notifications.show('Theme Applied', `Switched to ${theme.replace('-', ' ')}.`, 'info');
    }

    function setLayout(layout) {
        document.body.classList.remove('layout-hybrid', 'layout-windows', 'layout-linux');
        document.body.classList.add(`layout-${layout}`);

        // Sync dropdowns
        const topSel = document.getElementById('layout-select');
        if (topSel) topSel.value = layout;
        const setSel = document.getElementById('settings-layout-select');
        if (setSel) setSel.value = layout;

        Notifications.show('Layout Paradigm Changed', `Active Interface Mode: ${layout.toUpperCase()}`, 'info');
    }

    // ==========================================
    // 16. LIVE TELEMETRY & CLOCK UPDATER
    // ==========================================
    function startTelemetryLoop() {
        const topCpu = document.getElementById('top-cpu');
        const topCpuFill = document.getElementById('top-cpu-fill');
        const topRam = document.getElementById('top-ram');
        const topRamFill = document.getElementById('top-ram-fill');
        const hudClock = document.getElementById('hud-clock');
        const trayClock = document.getElementById('tray-clock');

        const tick = async () => {
            // Update Clocks
            const now = new Date();
            const timeStr = now.toLocaleTimeString();
            if (hudClock) hudClock.textContent = timeStr;
            if (trayClock) trayClock.textContent = timeStr.substring(0, 5);

            // Fetch live server telemetry or simulate jitter
            try {
                const res = await fetch('/api/neural/telemetry');
                if (res.ok) {
                    const data = await res.json();
                    const cpu = data.telemetry.cpuUsage;
                    const ram = data.telemetry.ramUsage;
                    if (topCpu) topCpu.textContent = `${cpu}%`;
                    if (topCpuFill) topCpuFill.style.width = `${cpu}%`;
                    if (topRam) topRam.textContent = `${ram}%`;
                    if (topRamFill) topRamFill.style.width = `${ram}%`;
                }
            } catch (e) {
                // Standalone local jitter fallback
                const randCpu = Math.floor(18 + Math.random() * 8);
                if (topCpu) topCpu.textContent = `${randCpu}%`;
                if (topCpuFill) topCpuFill.style.width = `${randCpu}%`;
            }
        };

        tick();
        setInterval(tick, 2000);
    }

    // Helper HTML escape
    function escapeHtml(str) {
        return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    }

    // ==========================================
    // 17. SYSTEM INITIALIZATION
    // ==========================================
    window.addEventListener('DOMContentLoaded', () => {
        // Init Subsystems
        Wallpaper.init();
        Notifications.init();
        WM.init();
        Palette.init();
        startTelemetryLoop();

        // Bind Desktop and Dock App Launchers
        document.querySelectorAll('[data-app]').forEach(btn => {
            btn.addEventListener('click', () => {
                const app = btn.dataset.app;
                const labels = {
                    terminal: 'Terminal (aegis_shell)',
                    neural: 'Neural Activity Monitor',
                    shield: 'Aegis Shield Firewall & Zero-Trust',
                    merkle: 'Self-Healing Merkle Filesystem',
                    optimizer: 'Aegis Task Optimizer',
                    troubleshoot: 'System Diagnostic Scanner',
                    store: 'Aegis App Store',
                    settings: 'Settings Center',
                    voice: 'Aegis Voice Assistant'
                };
                WM.open(app, labels[app] || app.toUpperCase());
            });
        });

        // Layout Dropdown
        document.getElementById('layout-select')?.addEventListener('change', (e) => {
            setLayout(e.target.value);
        });

        // Theme Dropdown
        document.getElementById('theme-select')?.addEventListener('change', (e) => {
            setTheme(e.target.value);
        });

        // Start Menu Toggle
        const startMenu = document.getElementById('start-menu');
        const startBtn = document.getElementById('dock-start-btn');
        const aegisBtn = document.getElementById('btn-aegis-menu');

        const toggleStart = (e) => {
            e.stopPropagation();
            startMenu.classList.toggle('hidden');
        };

        startBtn?.addEventListener('click', toggleStart);
        aegisBtn?.addEventListener('click', toggleStart);

        document.addEventListener('click', (e) => {
            if (startMenu && !startMenu.contains(e.target)) {
                startMenu.classList.add('hidden');
            }
        });

        // Start menu app launch
        startMenu?.querySelectorAll('.start-item[data-app]').forEach(item => {
            item.addEventListener('click', () => {
                startMenu.classList.add('hidden');
            });
        });

        // Architecture Demo Button in Start Menu
        document.getElementById('btn-run-demo')?.addEventListener('click', () => {
            startMenu.classList.add('hidden');
            WM.open('terminal', 'Terminal (aegis_shell)');
            setTimeout(() => {
                const termInput = document.querySelector('.term-input');
                if (termInput) {
                    termInput.value = 'sysinfo';
                    termInput.dispatchEvent(new KeyboardEvent('keydown', { key: 'Enter' }));
                }
            }, 300);
        });

        // Initial default open windows for high-impact experience
        setTimeout(() => {
            WM.open('neural', 'Neural Activity Monitor', 840, 520);
            WM.open('merkle', 'Self-Healing Merkle Filesystem', 780, 500);
            Notifications.show('AegisOS 2.0 Boot Verified', 'Dual-Kernel Microkernel active. Zero-Trust gate & Merkle filesystem verified healthy.', 'success', 5000);
        }, 400);
    });

})();

