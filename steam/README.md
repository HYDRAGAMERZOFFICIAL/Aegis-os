# Steam release integration checklist

Steam publishing is a later release milestone. This repository branch adds a desktop packaging path, not a completed Steamworks integration.

## Current state
- Electron desktop entry point and Windows NSIS / Linux AppImage build targets are configured.
- The campaign API and local save system work without a Steam account.
- Steam achievements, Cloud Saves, leaderboards, overlay and store configuration are not yet wired.

## Before Steamworks integration
1. Create the product in Steamworks and obtain the real App ID.
2. Select a supported Steamworks SDK binding and pin its version.
3. Map the achievement identifiers in `achievements.json` to Steamworks.
4. Implement initialization, achievement unlock, stats persistence and Cloud Save conflict handling.
5. Test with the Steam client and a private beta branch. Do not commit personal Steam credentials or partner keys.
6. Generate SteamPipe depots and configure Windows/Linux launch options.
7. Complete store assets, content survey, age rating, privacy disclosures and compatibility QA.

## Safety boundary
AegisOS gameplay is a fictional local simulation. Never execute malware, exploit host machines, scan external networks or modify files outside the game's save directory.
