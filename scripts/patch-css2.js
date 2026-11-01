const fs = require('fs');
const path = require('path');

const cssAdditions = `
/* ======================================================= */
/* VIRTUAL MICROKERNEL BOOT SCREEN                         */
/* ======================================================= */
.boot-screen {
    position: fixed;
    top: 0;
    left: 0;
    width: 100vw;
    height: 100vh;
    background: #030811;
    z-index: 100000;
    display: flex;
    align-items: center;
    justify-content: center;
    color: #eaf8ff;
    font-family: var(--font-mono, monospace);
    transition: opacity 0.5s ease;
}
.boot-screen.fade-out {
    opacity: 0;
    pointer-events: none;
}
.boot-content {
    width: min(640px, 92vw);
    display: flex;
    flex-direction: column;
    align-items: center;
    text-align: center;
}
.boot-crest svg {
    width: 72px;
    height: 72px;
    filter: drop-shadow(0 0 16px rgba(0, 243, 255, 0.6));
    animation: pulseCrest 1.5s infinite alternate ease-in-out;
}
@keyframes pulseCrest {
    from { transform: scale(0.95); opacity: 0.8; }
    to { transform: scale(1.05); opacity: 1; }
}
.boot-title {
    font-size: 18px;
    letter-spacing: 3px;
    margin: 14px 0 16px;
    color: #00f3ff;
}
.boot-console {
    width: 100%;
    height: 150px;
    background: #02050b;
    border: 1px solid rgba(0, 243, 255, 0.3);
    border-radius: 8px;
    padding: 12px;
    overflow-y: auto;
    font-size: 11px;
    line-height: 1.6;
    text-align: left;
    color: #a4e5ff;
    box-shadow: inset 0 0 15px rgba(0, 0, 0, 0.8);
    margin-bottom: 14px;
}
.boot-line {
    animation: fadeIn 0.15s ease-out;
}
.boot-bar-wrap {
    width: 100%;
    height: 8px;
    background: rgba(255, 255, 255, 0.1);
    border-radius: 4px;
    overflow: hidden;
    margin-bottom: 10px;
}
.boot-bar-fill {
    width: 0%;
    height: 100%;
    background: linear-gradient(90deg, #00f3ff, #00ffaa);
    box-shadow: 0 0 12px rgba(0, 243, 255, 0.8);
    transition: width 0.15s linear;
}
.boot-status {
    font-size: 11px;
    letter-spacing: 1px;
    color: var(--text-muted);
}

/* ======================================================= */
/* CRITICAL ZERO-DAY BREACH OVERLAY & RED SCREEN           */
/* ======================================================= */
body.system-breached-critical {
    box-shadow: inset 0 0 80px rgba(255, 0, 0, 0.7);
    animation: flashRedVignette 1s infinite alternate ease-in-out;
}
@keyframes flashRedVignette {
    0% { box-shadow: inset 0 0 40px rgba(255, 0, 0, 0.4); }
    100% { box-shadow: inset 0 0 90px rgba(255, 0, 0, 0.85); }
}
.breach-overlay {
    position: fixed;
    top: 0;
    left: 0;
    width: 100vw;
    height: 100vh;
    background: rgba(8, 2, 4, 0.94);
    backdrop-filter: blur(20px);
    z-index: 999999;
    display: flex;
    align-items: center;
    justify-content: center;
    color: #fff;
    font-family: inherit;
    animation: breachSlideIn 0.3s cubic-bezier(0.16, 1, 0.3, 1);
}
@keyframes breachSlideIn {
    from { opacity: 0; transform: scale(1.05); }
    to { opacity: 1; transform: scale(1); }
}
.breach-panel {
    width: min(580px, 94vw);
    background: linear-gradient(135deg, rgba(35, 5, 10, 0.98), rgba(18, 2, 4, 0.98));
    border: 2px solid #ff2244;
    border-radius: 14px;
    padding: 24px;
    box-shadow: 0 0 60px rgba(255, 34, 68, 0.4), inset 0 0 30px rgba(255, 34, 68, 0.2);
    text-align: center;
}
.breach-header {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 10px;
    margin-bottom: 8px;
}
.breach-skull {
    font-size: 28px;
    animation: shakeSkull 0.5s infinite alternate;
}
@keyframes shakeSkull {
    0% { transform: rotate(-5deg); }
    100% { transform: rotate(5deg); }
}
.breach-title {
    font-size: 15px;
    font-weight: 800;
    letter-spacing: 1.5px;
    color: #ff3355;
    font-family: var(--font-mono, monospace);
}
.breach-subtitle {
    font-size: 13px;
    font-weight: 700;
    color: #ff99aa;
    margin-bottom: 12px;
}
.breach-warning {
    font-size: 11px;
    line-height: 1.5;
    color: #ffd0d8;
    margin-bottom: 16px;
}
.breach-timer-wrap {
    background: #0a0103;
    border: 1px solid #ff2244;
    border-radius: 8px;
    padding: 12px;
    margin-bottom: 16px;
    box-shadow: inset 0 0 15px rgba(255, 0, 0, 0.5);
}
.breach-clock {
    font-size: 42px;
    font-weight: 900;
    font-family: var(--font-mono, monospace);
    color: #ff2244;
    letter-spacing: 4px;
    text-shadow: 0 0 20px rgba(255, 34, 68, 0.8);
}
.breach-puzzle-box {
    background: rgba(0, 0, 0, 0.4);
    border: 1px solid rgba(255, 255, 255, 0.1);
    border-radius: 8px;
    padding: 14px;
    margin-bottom: 12px;
    text-align: left;
}
.breach-hint {
    font-size: 11px;
    color: #ffd080;
    margin-bottom: 10px;
}
.breach-input-row {
    display: flex;
    gap: 8px;
}
.breach-input-row input {
    flex: 1;
    background: #02050b;
    border: 1px solid #ff4466;
    border-radius: 6px;
    padding: 8px 12px;
    color: #fff;
    font-family: var(--font-mono, monospace);
    font-size: 12px;
}
.btn-breach-fix {
    padding: 8px 16px;
    background: linear-gradient(135deg, #ff2244, #ff5577);
    border: none;
    border-radius: 6px;
    color: #fff;
    font-weight: 800;
    font-size: 12px;
    cursor: pointer;
    box-shadow: 0 0 15px rgba(255, 34, 68, 0.6);
    transition: all 0.2s ease;
}
.btn-breach-fix:hover {
    transform: scale(1.04);
    filter: brightness(1.15);
}
.breach-status {
    font-size: 10px;
    font-family: var(--font-mono, monospace);
    color: #ff8899;
}

/* ======================================================= */
/* TUTORIAL MODAL STYLING                                  */
/* ======================================================= */
.tutorial-window {
    width: min(640px, 94vw);
    max-height: 85vh;
    background: rgba(7, 16, 27, 0.98);
    border: 1px solid rgba(0, 243, 255, 0.45);
    border-radius: 12px;
    padding: 20px;
    display: flex;
    flex-direction: column;
    box-shadow: 0 16px 50px rgba(0, 0, 0, 0.8), 0 0 30px rgba(0, 243, 255, 0.2);
}
.tutorial-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding-bottom: 12px;
    border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}
.tutorial-header h3 {
    margin: 0;
    font-size: 13px;
    letter-spacing: 1.5px;
    color: #00f3ff;
}
.tutorial-tabs {
    display: flex;
    gap: 6px;
    overflow-x: auto;
    padding: 10px 0;
    border-bottom: 1px solid rgba(255, 255, 255, 0.08);
}
.tutorial-tab {
    padding: 6px 10px;
    background: rgba(255, 255, 255, 0.04);
    border: 1px solid rgba(255, 255, 255, 0.1);
    border-radius: 6px;
    color: var(--text-muted);
    font-size: 11px;
    cursor: pointer;
    white-space: nowrap;
    transition: all 0.2s ease;
}
.tutorial-tab.active {
    background: rgba(0, 243, 255, 0.15);
    border-color: #00f3ff;
    color: #fff;
    font-weight: 700;
}
.tutorial-body {
    flex: 1;
    overflow-y: auto;
    padding: 14px 4px;
    font-size: 12px;
    line-height: 1.6;
}
.tutorial-body h4 {
    margin: 0 0 8px;
    color: #00f3ff;
}
.tutorial-body ul {
    margin: 8px 0 12px 18px;
    padding: 0;
}
.tutorial-body li {
    margin-bottom: 6px;
}
.tutorial-alert-sample {
    background: rgba(255, 34, 68, 0.15);
    border-left: 3px solid #ff2244;
    padding: 10px 14px;
    border-radius: 4px;
    margin: 10px 0;
    color: #ffd0d8;
}
.tutorial-footer {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding-top: 14px;
    border-top: 1px solid rgba(255, 255, 255, 0.1);
    font-size: 11px;
    color: var(--text-muted);
}

/* ======================================================= */
/* OPERATOR CASE NOTEPAD STYLING                           */
/* ======================================================= */
.app-notepad {
    display: flex;
    flex-direction: column;
    height: 100%;
}
.notepad-toolbar {
    display: flex;
    gap: 8px;
    align-items: center;
    padding: 8px 12px;
    background: rgba(5, 12, 20, 0.6);
    border-bottom: 1px solid var(--border-glass);
    flex-wrap: wrap;
}
.notepad-path {
    font-size: 10px;
    font-family: var(--font-mono, monospace);
    color: var(--text-muted);
    margin-left: auto;
}
.notepad-body {
    flex: 1;
    display: flex;
    overflow: hidden;
    position: relative;
}
.notepad-textarea {
    width: 100%;
    height: 100%;
    background: #02070e;
    border: none;
    outline: none;
    color: #d8f4ff;
    font-family: var(--font-mono, monospace);
    font-size: 12px;
    line-height: 1.7;
    padding: 14px;
    resize: none;
}
.notepad-footer {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 6px 12px;
    background: rgba(5, 12, 20, 0.8);
    border-top: 1px solid var(--border-glass);
    font-size: 10px;
    font-family: var(--font-mono, monospace);
    color: var(--text-muted);
}
.step-lock-badge {
    font-size: 9px;
    color: var(--status-warn);
    font-family: var(--font-mono, monospace);
    display: inline-block;
    margin-left: 6px;
}
`;

['simulator/web/style.css', 'public/style.css'].forEach(rel => {
    const f = path.join(__dirname, '..', rel);
    if (fs.existsSync(f)) {
        let text = fs.readFileSync(f, 'utf8');
        if (!text.includes('CRITICAL ZERO-DAY BREACH OVERLAY')) {
            text += '\n' + cssAdditions;
            fs.writeFileSync(f, text, 'utf8');
            console.log(`Updated ${rel} with boot, breach, and notepad styles.`);
        }
    }
});

