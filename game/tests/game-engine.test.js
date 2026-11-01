'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const os = require('node:os');
const path = require('node:path');
const fs = require('node:fs');
const GameManager = require('../core/GameManager');
const SaveManager = require('../saves/SaveManager');

test('mission starts and progresses only through valid objectives', () => {
  const game = new GameManager();
  const mission = game.startMission();
  assert.equal(mission.status, 'active');
  assert.equal(mission.objectives.length, 6);
  assert.equal(game.performAction('not-a-real-action').ok, false);
  assert.equal(game.performAction('inspect_process').ok, true);
  assert.equal(game.performAction('trace_origin', { answer: '185.220.101.5' }).ok, true);
  assert.equal(game.performAction('configure_firewall', { answer: 'deny-untrusted' }).ok, true);
  assert.equal(game.performAction('quarantine_process').ok, true);
  assert.equal(game.performAction('restore_files', { answer: 'chkpt-0003' }).ok, true);
  const result = game.performAction('submit_report', { answer: 'critical-contained' });
  assert.equal(result.progress.completed, true);
  assert.equal(game.getState().resources.credits, 750);
  assert.equal(game.getState().resources.research, 50);
});

test('actions cannot mutate game state before mission deployment or out of order', () => {
  const game = new GameManager();
  assert.equal(game.performAction('inspect_process').ok, false);
  game.startMission();
  assert.equal(game.performAction('quarantine_process').ok, false);
  assert.equal(game.getState().threats[0].detected, false);
});

test('threat lifecycle requires detection before containment', () => {
  const game = new GameManager();
  const threat = game.spawnThreat('ransomware-stager');
  assert.equal(game.threats.contain(threat.instanceId), false);
  assert.equal(game.threats.detect(threat.instanceId), true);
  assert.equal(game.threats.contain(threat.instanceId), true);
  assert.equal(game.threats.list()[0].status, 'contained');
});

test('research enforces prerequisites and spends points once', () => {
  const game = new GameManager({ resources: { research: 100 } });
  assert.equal(game.unlockResearch('behavioral-analysis').ok, false);
  assert.equal(game.unlockResearch('network-hardening').ok, true);
  assert.equal(game.unlockResearch('network-hardening').ok, false);
  assert.equal(game.getState().resources.research, 60);
});

test('three campaign missions can be completed with their dedicated action sequences', () => {
  const game = new GameManager();
  assert.equal(game.getState().missions.length, 3);
  game.startMission('mission-01');
  const chapterOne = [
    ['inspect_process', {}], ['trace_origin', { answer: '185.220.101.5' }],
    ['configure_firewall', { answer: 'deny-untrusted' }], ['quarantine_process', {}],
    ['restore_files', { answer: 'chkpt-0003' }], ['submit_report', { answer: 'critical-contained' }]
  ];
  for (const [action, payload] of chapterOne) assert.equal(game.performAction(action, payload).ok, true, action);
  game.startMission('mission-02');
  const chapterTwo = [
    ['inspect_memory', {}], ['revoke_token', { answer: 'TOK-KERNEL-ESCALATED' }],
    ['apply_patch', { answer: 'AEGIS-KERNEL-2.0.4' }], ['quarantine_process', {}],
    ['audit_kernel', { answer: 'AEGIS-ROOT-VERIFIED' }]
  ];
  for (const [action, payload] of chapterTwo) assert.equal(game.performAction(action, payload).ok, true, action);
  assert.equal(game.getState().currentMission.status, 'completed');
  game.startMission('mission-03');
  const chapterThree = [
    ['analyze_traffic', {}], ['rate_limit', { answer: 'adaptive-750' }],
    ['reroute_traffic', { answer: 'edge-route-02' }], ['restore_service', { answer: 'api-gateway' }],
    ['submit_report', { answer: 'availability-restored' }]
  ];
  for (const [action, payload] of chapterThree) assert.equal(game.performAction(action, payload).ok, true, action);
  assert.equal(game.getState().currentMission.status, 'completed');
  assert.equal(game.getState().missions.filter(item => item.completed).length, 3);
});

test('save manager round-trips game state and rejects unsafe slots', () => {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'aegis-game-'));
  try {
    const game = new GameManager(); game.startMission(); game.performAction('inspect_process');
    const saves = new SaveManager(dir); saves.save(game, 'test-slot');
    const payload = saves.load('test-slot');
    const restored = new GameManager(); restored.restore(payload.game);
    assert.equal(restored.getState().currentMission.objectives[0].completed, true);
    assert.throws(() => saves.save(game, '../escape'));
  } finally { fs.rmSync(dir, { recursive: true, force: true }); }
});
