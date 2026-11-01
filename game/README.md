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

## Player identity and save storage

### Vercel cloud deployment
The Vercel build uses Supabase Auth for email/password identity and Postgres for campaign persistence. Player profiles are keyed to `auth.users.id`; every campaign save is keyed by the same UUID. Row-level security restricts profile and save access to `auth.uid()`. Successful mission actions and research purchases are validated by the server-side GameManager and persisted automatically.

Apply `supabase/migrations/20261101000000_aegis_cloud_players.sql` and follow [VERCEL_DEPLOYMENT.md](../VERCEL_DEPLOYMENT.md) before deploying.

### Local desktop mode
The Electron/local Node simulator stores player profiles in `~/.aegis-os/accounts.json` and per-player campaign saves in `~/.aegis-os/saves/`. Passwords are stored as salted scrypt hashes. This local profile store is separate from the Vercel cloud identity service and is not Steam authentication.

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
