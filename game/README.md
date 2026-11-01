# AegisOS Native Operations Campaign

The campaign is part of the original AegisOS desktop. There is no separate game launcher, desktop icon, or campaign window. The player investigates incidents through the OS's existing applications and a compact Operations panel.

## Player experience

- The desktop boots behind an account gate. Each operator signs in or creates a profile.
- Incidents arrive through the native AegisOS notification system.
- Neural Monitor, Terminal, Files, Aegis Mind, CyberSecurity Arsenal, Aegis Shield, Merkle Filesystem and Task Optimizer are the investigation tools.
- Opening an application only records one part of a multi-source objective. Objectives that require multiple sources track each source independently.
- The Operations panel records clues, partial evidence, field notes, hints and objective progress.
- Strategic decisions remain locked until the investigation milestones preceding the decision are complete.
- Each operation ends with a deduction report. The player must enter answers derived from the collected evidence before the report can be sealed.
- Choices affect the outcome, command score, grade, XP, credits and research rewards.

## Campaign operations

### 01 — The Silent Intrusion
Reconstruct an unauthorised service restart, identify the staged persistence artefact, correlate the outbound beacon, choose a containment strategy, apply a narrow firewall policy, verify containment, inspect Merkle integrity and restore the trusted checkpoint. Final deductions identify the remote endpoint and event sequence.

### 02 — Ghost in the Kernel
Investigate an unexpected capability grant to a trusted component. Correlate Neural Monitor telemetry, Zero-Trust token grants, process ownership, binary provenance and Merkle state. Choose a least-privilege response, verify the training patch, quarantine the simulated component and prove kernel trust. Final deductions identify the anomalous token and component.

### 03 — Blackout Protocol
Investigate a simulated traffic flood without disrupting legitimate priority traffic. Compare system load, packet distribution, service dependencies and edge health. Choose a mitigation profile, apply the Aegis Shield policy, select a resilient route, verify service recovery and submit a post-mortem. Final deductions identify the degraded and healthy edge routes.

Each operation contains 11 milestones. Progress is evidence-driven, not a simple button sequence. The fictional training commands are documented in Terminal's `help` output, including `capability-audit`, `verify-containment`, `verify-root`, `quarantine-component`, `service-health`, `route-health` and `route-select`.

## Player accounts and saves

The desktop authenticates against the local AegisOS account service. Passwords are stored as scrypt hashes; the session cookie is HttpOnly and SameSite. Native campaign state is stored server-side in a separate save file for each authenticated operator under:

- `~/.aegis-os/accounts.json` — local operator accounts
- `~/.aegis-os/campaigns/<operator-id>.json` — native campaign progress, evidence, decisions, deductions and rewards

The browser's local storage is only a cache; the authenticated server-side campaign slot is authoritative. For hosted deployment, the Node.js backend must use a persistent volume or a database for these files. A stateless/ephemeral server filesystem is not suitable for durable player accounts and saves.

This is AegisOS player authentication, not Steamworks authentication. Steam account linking, cloud achievements and Steam Cloud remain separate release tasks.

## Run and test

From the repository root:

```bash
npm install
npm start
```

Open `http://127.0.0.1:3000/`. The first launch prompts you to create an operator profile. The legacy `/game` route resolves to the native desktop.

Run the campaign and game engine tests:

```bash
npm run test:game
```

## Safety boundary

All threats, packet traces, files, endpoints, process states and remediation commands are fictional simulation data. The campaign commands do not execute host shell commands, attack external networks, or modify real system files.
