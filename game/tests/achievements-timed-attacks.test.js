'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');

const web = path.join(__dirname, '..', '..', 'simulator', 'web');
const campaign = fs.readFileSync(path.join(web, 'native-campaign.js'), 'utf8');
const server = fs.readFileSync(path.join(web, '..', 'server.js'), 'utf8');
const vercel = JSON.parse(fs.readFileSync(path.join(__dirname, '..', '..', 'vercel.json'), 'utf8'));

test('campaign defines 12 cybersecurity achievements with XP and Credits', () => {
  const expectedAchievements = [
    'first_breach',
    'timed_hero',
    'rapid_triage',
    'zero_trust_elite',
    'merkle_savior',
    'packet_hunter',
    'forensic_genius',
    'terminal_warrior',
    'ghost_protocol',
    'crypt_inode',
    'director_commendation',
    'citadel_sealed'
  ];
  for (const ach of expectedAchievements) {
    assert.ok(campaign.includes("id:'" + ach + "'"), 'missing achievement ' + ach);
  }
  assert.match(campaign, /unlockAchievement/);
  assert.match(campaign, /achievementToast/);
});

test('campaign defines time-based emergency attack scenarios', () => {
  assert.match(campaign, /TIMED_SCENARIOS/);
  assert.match(campaign, /triggerTimedAttack/);
  assert.match(campaign, /aegis-emergency-banner/);
  assert.match(campaign, /emergencyTimeOut/);
  assert.match(campaign, /defusalActions/);
});

test('server supports emergency drills and achievement shell commands', () => {
  assert.match(server, /case 'achievements':/);
  assert.match(server, /case 'drill':/);
  assert.match(server, /case 'simulate-attack':/);
  assert.match(server, /case 'emergency-status':/);
});

test('vercel deployment configuration routes static and API endpoints cleanly', () => {
  assert.equal(vercel.version, 2);
  assert.ok(Array.isArray(vercel.rewrites));
  assert.ok(vercel.rewrites.some(r => r.source === '/api/(.*)' && r.destination === '/api/index.js'));
  const publicDir = path.join(__dirname, '..', '..', 'public');
  assert.ok(fs.existsSync(path.join(publicDir, 'index.html')), 'public/index.html must exist to prevent Vercel 404');
  assert.ok(fs.existsSync(path.join(__dirname, '..', '..', 'api', 'index.js')), 'api/index.js must exist for serverless requests');
});

