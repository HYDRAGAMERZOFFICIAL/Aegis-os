# AegisOS Game Foundation

This directory contains the modular campaign layer for the AegisOS cyberwarfare strategy simulation. It is integrated with the existing AegisOS desktop simulator; the original desktop interface remains the primary OS environment.

## Current game systems
- Event bus for decoupled game-system events.
- Resource economy with validated transactions.
- Research tree with prerequisite and point checks.
- Threat lifecycle: spawn, detect, contain and turn-based pressure.
- Three sequential campaign operations with briefings, objectives, tactical decisions, completion rewards and unlock progression.
- Versioned, atomic local save files, scoped to each player profile.
- Local player authentication using salted Node.js scrypt password hashes and HttpOnly, SameSite session cookies.
- Boot sequence, player sign-in/profile creation, session lock and password unlock screens.

## Campaign operations
1. **The Silent Intrusion** — investigate a suspicious process, trace the hostile relay, configure Aegis Shield, quarantine the process, restore a clean checkpoint and submit the incident report.
2. **Ghost in the Kernel** — inspect a privilege anomaly, revoke the compromised capability, apply a verified training patch, quarantine the process and audit kernel integrity. Unlocked after Operation 1.
3. **Blackout Protocol** — analyse a simulated traffic flood, apply adaptive ingress limits, reroute critical traffic, restore the API gateway and submit the recovery report. Unlocked after Operation 2.

Selected mission steps require the player to choose the correct evidence, policy, patch, checkpoint or recovery option. Incorrect choices do not advance the objective.

## Local profile data
The local simulator stores player profiles in `~/.aegis-os/accounts.json` and per-player campaign saves in `~/.aegis-os/saves/`. Passwords are never stored in plaintext. Session tokens are held in memory and expire after seven days; restarting the local server requires the player to sign in again.

This is local game authentication, not Steam account authentication or a cloud identity service. Steamworks integration remains a separate release task.

## Run and test
From the repository root:

```bash
npm install
npm start
```

Open `http://127.0.0.1:3000/game`. The first launch prompts you to create a local player profile.

Run all automated tests and syntax checks:

```bash
npm run check:game
npm run test:game
```

## Safety boundary
All threats, attacks, files, endpoints and recovery operations are fictional game-state objects. The game layer does not execute malware, scan external networks or modify host files.
