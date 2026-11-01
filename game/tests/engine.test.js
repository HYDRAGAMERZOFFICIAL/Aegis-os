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
    const result=game.perform(step);
    if(step==="restore") {
      assert.equal(result.complete,true);
      assert.equal(result.reward.xp,250);
    } else assert.equal(result.ok,true);
  }
  const saved=game.snapshot();
  assert.deepEqual(saved.completed,["silent-intrusion"]);
  assert.equal(saved.player.credits,680);
  assert.equal(saved.player.research,2);
  assert.equal(saved.stats.systemsRestored,1);
});
test("recovery cannot occur before containment and integrity verification",()=>{
  const game=new AegisGameEngine();
  game.startMission("silent-intrusion");
  game.perform("inspect");
  const premature=game.perform("isolate");
  assert.equal(premature.ok,true);
  const restore=game.perform("restore");
  assert.equal(restore.ok,false);
  assert.match(restore.message,/integrity/i);
});
test("campaign progress survives engine recreation",()=>{
  const first=new AegisGameEngine();
  first.startMission("silent-intrusion");
  ["inspect","isolate","scan","restore"].forEach(step=>first.perform(step));
  const second=new AegisGameEngine();
  assert.deepEqual(second.snapshot().completed,["silent-intrusion"]);
  assert.equal(second.snapshot().player.xp,250);
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
