# Aegis: The Last Firewall

A browser-based campaign game integrated into the AegisOS desktop simulator.

## Run it

From the repository root:

```bash
npm start
```

Open `http://localhost:3000`. The **Last Firewall** launcher appears among the AegisOS desktop icons.

## Current playable slice

The game engine has a Node.js test suite. Run it with `npm run test:game`.

- Five fictional incident-response missions across three campaign chapters.
- Objective sequence: inspect → contain → verify integrity → recover.
- Persistent player profile with XP, credits, research points, level and service record.
- Six research upgrades.
- Evidence-based branching decisions, mission grades and bonus rewards.
- Active mission sessions and selected decisions resume after a browser refresh.
- Mission debriefs, chapter unlocks and local save/reset.
- Responsive game UI that runs as an overlay on the existing AegisOS desktop.
- A dedicated static asset mount at `/game/*`, constrained to the repository's root `game/` directory.

## Architecture

- `engine/engine.js`: deterministic game state, mission lifecycle, rewards, progression and local persistence.
- `engine/missions.js`: campaign content and mission objectives.
- `ui/game.js`: desktop launcher, game shell, campaign/research/records screens and interaction wiring.
- `ui/game.css`: isolated game presentation layer.

The game is deliberately implemented as a separate layer. It does not modify real host processes, inspect the host filesystem, or perform network attacks. Incident names and telemetry are fictional game data.

## Save data

Campaign progress is stored in browser local storage under:

`aegis.lastFirewall.save.v1`

Use **Service Record → Reset Campaign** to clear the local profile.

## Design principles

1. Keep the existing AegisOS desktop as the player's in-world interface.
2. Keep game state separate from operating-system simulator state.
3. Make mission outcomes deterministic and testable.
4. Keep security gameplay defensive and contained inside the fictional simulation.
5. Add new mechanics behind the game engine rather than coupling them to desktop internals.

## Current scope and next milestones

This is the first playable vertical slice, not the finished game. Recommended next work:

1. Add automated unit tests for mission ordering, save/load and reward calculations.
2. Add richer branching mission decisions and evidence-based scoring.
3. Add mission-specific consequence branches and scenario simulation tests.
4. Connect mission events to existing simulator telemetry through a read-only adapter.
5. Add sandbox and survival modes.
6. Add audio, tutorial onboarding, accessibility review and balancing.
7. Add a versioned save migration strategy before changing the save schema.
