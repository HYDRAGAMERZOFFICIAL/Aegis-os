# AegisOS Vercel deployment

## Cloud player accounts and campaign saves

The Vercel build uses Supabase Auth for registration/login and Postgres for per-player campaign saves. Every game API request validates the player's Supabase session. Save rows are keyed by the authenticated Supabase user UUID, and row-level security prevents one player from reading or changing another player's profile or save.

### 1. Create a dedicated Supabase project

Do not reuse a project that contains unrelated application data. In Supabase, create a new project for AegisOS.

### 2. Apply the database migration

Open **SQL Editor** in the new project and run the complete contents of:

`supabase/migrations/20261101000000_aegis_cloud_players.sql`

The migration creates:
- `player_profiles` with unique operator IDs and auth-user linkage.
- `player_game_saves` with one versioned JSON campaign save per user.
- A profile-creation trigger and ownership-enforcing RLS policies.

### 3. Configure Supabase Auth

In **Authentication → URL Configuration**, add the deployed Vercel URL to the allowed site URLs and redirect URLs. Email confirmation can remain enabled; the game will ask players to verify their email before their first sign-in if Supabase requires it.

### 4. Configure Vercel environment variables

Add these variables to **Production**, **Preview**, and **Development** as needed:

- `SUPABASE_URL`: the new project's Project URL.
- `SUPABASE_ANON_KEY`: the project's publishable/anon key.

The anon key is designed for client-facing use; this implementation keeps it on the API side. **Never add the service-role key.** Database access is performed with the player's verified access token and RLS.

### 5. Deploy

Import the GitHub repository into Vercel and deploy the branch. The `public/` directory contains the Vercel static game build. Both `/` and `/game` open the AegisOS command campaign so the incompatible legacy desktop simulator controls are not exposed in the Vercel game experience. The `api/[...route].js` function provides the cloud API under `/api`.

### 6. Verify the release

1. Register Player A with a unique operator ID and email.
2. Complete an objective and confirm the save timestamp updates.
3. Sign out and register Player B.
4. Confirm Player B starts with a separate campaign.
5. Sign back in as Player A and confirm their progress is restored.
6. Repeat on another browser/device to verify cloud persistence.

## Local desktop mode

The existing Electron/local Node server remains a separate local profile mode. Vercel is the cloud multiplayer/account deployment target. Do not expose the local desktop simulator server directly to the public internet.

## Security notes

- Passwords are handled by Supabase Auth; the game never stores password hashes itself in the cloud database.
- Access and refresh tokens are held in HttpOnly, SameSite cookies.
- Every cloud game request resolves the authenticated user before loading a save.
- RLS uses `auth.uid()` for profile and save ownership.
- Campaign actions and rewards are validated by the server-side GameManager before the new save is written.
- All hostile activity remains fictional and simulated; the game does not scan real networks or modify host files.
