'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');

const web = path.join(__dirname, '..', '..', 'simulator', 'web');
const campaign = fs.readFileSync(path.join(web, 'native-campaign.js'), 'utf8');
const desktop = fs.readFileSync(path.join(web, 'index.html'), 'utf8');
const server = fs.readFileSync(path.join(web, '..', 'server.js'), 'utf8');

test('native campaign is mounted inside the original desktop, not as a launcher app', () => {
  assert.match(desktop, /native-campaign\.js/);
  assert.doesNotMatch(desktop, /Aegis Campaign/);
  assert.match(campaign, /aegis:wm-open/);
  assert.match(campaign, /aegis:terminal-command/);
});

test('campaign contains three long investigations with evidence and decision gates', () => {
  for (const title of ['THE SILENT INTRUSION', 'GHOST IN THE KERNEL', 'BLACKOUT PROTOCOL']) {
    assert.ok(campaign.includes(title), 'missing operation ' + title);
  }
  assert.match(campaign, /mission-report-btn/);
  assert.match(campaign, /state\.decision\[m\.id\]===undefined/);
  assert.match(campaign, /progress\.length===s\[3\]\.length/);
  assert.match(campaign, /evidence\(\)\.length<m\.steps\.filter/);
  assert.match(campaign, /aegis:authenticated/);
  assert.match(campaign, /\/api\/campaign\/state/);
  assert.match(campaign, /mission-quiz/);
  assert.match(campaign, /answersCorrect/);
  assert.match(campaign, /decisionUnlocked/);
  assert.match(campaign, /state\.stats\.xp/);
  assert.match(server, /pathname === '\/api\/campaign\/state'/);
  assert.match(server, /case 'capability-audit'/);
  assert.match(server, /case 'route-select'/);
  assert.match(server, /svc-sync-cache\.bin/);
  assert.match(server, /pathname === '\/game' \|\| pathname === '\/game\.html'/);
  assert.match(desktop, /desktop-auth\.js/);
  assert.match(desktop, /aegis-auth-gate/);
  assert.match(desktop, /desktop-auth\.css/);
});
