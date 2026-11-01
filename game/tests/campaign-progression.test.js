'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const GameManager = require('../core/GameManager');

test('campaign operations unlock sequentially after the previous operation is secured', () => {
  const game = new GameManager();
  assert.throws(() => game.startMission('mission-02'), /Complete The Silent Intrusion/);
  game.startMission('mission-01');
  assert.equal(game.performAction('trace_origin', { answer: '10.0.0.8' }).ok, false, 'incorrect intelligence must not advance the mission');
  assert.equal(game.getState().currentMission.objectives[1].completed, false);
  const steps = [
    ['inspect_process', {}], ['trace_origin', { answer: '185.220.101.5' }],
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
});
