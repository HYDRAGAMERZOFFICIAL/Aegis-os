# AegisOS Game Foundation

This directory contains the first modular game layer for the AegisOS cyberwarfare simulation. It is deliberately separate from the existing OS simulator.

## Current foundation
- Event bus for decoupled game-system events.
- Resource economy with validated transactions.
- Research tree with prerequisite and point checks.
- Threat lifecycle: spawn, detect, contain and turn-based pressure.
- Mission framework with objectives, completion and rewards.
- Versioned, atomic local save files.
- Mission 01: The Silent Intrusion.

All threats, attacks, files and recovery operations are fictional game-state objects. The game layer must never execute malware, scan external networks, or alter host files.

## Run the smoke tests
From the repository root:

```bash
node --test game/tests/*.test.js
```

## API integration
The game engine is CommonJS and has no third-party runtime dependencies. The next integration step is to expose its state/actions through the local simulator API and build a dedicated campaign UI.
