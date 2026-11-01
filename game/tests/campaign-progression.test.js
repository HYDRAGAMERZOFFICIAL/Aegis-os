'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const GameManager = require('../core/GameManager');

test('campaign operations unlock sequentially after the previous operation is secured', () => {
  const game = new GameManager();
  assert.throws(() => game.startMission('mission-02'), /Complete The Silent Intrusion/);
  game.startMission('mission-01');
  for (const action of ['inspect_process', 'trace_origin', 'configure_firewall', 'quarantine_process', 'restore_files', 'submit_report']) {
    const result = game.performAction(action);
    assert.equal(result.ok, true, action + ' should advance the mission');
  }
  const missions = game.getState().missions;
  assert.equal(missions[0].completed, true);
  assert.equal(missions[1].unlocked, true);
  assert.equal(missions[2].unlocked, false);
  game.startMission('mission-02');
  assert.equal(game.getState().currentMission.id, 'mission-02');
});
