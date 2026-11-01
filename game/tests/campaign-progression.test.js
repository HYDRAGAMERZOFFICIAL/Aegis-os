'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const GameManager = require('../core/GameManager');

test('campaign operations unlock sequentially after the previous operation is secured', () => {
  const game = new GameManager();
  assert.throws(() => game.startMission('mission-02'), /Complete The Silent Intrusion/);
  game.startMission('mission-01');
  assert.equal(game.performAction('inspect_process').ok, true);
  assert.equal(game.performAction('trace_origin', { answer: '10.0.0.8' }).ok, false, 'incorrect intelligence must not advance the mission');
  assert.equal(game.getState().currentMission.objectives[1].completed, false);
  const steps = [
    ['trace_origin', { answer: '185.220.101.5' }],
    ['configure_firewall', { answer: 'deny-untrusted' }], ['quarantine_process', {}],
    ['restore_files', { answer: 'chkpt-0003' }], ['submit_report', { answer: 'critical-contained' }]
  ];
  for (const [action, payload] of steps) {
    const result = game.performAction(action, payload);
    assert.equal(result.ok, true, action + ' should advance the mission');
  }
  const missions = game.getState().missions;
  assert.equal(missions[0].completed, true);
  assert.equal(missions[1].unlocked, true);
  assert.equal(missions[2].unlocked, false);
  game.startMission('mission-02');
  assert.equal(game.getState().currentMission.id, 'mission-02');
  const chapterTwo = [
    ['inspect_memory', {}], ['revoke_token', { answer: 'TOK-KERNEL-ESCALATED' }],
    ['apply_patch', { answer: 'AEGIS-KERNEL-2.0.4' }], ['quarantine_process', {}],
    ['audit_kernel', { answer: 'AEGIS-ROOT-VERIFIED' }], ['submit_report', { answer: 'privilege-contained' }]
  ];
  for (const [action, payload] of chapterTwo) assert.equal(game.performAction(action, payload).ok, true, action);
  assert.equal(game.getState().missions[2].unlocked, true);
  game.startMission('mission-03');
  const chapterThree = [
    ['analyze_traffic', {}], ['rate_limit', { answer: 'adaptive-750' }],
    ['reroute_traffic', { answer: 'edge-route-02' }], ['restore_service', { answer: 'api-gateway' }],
    ['submit_report', { answer: 'availability-restored' }]
  ];
  for (const [action, payload] of chapterThree) assert.equal(game.performAction(action, payload).ok, true, action);
  assert.equal(game.getState().missions.every(mission => mission.completed), true);
});
