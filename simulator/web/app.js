/**
 * AegisOS - Interactive Web Desktop Simulator Controller
 * Full-fidelity client-side environment for testing and demonstrating AegisOS architecture.
 * 
 * Features:
 * - Dynamic Window Compositor (WM)
 * - Aegis Mind Autonomous AI System Controller
 * - Cryptographic File Manager & Tiered Storage Optimizer
 * - Cybersecurity Arsenal (DPI Sniffer, Vulnerability Hunter, Malware Sandbox, Zero-Trust Broker)
 * - Self-Healing Merkle Filesystem (< 10s Atomic Rollback)
 * - Task Optimizer (Game/Work/Stream/Eco Modes)
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
                    osc.frequency.setValueAtTime(523.25, now);
                    osc.frequency.setValueAtTime(659.25, now + 0.08);
                    osc.frequency.setValueAtTime(783.99, now + 0.16);
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
            } catch (e) { }
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
    // Expose the original OS notification surface to native campaign events.
    window.AegisSystemNotify = (title, desc, type = 'info') => { if (!Notifications.container) Notifications.container = document.getElementById('toast-container'); if (Notifications.container) Notifications.show(title, desc, type); };

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

        open(appId, title, width = 740, height = 500) {
            window.dispatchEvent(new CustomEvent('aegis:wm-open', { detail: { app: appId, title } }));
            if (this.windows.has(appId)) {
                const win = this.windows.get(appId);
                if (win.classList.contains('minimized')) {
                    win.classList.remove('minimized');
                    this.bringToFront(win);
                } else if (win.classList.contains('active')) {
                    win.classList.add('minimized');
                } else {
                    this.bringToFront(win);
                }
                return;
            }

            const template = document.getElementById(`tmpl-${appId}`);
            if (!template) {
                Notifications.show('Launch Error', `Application '${appId}' template not found.`, 'error');
                return;
            }

            const win = document.createElement('div');
            win.className = 'aegis-window active';
            win.dataset.appId = appId;
            win.style.width = `${Math.min(width, window.innerWidth - 40)}px`;
            win.style.height = `${Math.min(height, window.innerHeight - 100)}px`;

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
                        <button class="win-btn win-btn-min" title="Minimize" aria-label="Minimize"></button>
                        <button class="win-btn win-btn-max" title="Maximize" aria-label="Maximize"></button>
                        <button class="win-btn win-btn-close" title="Close" aria-label="Close"></button>
                    </div>
                </div>
                <div class="win-body"></div>
            `;

            const winBody = win.querySelector('.win-body');
            winBody.appendChild(template.content.cloneNode(true));
            this.container.appendChild(win);
            this.windows.set(appId, win);

            this.updateDockState(appId, true);

            const winControls = win.querySelector('.win-controls');
            winControls.addEventListener('mousedown', (e) => e.stopPropagation());

            win.querySelector('.win-btn-close').addEventListener('click', (e) => {
                e.stopPropagation();
                this.close(appId);
            });
            win.querySelector('.win-btn-min').addEventListener('click', (e) => {
                e.stopPropagation();
                if ('speechSynthesis' in window) {
                    try { window.speechSynthesis.cancel(); } catch (err) { }
                }
                win.classList.add('minimized');
            });
            win.querySelector('.win-btn-max').addEventListener('click', (e) => {
                e.stopPropagation();
                win.classList.toggle('maximized');
            });
            win.querySelector('.win-header').addEventListener('dblclick', (e) => {
                if (e.target.closest('.win-controls')) return;
                win.classList.toggle('maximized');
            });

            win.addEventListener('mousedown', () => this.bringToFront(win));
            this.setupDrag(win);
            this.initAppLogic(appId, win);
            AudioEngine.playChime('info');
        },

        close(appId) {
            if (this.windows.has(appId)) {
                // Immediately stop all speech synthesis when any window / agent is closed
                if ('speechSynthesis' in window) {
                    try { window.speechSynthesis.cancel(); } catch (e) { }
                }

                // If Quantum Ghost Browser is closed, safely shred the ephemeral RAM container
                if (appId === 'browser') {
                    BrowserApp.destroyCurrentContainer();
                }

                const win = this.windows.get(appId);
                win.dispatchEvent(new CustomEvent('aegis-close'));
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
                // Never trigger drag if user clicked window control buttons, inputs, or interactive controls
                if (e.target.closest('.win-controls') || e.target.closest('.win-btn') || e.target.closest('button') || e.target.closest('input') || e.target.closest('select')) {
                    return;
                }
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
                case 'browser': BrowserApp.init(win); break;
                case 'ai': AIApp.init(win); break;
                case 'files': FilesApp.init(win); break;
                case 'cybersec': CyberSecApp.init(win); break;
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
    // 4.5. QUANTUM GHOST BROWSER CONTROLLER
    // ==========================================
    const BrowserApp = {
        activeWin: null,
        currentContainerId: null,

        async init(win) {
            this.activeWin = win;
            const urlInput = win.querySelector('#browser-url-input');
            const btnGo = win.querySelector('#btn-browser-go');
            const btnBack = win.querySelector('#btn-browser-back');
            const btnForward = win.querySelector('#btn-browser-forward');
            const btnReload = win.querySelector('#btn-browser-reload');
            const btnHome = win.querySelector('#btn-browser-home');
            const btnBookmark = win.querySelector('#btn-browser-bookmark');
            const btnShred = win.querySelector('#btn-browser-shred');
            const containerBadge = win.querySelector('#browser-container-badge');
            const ramBadge = win.querySelector('#browser-ram-badge');
            const bookmarksList = win.querySelector('#browser-bookmarks-list');
            const iframe = win.querySelector('#browser-viewport-frame');
            const fallbackPage = win.querySelector('#browser-fallback-page');
            const fallbackInput = win.querySelector('#fallback-search-input');
            const btnFallbackSearch = win.querySelector('#btn-fallback-search');

            // 1. Provision Ephemeral RAM Container
            await this.spawnContainer(containerBadge, ramBadge);

            // 2. Load Host-Persisted Bookmarks
            const loadBookmarks = async () => {
                try {
                    const res = await fetch('/api/browser/status');
                    const data = await res.json();
                    const bms = data.browser?.persistentBookmarksOnHost || [
                        { title: "DuckDuckGo", url: "https://duckduckgo.com" },
                        { title: "Wikipedia", url: "https://en.wikipedia.org" },
                        { title: "Kernel.org", url: "https://kernel.org" },
                        { title: "Hacker News", url: "https://news.ycombinator.com" }
                    ];

                    bookmarksList.innerHTML = bms.map(bm => `
                        <div class="bookmark-pill" data-url="${bm.url}" title="Open ${escapeHtml(bm.url)} in ephemeral sandbox">
                            <span>🌐</span>
                            <span>${escapeHtml(bm.title)}</span>
                        </div>
                    `).join('');

                    bookmarksList.querySelectorAll('.bookmark-pill').forEach(pill => {
                        pill.addEventListener('click', () => {
                            navigateTo(pill.dataset.url);
                        });
                    });
                } catch (e) { }
            };
            loadBookmarks();

            // 3. Navigation Engine
            const navigateTo = (rawUrl) => {
                if (!rawUrl) return;
                let targetUrl = rawUrl.trim();

                // Convert keyword search to secure DuckDuckGo search query
                if (!targetUrl.startsWith('http://') && !targetUrl.startsWith('https://')) {
                    if (targetUrl.includes('.') && !targetUrl.includes(' ')) {
                        targetUrl = 'https://' + targetUrl;
                    } else {
                        targetUrl = `https://duckduckgo.com/?q=${encodeURIComponent(targetUrl)}`;
                    }
                }

                urlInput.value = targetUrl;
                iframe.src = targetUrl;

                // Handle cross-origin iframe security fallback gracefully
                iframe.onerror = () => {
                    fallbackPage.classList.remove('hidden');
                };
                fallbackPage.classList.add('hidden');

                // Jitter threat telemetry to illustrate real-time container interception
                const cookies = win.querySelector('#browser-cookies-count');
                const trackers = win.querySelector('#browser-trackers-count');
                const scripts = win.querySelector('#browser-scripts-count');
                if (cookies) cookies.textContent = Math.floor(10 + Math.random() * 25);
                if (trackers) trackers.textContent = Math.floor(20 + Math.random() * 40);
                if (scripts) scripts.textContent = Math.floor(4 + Math.random() * 12);

                try {
                    const host = new URL(targetUrl).hostname || targetUrl;
                    Notifications.show('RAM Container Routing', `Sandboxed HTTP connection established: ${host}`, 'info', 2500);
                } catch (e) { }
            };

            btnGo.addEventListener('click', () => navigateTo(urlInput.value));
            urlInput.addEventListener('keydown', (e) => {
                if (e.key === 'Enter') navigateTo(urlInput.value);
            });

            btnBack.addEventListener('click', () => {
                try { iframe.contentWindow.history.back(); } catch (e) { }
            });
            btnForward.addEventListener('click', () => {
                try { iframe.contentWindow.history.forward(); } catch (e) { }
            });
            btnReload.addEventListener('click', () => {
                iframe.src = urlInput.value || 'https://duckduckgo.com';
                AudioEngine.playChime('info');
            });
            btnHome.addEventListener('click', () => {
                navigateTo('https://duckduckgo.com');
            });

            // 4. Save Bookmark to Host Vault (Not ephemeral)
            btnBookmark.addEventListener('click', async () => {
                const url = urlInput.value || 'https://duckduckgo.com';
                let defaultTitle = url;
                try { defaultTitle = new URL(url).hostname; } catch (e) { }
                const title = prompt("Save bookmark to Host Vault (/vault/browser_data/):", defaultTitle);
                if (!title) return;

                const res = await fetch('/api/browser/save-bookmark', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ title, url })
                });
                const data = await res.json();
                if (data.success) {
                    Notifications.show('Host Vault Synchronized', `Saved bookmark "${title}" safely on host. Outbound browser cannot access host disks.`, 'success');
                    loadBookmarks();
                }
            });

            // 5. Shred & Re-Roll Container Action
            btnShred.addEventListener('click', async () => {
                await this.shredAndReroll(win, containerBadge, ramBadge);
            });

            // 6. Fallback Search Box
            btnFallbackSearch?.addEventListener('click', () => {
                if (fallbackInput.value.trim()) {
                    navigateTo(fallbackInput.value.trim());
                }
            });
            fallbackInput?.addEventListener('keydown', (e) => {
                if (e.key === 'Enter' && fallbackInput.value.trim()) {
                    navigateTo(fallbackInput.value.trim());
                }
            });

            win.querySelectorAll('.quicklink-btn').forEach(btn => {
                btn.addEventListener('click', () => navigateTo(btn.dataset.url));
            });
        },

        async spawnContainer(badgeEl, ramEl) {
            try {
                const res = await fetch('/api/browser/new-container', { method: 'POST' });
                const data = await res.json();
                this.currentContainerId = data.containerId;
                if (badgeEl) badgeEl.textContent = `MICRO-VM: ${data.containerId}`;
                if (ramEl) ramEl.textContent = `RAM-ISOLATED (${data.ramAllocatedMb || 128} MB VOLATILE)`;
            } catch (e) {
                this.currentContainerId = `MICRO-VM-GHOST-${Math.floor(Math.random() * 90000) + 10000}`;
                if (badgeEl) badgeEl.textContent = `MICRO-VM: ${this.currentContainerId}`;
            }
        },

        async shredAndReroll(win, badgeEl, ramEl) {
            AudioEngine.playChime('alert');
            try {
                const res = await fetch('/api/browser/destroy-container', { method: 'POST' });
                const data = await res.json();
                Notifications.show('Container Shredded', `${data.shreddedContainerId} incinerated with 3-Pass 0x00 wipe. Memory scrubbed.`, 'warn');
            } catch (e) { }

            // Re-spawn new container
            await this.spawnContainer(badgeEl, ramEl);
            AudioEngine.playChime('success');
            Notifications.show('Fresh Sandbox Provisioned', `New isolated RAM container online: ${this.currentContainerId}`, 'success');

            const iframe = win.querySelector('#browser-viewport-frame');
            if (iframe) iframe.src = 'https://duckduckgo.com';
            const urlInput = win.querySelector('#browser-url-input');
            if (urlInput) urlInput.value = 'https://duckduckgo.com';
        },

        async destroyCurrentContainer() {
            try {
                const res = await fetch('/api/browser/destroy-container', { method: 'POST' });
                const data = await res.json();
                Notifications.show('Ghost Browser Vaporized', `Container ${data.shreddedContainerId || 'MICRO-VM'} purged with 3-Pass 0x00 wipe. Zero host footprint.`, 'info');
            } catch (e) { }
            this.currentContainerId = null;
        }
    };

    // ==========================================
    // 5. AGENTIC AI CONTROLLER ("Aegis Mind")
    // ==========================================
    const AIApp = {
        init(win) {
            const chatBox = win.querySelector('#ai-chat-stream');
            const input = win.querySelector('#ai-user-input');
            const btnSend = win.querySelector('#btn-ai-send');

            const executePrompt = async (promptText) => {
                if (!promptText) return;
                document.dispatchEvent(new CustomEvent('aegis:ai-prompt', { detail: { prompt: promptText } }));

                // Append user message
                const userMsg = document.createElement('div');
                userMsg.className = 'ai-msg ai-msg-user';
                userMsg.innerHTML = `<div class="msg-bubble">${escapeHtml(promptText)}</div>`;
                chatBox.appendChild(userMsg);

                input.value = '';
                chatBox.scrollTop = chatBox.scrollHeight;

                // Typing indicator
                const botTyping = document.createElement('div');
                botTyping.className = 'ai-msg ai-msg-bot';
                botTyping.innerHTML = `<div class="msg-bubble" style="color: #ffaa00;">Aegis Mind reasoning and executing system actions...</div>`;
                chatBox.appendChild(botTyping);
                chatBox.scrollTop = chatBox.scrollHeight;

                try {
                    const res = await fetch('/api/ai/execute', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ prompt: promptText })
                    });
                    const data = await res.json();
                    botTyping.remove();

                    // Render Bot Reply
                    const botMsg = document.createElement('div');
                    botMsg.className = 'ai-msg ai-msg-bot';

                    let actionsHtml = '';
                    if (data.actionsExecuted && data.actionsExecuted.length) {
                        actionsHtml = `
                            <div style="margin-top: 8px; border-top: 1px solid rgba(255, 170, 0, 0.2); padding-top: 6px;">
                                <div style="font-size: 10px; font-weight: 700; color: #ffaa00; margin-bottom: 4px;">ACTIONS EXECUTED:</div>
                                ${data.actionsExecuted.map(a => `
                                    <div class="ai-action-step">
                                        <span style="color: #22dd66;">✔</span>
                                        <strong>${escapeHtml(a.task)}</strong>: <span style="color: var(--text-muted);">${escapeHtml(a.result)}</span>
                                    </div>
                                `).join('')}
                            </div>
                        `;
                    }

                    let followupsHtml = '';
                    if (data.suggestedFollowups && data.suggestedFollowups.length) {
                        followupsHtml = `
                            <div style="display: flex; flex-wrap: wrap; gap: 4px; margin-top: 8px;">
                                ${data.suggestedFollowups.map(f => `<button class="ai-pill followup-pill" data-text="${escapeHtml(f)}">➜ ${escapeHtml(f)}</button>`).join('')}
                            </div>
                        `;
                    }

                    botMsg.innerHTML = `
                        <div class="msg-bubble">
                            ${data.reply.replace(/\n/g, '<br>')}
                            ${actionsHtml}
                            ${followupsHtml}
                        </div>
                    `;
                    chatBox.appendChild(botMsg);
                    chatBox.scrollTop = chatBox.scrollHeight;

                    // Bind follow-up buttons
                    botMsg.querySelectorAll('.followup-pill').forEach(btn => {
                        btn.addEventListener('click', () => executePrompt(btn.dataset.text));
                    });

                    AudioEngine.playChime('success');

                    // Voice output if available - only if AI window is still open
                    if (WM.windows.has('ai') && !WM.windows.get('ai').classList.contains('minimized') && 'speechSynthesis' in window) {
                        try {
                            const utter = new SpeechSynthesisUtterance(data.reply.split('\n')[0]);
                            utter.rate = 1.05;
                            window.speechSynthesis.speak(utter);
                        } catch (e) { }
                    }
                } catch (err) {
                    botTyping.remove();
                    const errMsg = document.createElement('div');
                    errMsg.className = 'ai-msg ai-msg-bot';
                    errMsg.innerHTML = `<div class="msg-bubble" style="color: var(--status-err);">Error contacting Aegis Mind AI: ${err.message}</div>`;
                    chatBox.appendChild(errMsg);
                }
            };

            btnSend.addEventListener('click', () => executePrompt(input.value.trim()));
            input.addEventListener('keydown', (e) => {
                if (e.key === 'Enter') executePrompt(input.value.trim());
            });

            // Quick action pills
            win.querySelectorAll('.ai-pill[data-prompt]').forEach(btn => {
                btn.addEventListener('click', () => executePrompt(btn.dataset.prompt));
            });
        }
    };

    // ==========================================
    // 6. FILES & TIERED STORAGE CONTROLLER
    // ==========================================
    const FilesApp = {
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

            const tableBody = win.querySelector('#files-table-body');
            const searchInput = win.querySelector('#files-search-input');
            const btnRefresh = win.querySelector('#btn-refresh-files');
            const btnCreate = win.querySelector('#btn-create-file-modal');
            const btnTrim = win.querySelector('#btn-run-trim-optimize');
            const tiersContainer = win.querySelector('#storage-tiers-container');

            // File Editor DOM elements
            const editorPanel = win.querySelector('#file-editor-panel');
            const editorPath = win.querySelector('#editor-file-path');
            const editorMode = win.querySelector('#editor-mode-badge');
            const editorEnc = win.querySelector('#editor-enc-check');
            const editorTextarea = win.querySelector('#editor-textarea');
            const editorLineNumbers = win.querySelector('#editor-line-numbers');
            const editorStats = win.querySelector('#editor-stats');
            const editorHash = win.querySelector('#editor-hash');
            const btnSaveFile = win.querySelector('#btn-save-file-editor');
            const btnCloseEditor = win.querySelector('#btn-close-file-editor');

            let cachedFiles = [];
            let activeEditingFileId = null;

            // Line numbers and character counter
            const updateEditorStats = () => {
                const text = editorTextarea.value;
                const lines = text.split('\n').length;
                let lineNums = '';
                for (let i = 1; i <= lines; i++) {
                    lineNums += i + '\n';
                }
                editorLineNumbers.textContent = lineNums;
                const chars = text.length;
                const sizeBytes = new Blob([text]).size;
                const sizeStr = sizeBytes >= 1024 ? `${(sizeBytes / 1024).toFixed(1)} KB` : `${sizeBytes} B`;
                editorStats.textContent = `Lines: ${lines} | Characters: ${chars} | Size: ${sizeStr}`;
            };

            editorTextarea.addEventListener('input', updateEditorStats);
            editorTextarea.addEventListener('scroll', () => {
                editorLineNumbers.scrollTop = editorTextarea.scrollTop;
            });

            const renderFiles = (filter = '') => {
                const filtered = cachedFiles.filter(f =>
                    f.name.toLowerCase().includes(filter.toLowerCase()) ||
                    f.path.toLowerCase().includes(filter.toLowerCase()) ||
                    f.hash.toLowerCase().includes(filter.toLowerCase())
                );

                tableBody.innerHTML = filtered.map(f => `
                    <tr>
                        <td>
                            <strong>${escapeHtml(f.name)}</strong>
                            <div style="font-size: 10px; color: var(--text-muted); font-family: var(--font-mono);">${escapeHtml(f.path)}</div>
                        </td>
                        <td>${f.sizeStr}</td>
                        <td><span class="status-pill ${f.ring === 'Ring 0' ? 'pill-blocked' : 'pill-secure'}">${f.ring}</span></td>
                        <td><code>${f.perm}</code></td>
                        <td>
                            <button class="btn-action-small btn-toggle-enc" data-id="${f.id}" style="font-size: 10px;">
                                ${f.encrypted ? '🔒 AES-256 (Encrypted)' : '🔓 Plaintext'}
                            </button>
                        </td>
                        <td><code style="font-size: 10px; color: var(--accent);">${f.hash}</code></td>
                        <td>
                            <div style="display: flex; gap: 6px;">
                                <button class="btn-action-small btn-edit-file" data-id="${f.id}" data-path="${escapeHtml(f.path)}" style="color: var(--accent); border-color: var(--accent);">✏️ Edit</button>
                                <button class="btn-action-small btn-del-file" data-id="${f.id}" style="color: var(--status-err); border-color: var(--status-err);">Delete</button>
                            </div>
                        </td>
                    </tr>
                `).join('');

                // Open existing file in Editor
                tableBody.querySelectorAll('.btn-edit-file').forEach(b => {
                    b.addEventListener('click', async () => {
                        const fileId = b.dataset.id;
                        try {
                            const res = await fetch(`/api/files/read?id=${encodeURIComponent(fileId)}`);
                            const data = await res.json();
                            if (data.file) {
                                activeEditingFileId = data.file.id;
                                editorMode.textContent = `INODE: ${data.file.name}`;
                                editorPath.value = data.file.path;
                                editorEnc.checked = !!data.file.encrypted;
                                editorTextarea.value = data.file.content || '';
                                editorHash.textContent = `Merkle Leaf: 0x${data.file.hash}`;
                                updateEditorStats();
                                editorPanel.classList.remove('hidden');
                                editorTextarea.focus();
                                Notifications.show('File Loaded', `Opened ${data.file.name} in Aegis Editor.`, 'info', 2000);
                            }
                        } catch (e) {
                            Notifications.show('Read Error', `Unable to read inode ${fileId}`, 'error');
                        }
                    });
                });

                tableBody.querySelectorAll('.btn-toggle-enc').forEach(b => {
                    b.addEventListener('click', async () => {
                        const res = await fetch('/api/files/toggle-encryption', {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify({ fileId: b.dataset.id })
                        });
                        const data = await res.json();
                        if (data.success) {
                            Notifications.show('AegisFS Encryption', `File ${data.file.name}: ${data.cipher}`, 'success');
                            loadFiles();
                        }
                    });
                });

                tableBody.querySelectorAll('.btn-del-file').forEach(b => {
                    b.addEventListener('click', async () => {
                        const res = await fetch('/api/files/delete', {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify({ fileId: b.dataset.id })
                        });
                        const data = await res.json();
                        if (data.success) {
                            Notifications.show('File Deleted', `Removed inode ${data.deletedFile.name} safely.`, 'info');
                            loadFiles();
                        }
                    });
                });
            };

            const loadFiles = async () => {
                try {
                    const res = await fetch('/api/files/list');
                    const data = await res.json();
                    cachedFiles = data.files || [];
                    renderFiles(searchInput.value);
                } catch (e) { }
            };
            loadFiles();

            searchInput.addEventListener('input', () => renderFiles(searchInput.value));
            btnRefresh.addEventListener('click', loadFiles);

            // Create New File Composer via in-window Editor
            btnCreate.addEventListener('click', () => {
                activeEditingFileId = null;
                const randSuffix = Math.floor(1000 + Math.random() * 9000);
                editorMode.textContent = "NEW INODE COMPOSER";
                editorPath.value = `/userland/data/aegis_doc_${randSuffix}.txt`;
                editorEnc.checked = false;
                editorTextarea.value = `// AegisOS Cryptographic Protected Document\n// Inode Path: /userland/data/aegis_doc_${randSuffix}.txt\n// Timestamp: ${new Date().toISOString()}\n\n# AegisFS Document\nEnter payload or script contents here.\n`;
                editorHash.textContent = "Merkle Leaf: Uncommitted Block";
                updateEditorStats();
                editorPanel.classList.remove('hidden');
                editorTextarea.focus();
            });

            // Save File Action
            btnSaveFile.addEventListener('click', async () => {
                const path = editorPath.value.trim();
                if (!path) return Notifications.show('File Error', 'File path is required.', 'error');
                const content = editorTextarea.value;
                const encrypted = editorEnc.checked;

                try {
                    const res = await fetch('/api/files/write', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({
                            path,
                            content,
                            fileId: activeEditingFileId,
                            encrypted
                        })
                    });
                    const data = await res.json();
                    if (data.success) {
                        activeEditingFileId = data.file.id;
                        editorHash.textContent = `Merkle Leaf: 0x${data.file.hash}`;
                        editorMode.textContent = `INODE: ${data.file.name}`;
                        Notifications.show('AegisFS Write Success', `Saved ${data.file.name} to persistent storage.`, 'success');
                        loadFiles();
                    } else {
                        Notifications.show('Write Failed', data.error || 'Could not save file.', 'error');
                    }
                } catch (err) {
                    Notifications.show('Write Error', err.message, 'error');
                }
            });

            // Close Editor Drawer
            btnCloseEditor.addEventListener('click', () => {
                editorPanel.classList.add('hidden');
            });

            // Tiered storage render
            const loadStorageTiers = async () => {
                try {
                    const res = await fetch('/api/storage/info');
                    const data = await res.json();
                    const s = data.storage;

                    tiersContainer.innerHTML = s.tiers.map(t => {
                        const pct = Math.round((t.usedGb / t.capacityGb) * 100);
                        return `
                            <div class="tier-card">
                                <div class="tier-card-top">
                                    <div>
                                        <div class="tier-name">${t.name}</div>
                                        <div class="tier-type">${t.type}</div>
                                    </div>
                                    <span class="status-pill pill-secure">${t.status}</span>
                                </div>
                                <div class="tier-bar-track">
                                    <div class="tier-bar-fill" style="width: ${pct}%;"></div>
                                </div>
                                <div class="tier-specs">
                                    <span>${t.usedGb} / ${t.capacityGb} GB (${pct}%)</span>
                                    <span>Latency: ${t.latencyUs} µs</span>
                                    <span>${t.bandwidthMbps} MB/s</span>
                                </div>
                            </div>
                        `;
                    }).join('');
                } catch (e) { }
            };
            loadStorageTiers();

            // Run TRIM & Deduplication
            btnTrim.addEventListener('click', async () => {
                btnTrim.disabled = true;
                btnTrim.textContent = "Running Deduplication & TRIM...";
                try {
                    const res = await fetch('/api/storage/optimize', { method: 'POST' });
                    const data = await res.json();
                    Notifications.show('Storage Optimized', data.message, 'success');
                    loadStorageTiers();
                } catch (e) { }
                btnTrim.disabled = false;
                btnTrim.innerHTML = `<svg viewBox="0 0 24 24"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg> Run Deduplication & TRIM Optimizer`;
            });
        }
    };

    // ==========================================
    // 7. CYBERSECURITY ARSENAL CONTROLLER
    // ==========================================
    const CyberSecApp = {
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

            // 1. DPI Sniffer
            const dpiFeed = win.querySelector('#dpi-feed-list');
            const btnPauseSniffer = win.querySelector('#btn-toggle-sniffer');
            const btnClearSniffer = win.querySelector('#btn-clear-sniffer');
            let snifferActive = true;

            const pollPackets = async () => {
                if (!snifferActive || !document.body.contains(win)) {
                    clearInterval(snifferTimer);
                    return;
                }
                try {
                    const res = await fetch('/api/cybersec/packets');
                    const data = await res.json();
                    data.packets.forEach(p => {
                        const row = document.createElement('div');
                        row.className = `packet-row ${p.status === 'FLAGGED_INSPECTION' ? 'packet-flagged' : ''}`;
                        row.innerHTML = `
                            <span>${p.timestamp}</span>
                            <strong>${p.protocol}</strong>
                            <span>${p.srcIp}</span>
                            <span>${p.dstIp}:${p.port}</span>
                            <span style="color: ${p.status === 'ALLOWED' ? 'var(--status-ok)' : 'var(--status-err)'};">${p.status}</span>
                            <span style="font-size: 10px; color: var(--text-muted);">${p.payloadFlag}</span>
                        `;
                        dpiFeed.prepend(row);
                        if (dpiFeed.children.length > 30) dpiFeed.lastChild.remove();
                    });
                } catch (e) { }
            };

            for (let i = 0; i < 3; i++) pollPackets();
            const snifferTimer = setInterval(pollPackets, 2000);
            win.addEventListener('aegis-close', () => clearInterval(snifferTimer));

            btnPauseSniffer.addEventListener('click', () => {
                snifferActive = !snifferActive;
                btnPauseSniffer.textContent = snifferActive ? "Pause Sniffer" : "Resume Sniffer";
            });
            btnClearSniffer.addEventListener('click', () => dpiFeed.innerHTML = '');

            // 2. Vulnerability Scan
            const btnVuln = win.querySelector('#btn-run-vuln-scan');
            const vulnList = win.querySelector('#vuln-results-list');

            btnVuln.addEventListener('click', async () => {
                vulnList.innerHTML = `<div style="padding: 16px; text-align: center; color: var(--accent);">Running Deep Vulnerability Assessment across Kernel & Ring-3 Sandboxes...</div>`;
                try {
                    const res = await fetch('/api/cybersec/vulnerability-scan', { method: 'POST' });
                    const data = await res.json();
                    vulnList.innerHTML = data.auditPoints.map(p => `
                        <div class="diag-item">
                            <div class="status-pill pill-secure">${p.status}</div>
                            <div>
                                <div class="diag-name">${p.title}</div>
                                <div class="diag-msg">${p.detail}</div>
                            </div>
                        </div>
                    `).join('') + `
                        <div style="margin-top: 10px; padding: 10px; background: rgba(0, 255, 170, 0.05); border: 1px solid var(--status-ok); border-radius: 8px;">
                            <strong>Aegis Hardening Score: ${data.systemHardenScore}</strong>
                            <div style="font-size: 11px; color: var(--text-muted);">Zero active zero-day vulnerabilities. All Ring 3 DMA boundaries enforced.</div>
                        </div>
                    `;
                    Notifications.show('CyberSec Audit Complete', 'All 6 microkernel mitigation layers verified robust.', 'success');
                } catch (e) { }
            });

            // 3. Malware Sandbox Detonator
            const btnDetonate = win.querySelector('#btn-detonate-sample');
            const sampleSelect = win.querySelector('#sandbox-sample-select');
            const sandboxReport = win.querySelector('#sandbox-report');

            btnDetonate.addEventListener('click', async () => {
                sandboxReport.innerHTML = `<div style="text-align: center; color: var(--status-err); padding: 20px;">Detonating payload in isolated microkernel sandbox... Analyzing heap & syscall anomalies...</div>`;
                try {
                    const res = await fetch('/api/cybersec/sandbox-detonate', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ sampleName: sampleSelect.value })
                    });
                    const data = await res.json();
                    sandboxReport.innerHTML = `
                        <div style="border-left: 4px solid var(--status-err); padding-left: 12px; margin-bottom: 12px;">
                            <div style="font-size: 14px; font-weight: 700; color: var(--status-err);">THREAT ISOLATED: ${data.sampleName} (Confidence: ${data.threatConfidence})</div>
                            <div style="font-size: 11px; color: var(--text-muted);">Sandbox PID: ${data.sandboxPid} | Vault ID: ${data.quarantineId}</div>
                        </div>
                        <div style="font-size: 11px; font-weight: 700; color: #ffaa00; margin-bottom: 6px;">BEHAVIORAL ANOMALIES INTERCEPTED:</div>
                        <ul style="font-size: 11px; line-height: 1.6; padding-left: 18px; color: var(--text-primary);">
                            ${data.detectedBehaviors.map(b => `<li>${b}</li>`).join('')}
                        </ul>
                        <div style="margin-top: 12px; padding: 8px 12px; background: rgba(34, 221, 102, 0.15); border: 1px solid var(--status-ok); border-radius: 6px; font-size: 11px; color: var(--status-ok); font-weight: 600;">
                            ACTION: ${data.actionTaken}
                        </div>
                    `;
                    Notifications.show('Threat Neutralized', `Sample ${data.sampleName} was detonated and isolated with 0 system impact.`, 'error');
                } catch (e) { }
            });

            // 4. Zero-Trust Token Broker
            const btnIssueToken = win.querySelector('#btn-issue-zt-token');
            const subjectInput = win.querySelector('#zt-subject-input');
            const capSelect = win.querySelector('#zt-cap-select');
            const tokenTable = win.querySelector('#zt-tokens-table-body');

            const tokens = [
                { id: "TOK-9921", subject: "aegis_ui_compositor", cap: "CAP_FRAMEBUFFER_WRITE", ttl: "3600s", status: "VALID" },
                { id: "TOK-9922", subject: "neural_monitor", cap: "CAP_KERNEL_TELEMETRY", ttl: "1800s", status: "VALID" },
                { id: "TOK-9923", subject: "aegis_shield_daemon", cap: "CAP_NET_RAW_SOCKET", ttl: "7200s", status: "VALID" }
            ];

            const renderTokens = () => {
                tokenTable.innerHTML = tokens.map(t => `
                    <tr>
                        <td><strong>${t.id}</strong></td>
                        <td><code>${t.subject}</code></td>
                        <td><span class="status-pill pill-secure">${t.cap}</span></td>
                        <td>${t.ttl}</td>
                        <td><span class="status-pill pill-secure">● ${t.status}</span></td>
                    </tr>
                `).join('');
            };
            renderTokens();

            btnIssueToken.addEventListener('click', () => {
                const subj = subjectInput.value.trim() || 'user_service';
                const cap = capSelect.value;
                tokens.unshift({
                    id: `TOK-${Math.floor(Math.random() * 9000) + 1000}`,
                    subject: subj,
                    cap: cap,
                    ttl: "300s (5m)",
                    status: "VALID"
                });
                renderTokens();
                subjectInput.value = '';
                Notifications.show('Zero-Trust Token Issued', `Capability ${cap} granted to ${subj}.`, 'success');
            });
        }
    };

    // ==========================================
    // 8. TERMINAL APPLICATION CONTROLLER
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
                    document.dispatchEvent(new CustomEvent('aegis:terminal-command', { detail: { command: cmd } }));
                    historyIndex = commandHistory.length;

                    const line = document.createElement('div');
                    line.className = 'term-line';
                    line.innerHTML = `<span class="prompt-prefix">aegis@kernel-ring0:~$</span> ${escapeHtml(cmd)}`;
                    history.appendChild(line);

                    input.value = '';

                    if (cmd.toLowerCase() === 'clear') {
                        history.innerHTML = '';
                        return;
                    }

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
                        outLine.textContent = (data.output || 'Execution complete.').replace(/\\n/g, '\n');
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
                    const list = ['help', 'sysinfo', 'ps', 'cat', 'touch', 'write', 'rm', 'shred', 'encrypt', 'ls', 'whoami', 'date', 'uname', 'uptime', 'top', 'df', 'free', 'netstat', 'ping', 'curl', 'browser', 'shield', 'guard', 'ztac', 'troubleshoot', 'opt', 'checkpoint', 'neural', 'fs', 'storage', 'cybersec', 'ai', 'theme', 'layout', 'clear'];
                    const val = input.value.toLowerCase();
                    const match = list.find(c => c.startsWith(val));
                    if (match) input.value = match;
                }
            });
        }
    };

    // ==========================================
    // 9. NEURAL ACTIVITY MONITOR CONTROLLER
    // ==========================================
    const NeuralApp = {
        init(win) {
            const canvas = win.querySelector('#neural-wave-canvas');
            if (!canvas) return;
            const ctx = canvas.getContext('2d');
            const eventsList = win.querySelector('#neural-events-list');

            let step = 0;
            const drawWave = () => {
                if (!document.body.contains(win)) return;
                requestAnimationFrame(drawWave);
                step += 0.05;

                ctx.fillStyle = 'rgba(6, 11, 20, 0.25)';
                ctx.fillRect(0, 0, canvas.width, canvas.height);

                ctx.beginPath();
                ctx.strokeStyle = '#00f3ff';
                ctx.lineWidth = 2;
                for (let x = 0; x < canvas.width; x++) {
                    const y = (canvas.height / 2) + Math.sin(x * 0.03 + step) * 25 + Math.cos(x * 0.015 - step * 1.5) * 15;
                    if (x === 0) ctx.moveTo(x, y);
                    else ctx.lineTo(x, y);
                }
                ctx.stroke();

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

            const eventPool = [
                { type: 'FS_READ', desc: '/system/lib/libaegis.so (Page #402)', status: 'ALLOW' },
                { type: 'MEM_VERIFY', desc: 'Canary checked on PID 24 (Stack OK)', status: 'ALLOW' },
                { type: 'NET_FILTER', desc: 'Stateful inspection TLS 1.3 frame', status: 'ALLOW' },
                { type: 'ZT_GATE', desc: 'Kernel broker verified ephemeral token', status: 'ALLOW' },
                { type: 'HEURISTIC', desc: 'Guard scan: Entropy score 0.89 normal', status: 'ALLOW' }
            ];

            const addEvent = () => {
                if (!document.body.contains(win)) {
                    clearInterval(timer);
                    return;
                }
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
            win.addEventListener('aegis-close', () => clearInterval(timer));
        }
    };

    // ==========================================
    // 10. AEGIS SHIELD & ZERO-TRUST CONTROLLER
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

            const tbody = win.querySelector('#firewall-rules-body');
            const rules = [
                { id: 1, name: "Block Inbound Telnet", proto: "TCP", port: 23, dir: "INBOUND", action: "DENY", enabled: true },
                { id: 2, name: "Allow Secure Shell (SSH)", proto: "TCP", port: 22, dir: "INBOUND", action: "ALLOW", enabled: true },
                { id: 3, name: "Allow HTTPS Web Traffic", proto: "TCP", port: 443, dir: "OUTBOUND", action: "ALLOW", enabled: true },
                { id: 4, name: "Block Outbound Tor Relays", proto: "TCP", port: 9001, dir: "OUTBOUND", action: "DENY", enabled: true },
                { id: 5, name: "Zero-Trust Kernel IPC Gate", proto: "IPC", port: 0, dir: "INTERNAL", action: "ALLOW", enabled: true },
                { id: 6, name: "Block Unverified DNS over HTTP", proto: "UDP", port: 853, dir: "OUTBOUND", action: "DENY", enabled: true },
                { id: 7, name: "Block Suspected C2 Endpoint (185.220.101.5:8443)", proto: "TCP", port: 8443, dir: "OUTBOUND", action: "DENY", enabled: false },
                { id: 8, name: "Adaptive Ingress Shaping (Priority Edge)", proto: "TCP", port: 443, dir: "INBOUND", action: "RATE-LIMIT", enabled: false }
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

            win.querySelector('#btn-scan-guard').addEventListener('click', async () => {
                Notifications.show('Aegis Guard', 'Executing AI heuristic intrusion detection scan...', 'info');
                try {
                    const res = await fetch('/api/guard/scan', { method: 'POST' });
                    const data = await res.json();
                    Notifications.show('Guard Scan Complete', `Scanned ${data.scannedProcesses} processes. 0 unmitigated intrusions. ML Accuracy: ${data.accuracy}%.`, 'success');
                } catch (e) { }
            });

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
                <div class="zt-item">
                    <div>
                        <strong style="color: var(--status-err);">THR-8823: nop_sled_shellcode.raw</strong>
                        <div class="zt-desc">Buffer overflow payload intercepted by SPI hardware filter.</div>
                    </div>
                    <span class="status-pill pill-blocked">QUARANTINED</span>
                </div>
            `;
        }
    };

    // ==========================================
    // 11. MERKLE SELF-HEALING & ROLLBACK CONTROLLER
    // ==========================================
    const MerkleApp = {
        init(win) {
            const blocksContainer = win.querySelector('#merkle-blocks-container');
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
    // 12. TASK OPTIMIZER CONTROLLER
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
    // 13. SYSTEM TROUBLESHOOTER CONTROLLER
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
    // 14. APP STORE & SETTINGS
    // ==========================================
    const StoreApp = {
        init(win) {
            const catalog = win.querySelector('#store-catalog');
            const apps = [
                { id: 'devstudio', name: 'Aegis DevStudio', author: 'Aegis Core Team', desc: 'Integrated Rust & C development environment with live microkernel debugger.', installed: true },
                { id: 'cyberdeck', name: 'CyberDeck IDS', author: 'Quantum Security Labs', desc: 'Advanced packet graph visualizer and stateful intrusion forensics console.', installed: false },
                { id: 'browser', name: 'Quantum Browser', author: 'Mozilla / Aegis Port', desc: 'Sandboxed Zero-Trust browser with per-tab micro-virtualization memory isolation.', installed: false },
                { id: 'cryptsafe', name: 'CryptSafe Vault', author: 'Shield Foundation', desc: 'Hardware-backed AES-256 password & cryptographic identity manager.', installed: true }
            ];

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
                    const id = btn.dataset.id;
                    const target = apps.find(a => a.id === id);
                    if (target) {
                        target.installed = !target.installed;
                        btn.textContent = target.installed ? 'Installed' : 'Install';
                        btn.classList.toggle('installed', target.installed);
                        Notifications.show(target.installed ? 'Package Installed' : 'Package Removed',
                            `${target.name} has been ${target.installed ? 'isolated in Ring-3 sandbox' : 'uninstalled'}.`,
                            target.installed ? 'success' : 'info');
                    }
                });
            });
        }
    };

    const SettingsApp = {
        init(win) {
            const tabs = win.querySelectorAll('.settings-nav-btn');
            const panels = win.querySelectorAll('.settings-panel');
            tabs.forEach(btn => {
                btn.addEventListener('click', () => {
                    tabs.forEach(b => b.classList.remove('active'));
                    panels.forEach(p => p.classList.add('hidden'));
                    btn.classList.add('active');
                    const target = win.querySelector(`#${btn.dataset.tab}`);
                    if (target) target.classList.remove('hidden');
                });
            });

            const layoutSel = win.querySelector('#settings-layout-select');
            if (layoutSel) {
                layoutSel.value = document.body.className.match(/layout-(\w+)/)?.[1] || 'hybrid';
                layoutSel.addEventListener('change', () => setLayout(layoutSel.value));
            }

            const themeSel = win.querySelector('#settings-theme-select');
            if (themeSel) {
                themeSel.value = document.body.className.match(/theme-([\w-]+)/)?.[1] || 'quantum-cyan';
                themeSel.addEventListener('change', () => setTheme(themeSel.value));
            }
        }
    };

    const VoiceApp = {
        init(win) {
            const status = win.querySelector('#voice-status');
            const response = win.querySelector('#voice-response');

            const handleVoice = (cmd) => {
                // Immediately stop any currently playing speech before starting new
                if ('speechSynthesis' in window) {
                    try { window.speechSynthesis.cancel(); } catch (e) { }
                }

                status.textContent = `Aegis Voice: Processing "${cmd}"...`;
                AudioEngine.playChime('info');

                setTimeout(() => {
                    const low = cmd.toLowerCase();
                    let reply = `Executing voice instruction: "${cmd}"`;

                    if (low.includes('campaign') || low.includes('operation') || low.includes('mission')) {
                        reply = "Opening Aegis Tactical Operations Center. Field incident status ready.";
                        if (window.AegisCampaign) window.AegisCampaign.open();
                    } else if (low.includes('game') || low.includes('gaming')) {
                        reply = "Aegis Task Optimizer configured for Game Mode. Clocks boosted.";
                        WM.open('optimizer', 'Task Optimizer');
                        document.querySelector('.btn-opt-select[data-mode="GAME"]')?.click();
                    } else if (low.includes('storage') || low.includes('disk')) {
                        reply = "Optimizing tiered storage and running TRIM wear-leveling pass.";
                        WM.open('files', 'Files & Storage Manager');
                        document.querySelector('#btn-run-trim-optimize')?.click();
                    } else if (low.includes('security') || low.includes('scan')) {
                        reply = "Starting Aegis Guard AI heuristic sweep and diagnostic scan.";
                        WM.open('cybersec', 'CyberSec Arsenal');
                    } else if (low.includes('amber')) {
                        reply = "Switching adaptive theme to Cyberpunk Amber.";
                        setTheme('cyberpunk-amber');
                    } else if (low.includes('browser') || low.includes('web') || low.includes('internet')) {
                        reply = "Provisioning air-gapped Quantum Ghost Browser in volatile RAM.";
                        WM.open('browser', 'Quantum Ghost Browser (Air-Gapped Virtual Sandbox)', 920, 620);
                    }

                    response.textContent = `"${reply}"`;
                    status.textContent = "Aegis Voice: Ready for next command.";

                    // Check if voice window is still open and unminimized before initiating speech
                    if (WM.windows.has('voice') && !WM.windows.get('voice').classList.contains('minimized') && 'speechSynthesis' in window) {
                        try {
                            window.speechSynthesis.cancel();
                            const utter = new SpeechSynthesisUtterance(reply);
                            utter.rate = 1.05;
                            window.speechSynthesis.speak(utter);
                        } catch (e) { }
                    }
                }, 500);
            };

            const orb = win.querySelector('#voice-orb');
            orb?.addEventListener('click', () => {
                const cmd = prompt("Speak or enter voice command for Aegis Assistant:", "run security scan");
                if (cmd) handleVoice(cmd);
            });

            win.querySelectorAll('.voice-btn').forEach(btn => {
                btn.addEventListener('click', () => handleVoice(btn.dataset.cmd));
            });
        }
    };

    // ==========================================
    // 15. GLOBAL COMMAND PALETTE (CTRL+SPACE)
    // ==========================================
    const Palette = {
        overlay: null,
        input: null,
        results: null,
        selectedIndex: 0,
        items: [
            { title: "Tactical Operations // Campaign", sub: "Live incident response operations, cyberwarfare investigation", action: () => { if (window.AegisCampaign) window.AegisCampaign.open(); }, icon: "shield" },
            { title: "Quantum Ghost Browser", sub: "Air-gapped disposable web sandbox; zero host trace", app: "browser", icon: "browser" },
            { title: "Aegis Mind AI Controller", sub: "Autonomous system copilot: execute any high-level task", app: "ai", icon: "ai" },
            { title: "Files & Tiered Storage Manager", sub: "AegisFS file browser, in-window editor, deduplication & TRIM", app: "files", icon: "files" },
            { title: "CyberSec Arsenal", sub: "Deep packet inspection, vulnerability audit & malware sandbox", app: "cybersec", icon: "cybersec" },
            { title: "Terminal (aegis_shell)", sub: "Interactive microkernel shell and command console", app: "terminal", icon: "terminal" },
            { title: "Neural Activity Monitor", sub: "Live entropy waveforms, telemetry and thread graphs", app: "neural", icon: "neural" },
            { title: "Aegis Shield & Zero-Trust", sub: "Stateful packet firewall and threat quarantine vault", app: "shield", icon: "shield" },
            { title: "Self-Healing & Rollback Engine", sub: "Merkle tree integrity inspector and atomic rollback", app: "merkle", icon: "merkle" },
            { title: "Task Optimizer", sub: "Dynamic scheduler tuning (Game, Work, Stream, Eco)", app: "optimizer", icon: "optimizer" },
            { title: "7-Point System Diagnostics", sub: "Automated scan across memory, drivers, and network", app: "troubleshoot", icon: "troubleshoot" },
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

            window.addEventListener('keydown', (e) => {
                if ((e.ctrlKey || e.metaKey) && e.code === 'Space') {
                    e.preventDefault();
                    this.toggle();
                } else if (e.key === 'Escape' && !this.overlay.classList.contains('hidden')) {
                    this.close();
                }
            });

            this.input.addEventListener('input', () => this.render());
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

            this.overlay.addEventListener('click', (e) => {
                if (e.target === this.overlay) this.close();
            });

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
            if (item.action) item.action();
            else if (item.app) WM.open(item.app, item.title);
        }
    };

    // ==========================================
    // 16. THEME & LAYOUT DISPATCHERS
    // ==========================================
    function setTheme(theme) {
        document.body.classList.remove('theme-quantum-cyan', 'theme-cyberpunk-amber', 'theme-deep-space-dark', 'theme-crystal-light');
        document.body.classList.add(`theme-${theme}`);

        const topSel = document.getElementById('theme-select');
        if (topSel) topSel.value = theme;
        const setSel = document.getElementById('settings-theme-select');
        if (setSel) setSel.value = theme;

        Notifications.show('Theme Applied', `Switched to ${theme.replace('-', ' ')}.`, 'info');
    }

    function setLayout(layout) {
        document.body.classList.remove('layout-hybrid', 'layout-windows', 'layout-linux');
        document.body.classList.add(`layout-${layout}`);

        const topSel = document.getElementById('layout-select');
        if (topSel) topSel.value = layout;
        const setSel = document.getElementById('settings-layout-select');
        if (setSel) setSel.value = layout;

        Notifications.show('Layout Paradigm Changed', `Active Interface Mode: ${layout.toUpperCase()}`, 'info');
    }

    function startTelemetryLoop() {
        const topCpu = document.getElementById('top-cpu');
        const topCpuFill = document.getElementById('top-cpu-fill');
        const topRam = document.getElementById('top-ram');
        const topRamFill = document.getElementById('top-ram-fill');
        const hudClock = document.getElementById('hud-clock');
        const trayClock = document.getElementById('tray-clock');

        const tick = async () => {
            const now = new Date();
            const timeStr = now.toLocaleTimeString();
            if (hudClock) hudClock.textContent = timeStr;
            if (trayClock) trayClock.textContent = timeStr.substring(0, 5);

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
                const randCpu = Math.floor(18 + Math.random() * 8);
                if (topCpu) topCpu.textContent = `${randCpu}%`;
                if (topCpuFill) topCpuFill.style.width = `${randCpu}%`;
            }
        };

        tick();
        setInterval(tick, 2000);
    }

    function escapeHtml(str) {
        return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    }

    // ==========================================
    // 17. SYSTEM INITIALIZATION
    // ==========================================
    function initSystem() {
        Wallpaper.init();
        Notifications.init();
        WM.init();
        Palette.init();
        startTelemetryLoop();

        document.querySelectorAll('[data-app]').forEach(btn => {
            btn.addEventListener('click', () => {
                const app = btn.dataset.app;
                const labels = {
                    browser: 'Quantum Ghost Browser (Air-Gapped Virtual Sandbox)',
                    ai: 'Aegis Mind AI Controller',
                    files: 'Files & Storage Manager',
                    cybersec: 'CyberSecurity Arsenal',
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
                const w = app === 'browser' ? 920 : app === 'ai' ? 840 : app === 'files' ? 820 : 760;
                const h = app === 'browser' ? 620 : app === 'ai' ? 560 : app === 'files' ? 530 : 500;
                WM.open(app, labels[app] || app.toUpperCase(), w, h);
            });
        });

        document.getElementById('layout-select')?.addEventListener('change', (e) => setLayout(e.target.value));
        document.getElementById('theme-select')?.addEventListener('change', (e) => setTheme(e.target.value));

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

        startMenu?.querySelectorAll('.start-item[data-app]').forEach(item => {
            item.addEventListener('click', () => {
                startMenu.classList.add('hidden');
            });
        });

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

        // Open Aegis Mind AI & Storage Manager on first load
        setTimeout(() => {
            WM.open('ai', 'Aegis Mind AI Controller', 840, 560);
            WM.open('files', 'Files & Storage Manager', 800, 510);
            Notifications.show('AegisOS 2.0 Ready', 'Aegis Mind AI online. Tiered storage & Cybersecurity Arsenal active.', 'success', 5000);
        }, 400);
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initSystem);
    } else {
        initSystem();
    }
})();
