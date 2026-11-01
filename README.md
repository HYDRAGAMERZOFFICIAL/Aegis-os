<p align="center">
  <img width="800" height="auto" alt="AegisOS Banner" src="https://github.com/user-attachments/assets/12fc5883-8c34-4b4d-927a-09ffb8e19a5d" />
</p>

<h1 align="center">AegisOS: Hybrid Secure Intelligence Operating System</h1>

<p align="center">
  <b>A next-generation operating system combining Linux-level stability, Windows-level UI/UX usability, and AI-level self-protection.</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Kernel-v2.0--Alpha-00f3ff?style=for-the-badge" alt="Kernel Version">
  <img src="https://img.shields.io/badge/Architecture-Dual--Kernel%20Microkernel-00ffaa?style=for-the-badge" alt="Architecture">
  <img src="https://img.shields.io/badge/Security-Zero--Trust%20Default--Deny-ffaa00?style=for-the-badge" alt="Security">
  <img src="https://img.shields.io/badge/Filesystem-Merkle%20Self--Healing-ff3366?style=for-the-badge" alt="Filesystem">
  <img src="https://img.shields.io/badge/Tests-100%25%20Passing-brightgreen?style=for-the-badge" alt="Tests">
</p>

---

## 🌟 Core Pillars

1. **Linux-Level Stability**: Microkernel isolation, SELinux-grade capabilities, Ring 3 sandboxed user-space drivers, and robust memory management with 64-bit Address Space Layout Randomization (ASLR).
2. **Windows-Level UI/UX Usability**: Adaptive multi-window desktop compositor, real-time layout switching (**Aegis Hybrid**, **Windows Mode**, and **Linux Mode**), dynamic glassmorphic neon theming, spotlight command palette, and integrated app store.
3. **AI-Level Self-Protection**: Stateful Aegis Shield firewall, Aegis Guard heuristic intrusion detection (98.7% ML accuracy), Zero-Trust default-deny hardware gate, neural activity telemetry stream, and cryptographic Merkle tree filesystem with atomic **sub-10-second rollback**.

---

## 🚀 Quick Start & Simulators

AegisOS includes both a **Rich Web/GUI Interactive Desktop Simulator** and a compiled **Native C CLI Simulator**.

### 1. Launch the Interactive Web Desktop Simulator (Recommended)

Start the built-in Node.js simulation server and open the browser:

```bash
# Using npm
npm start
# or
node simulator/server.js
```
Then navigate to: **`http://localhost:3000`**

Or simply run the batch launcher on Windows:
```cmd
.\run_sim.bat
```

#### What You Can Experience in the Web GUI:
- **Desktop Window Manager**: Drag, resize, minimize, maximize, and stack glassmorphic windows.
- **Layout Paradigm Switcher**: Switch instantly between **Aegis Hybrid** (top floating HUD + bottom dock), **Windows Mode** (bottom taskbar + start menu + system tray), and **Linux Mode** (universal top bar + side dash).
- **Adaptive Theming Engine**: Switch seamlessly between **Quantum Cyan**, **Cyberpunk Amber**, **Deep Space Dark**, and **Crystal Light**.
- **Global Command Palette**: Press <kbd>Ctrl</kbd> + <kbd>Space</kbd> or click the search bar for live fuzzy command search.
- **Interactive Aegis Shell**: Real-time terminal with command history (<kbd>↑</kbd>/<kbd>↓</kbd>) and tab completion (<kbd>Tab</kbd>).
- **Live Neural Telemetry Monitor**: Animated HTML5 canvas displaying real-time thread entropy waveforms, system frequency, and kernel ring buffer events.
- **Aegis Shield & Zero-Trust Console**: Interactive firewall rules, stateful packet inspector, and quarantined threat vault.
- **Merkle Tree Self-Healing & Rollback**: Click **"Simulate Ransomware Attack"** to corrupt a filesystem block, watch Aegis Guard trigger the cryptographic alarm, and trigger a **10-Second Atomic Rollback** to restore baseline state with 0 data loss.
- **Task Optimizer**: Switch between **Game Mode** (+25% CPU, +30% GPU boost, background tasks paused), **Work Mode**, **Streaming Mode**, and **Eco Mode** (9W TDP limit).
- **7-Point Diagnostic Scanner**: Automated self-check across kernel memory, drivers, Merkle consistency, and TLS gates.
- **Aegis Voice Assistant**: Voice action triggers with speech synthesis and frequency orb animation.

---

### 2. Launch the Native C CLI Simulator

A standalone compiled native executable (`aegis_sim.exe`) is included, linking all 85+ microkernel modules:

