"use strict";
const test = require("node:test");
const assert = require("node:assert/strict");
const store = new Map();
global.window = global;
global.localStorage = {
  getItem:key=>store.has(key)?store.get(key):null,
  setItem:(key,value)=>store.set(key,String(value)),
  removeItem:key=>store.delete(key)
};
require("../engine/missions.js");
require("../engine/engine.js");

test.beforeEach(()=>store.clear());

test("mission objectives must be completed in order",()=>{
  const game=new AegisGameEngine();
  assert.equal(game.startMission("silent-intrusion").ok,true);
  const skipped=game.perform("scan");
  assert.equal(skipped.ok,false);
  assert.match(skipped.message,/sequence/i);
});
test("first campaign mission completes and awards its defined rewards",()=>{
  const game=new AegisGameEngine();
  game.startMission("silent-intrusion");
  for(const step of ["inspect","isolate","scan","restore"]) {
    if(step==="isolate") game.decide("preserve");
    const result=game.perform(step);
    if(step==="restore") {
      assert.equal(result.complete,true);
      assert.equal(result.reward.xp,300);
    } else assert.equal(result.ok,true);
  }
  const saved=game.snapshot();
  assert.deepEqual(saved.completed,["silent-intrusion"]);
  assert.equal(saved.player.credits,715);
  assert.equal(saved.player.research,2);
  assert.equal(saved.stats.systemsRestored,1);
});
test("recovery cannot occur before containment and integrity verification",()=>{
  const game=new AegisGameEngine();
  game.startMission("silent-intrusion");
  game.perform("inspect");
  const noDecision=game.perform("isolate");
  assert.equal(noDecision.ok,false);
  game.decide("preserve");
  const premature=game.perform("restore");
  assert.equal(premature.ok,false);
  const restore=game.perform("restore");
  assert.equal(restore.ok,false);
  assert.match(restore.message,/sequence/i);
});
test("campaign progress survives engine recreation",()=>{
  const first=new AegisGameEngine();
  first.startMission("silent-intrusion");
  first.perform("inspect"); first.decide("preserve");
  ["isolate","scan","restore"].forEach(step=>first.perform(step));
  const second=new AegisGameEngine();
  assert.deepEqual(second.snapshot().completed,["silent-intrusion"]);
  assert.equal(second.snapshot().player.xp,300);
});
test("research upgrades enforce costs and prevent duplicate purchases",()=>{
  const game=new AegisGameEngine();
  const denied=game.buyUpgrade("rapid-triage");
  assert.equal(denied.ok,false);
  game.state.player.research=2;
  const bought=game.buyUpgrade("rapid-triage");
  assert.equal(bought.ok,true);
  assert.equal(game.buyUpgrade("rapid-triage").ok,false);
});


test("branching response choices affect grade and bonus rewards",()=>{
  const game=new AegisGameEngine();
  game.startMission("silent-intrusion");
  game.perform("inspect");
  assert.equal(game.perform("isolate").ok,false);
  assert.equal(game.decide("preserve").ok,true);
  for(const step of ["isolate","scan","restore"]) {
    const result=game.perform(step);
    if(step==="restore") {
      assert.equal(result.grade,"A");
      assert.equal(result.score,100);
      assert.equal(result.reward.xp,300);
      assert.equal(result.reward.credits,215);
    }
  }
  assert.equal(game.snapshot().stats.perfectMissions,1);
});
test("active mission and selected decision survive engine recreation",()=>{
  const first=new AegisGameEngine();
  first.startMission("silent-intrusion");
  first.perform("inspect");
  first.decide("terminate");
  const second=new AegisGameEngine();
  assert.equal(second.getSession().missionId,"silent-intrusion");
  assert.equal(second.getSession().decision.choiceId,"terminate");
});