```bash
# Run interactive CLI console
.\aegis_sim.exe

# Or run the automated 8-point system demonstration
.\aegis_sim.exe --demo
```

---

## 🏗️ Architectural Overview

```
+-----------------------------------------------------------------------------------+
|                                 USERLAND DESKTOP                                  |
|  [Aegis Shell]   [Adaptive UI Compositor]   [Settings Center]   [Aegis App Store] |
|       |                     |                       |                   |         |
+-------+---------------------+-----------------------+-------------------+---------+
|                               SYSTEM SERVICES LAYER                               |
|  [Neural Monitor]   [System Troubleshoot]   [Task Optimizer]   [Checkpoint Engine]|
|  [Voice Assistant]  [Command Palette Engine]                                      |
+-----------------------------------------------------------------------------------+
|                             SECURITY SUBSYSTEM                                    |
|  [Aegis Shield (IDS/IPS)]   [Aegis Guard (AI IDS)]   [Zero-Trust Access Controller|
|  [Hardware Token Gate]      [Driver Ring 3 Isolation]                             |
+-----------------------------------------------------------------------------------+
|                             AEGIS DUAL-KERNEL CORE                                |
|  [Memory Manager (ASLR)]    [Completely Fair Scheduler (CFS)]    [Interrupt Gate] |
|  [Microkernel IPC Bus]      [Device Tree Parser]                 [Kernel Profiler]|
+-----------------------------------------------------------------------------------+
|                        CRYPTOGRAPHIC MERKLE FILESYSTEM                            |
|  [Merkle Hash Journal]      [Per-File AES-256]        [Sub-10s Atomic Rollbacks]  |
+-----------------------------------------------------------------------------------+
```

---

## 🧪 Comprehensive Test Suite (100% Pass)

AegisOS features extensive automated test coverage across all subsystems:

| Test Suite | File | Tests | Pass Rate |
| :--- | :--- | :--- | :--- |
| **Phases 1–7 Core System Integration** | `tests/test_main.c` | 84 | **100.0%** (84/84) |
| **Phase 7 Security Hardening** | `tests/test_phase7_security.c` | 58 | **100.0%** (58/58) |
| **Phase 8 Userland & UI Tests** | `tests/test_phase8_ui.c` | 76 | **100.0%** (76/76) |
| **Phase 9 Advanced Features & Memory Tagging** | `tests/test_phase9_advanced.c` | 57 | **100.0%** (57/57) |
| **Component Implementations Verification** | `tests/test_implementation_needed.c` | 67 | **100.0%** (67/67) |
| **Total Verified Test Cases** | | **342** | **100.0%** |

To run the full test suite with MinGW GCC:
```bash
gcc -Iinclude -Itests tests/test_main.c tests/test_phase7_security.c tests/test_phase8_ui.c tests/test_phase9_advanced.c tests/test_implementation_needed.c kernel/*.c security/*.c services/*.c userland/*.c filesystem/file_ops.c -o tests/runner.exe
.\tests\runner.exe
```

---

## ⌨️ Shell Commands Reference

Inside either the native simulator or the Web Terminal, the following commands are supported:

| Command | Description | Example |
| :--- | :--- | :--- |
| `help` | Display interactive command list and usage | `help` |
| `sysinfo` | View kernel version, memory, scheduler & security stats | `sysinfo` |
| `ps` | List active microkernel processes and threads | `ps` |
| `shield` | Inspect stateful firewall rules & blocked IDS packets | `shield` |
| `guard` | Trigger AI heuristic malware & ransomware scan | `guard` |
| `ztac` | View Zero-Trust access control tokens and active gates | `ztac` |
| `neural` | Dump real-time neural telemetry stream metrics | `neural` |
| `troubleshoot` | Run the automated 7-point diagnostic suite | `troubleshoot` |
| `opt <mode>` | Switch optimizer profile: `game`, `work`, `stream`, `eco` | `opt game` |
| `checkpoint` | List restore points or trigger atomic rollback | `checkpoint heal` |
| `fs <cmd>` | Interact with Merkle filesystem: `ls`, `corrupt`, `heal` | `fs corrupt` |
| `theme <name>` | Switch UI theme: `cyan`, `amber`, `dark`, `light` | `theme amber` |
| `layout <mode>`| Switch desktop layout: `hybrid`, `win`, `linux` | `layout win` |
| `clear` | Clear terminal display buffer | `clear` |

---

## 📜 License & Acknowledgments

AegisOS is open-source under the Apache 2.0 License. Built with pride for extreme security, modern aesthetics, and resilient intelligence.
