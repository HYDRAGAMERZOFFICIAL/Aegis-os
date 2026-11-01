/* Aegis: The Last Firewall — deterministic, local-first game state. */
(function (root) {
  "use strict";
  const SAVE_KEY = "aegis.lastFirewall.save.v1";
  const missions = () => root.AegisGameMissions || [];
  const clone = value => JSON.parse(JSON.stringify(value));
  const initialState = () => ({
    schema:1, activeSession:null, player:{callsign:"ARCHITECT",level:1,xp:0,credits:500,research:0,reputation:0},
    completed:[], activeMission:null, upgrades:[], difficulty:"standard",
    stats:{incidentsResolved:0,perfectMissions:0,systemsRestored:0,decisions:0},
    createdAt:new Date().toISOString(), updatedAt:new Date().toISOString()
  });
  class AegisGameEngine {
    constructor() { this.state=this.load(); this.listeners=new Set(); this.session=this.state.activeSession?clone(this.state.activeSession):null; }
    load() {
      try { const saved=JSON.parse(localStorage.getItem(SAVE_KEY)); if(saved && saved.schema===1) { if(!("activeSession" in saved)) saved.activeSession=null; return saved; } } catch (_) {}
      return initialState();
    }
    save() { this.state.activeSession=this.session?clone(this.session):null; this.state.updatedAt=new Date().toISOString(); try { localStorage.setItem(SAVE_KEY,JSON.stringify(this.state)); } catch (_) {} this.emit(); }
    subscribe(fn) { this.listeners.add(fn); return () => this.listeners.delete(fn); }
    emit() { this.listeners.forEach(fn=>fn(this.snapshot())); }
    snapshot() { return clone(this.state); }
    getMissions() { return missions().map(m=>({...m,locked:m.chapter>this.unlockedChapter()})); }
    unlockedChapter() { return this.state.completed.length>=4?3:this.state.completed.length>=2?2:1; }
    getMission(id) { return missions().find(m=>m.id===id)||null; }
    startMission(id) {
      const mission=this.getMission(id); if(!mission || mission.chapter>this.unlockedChapter()) return {ok:false,message:"Mission locked."};
      if(this.state.completed.includes(id)) return {ok:false,message:"Mission already completed."};
      this.session={missionId:id,startedAt:Date.now(),steps:[],evidence:[],decision:null,contained:false,integrityVerified:false,restored:false,failed:false};
      this.state.activeMission=id; this.state.stats.decisions++; this.save();
      return {ok:true,session:this.getSession()};
    }
    getSession() { if(!this.session) return null; return {...clone(this.session),mission:clone(this.getMission(this.session.missionId))}; }
    abandonMission() { this.session=null; this.state.activeMission=null; this.save(); }
    decide(choiceId) {
      if(!this.session) return {ok:false,message:"No active mission."};
      const mission=this.getMission(this.session.missionId), decision=mission&&mission.decision;
      if(!decision) return {ok:false,message:"This mission has no branching decision."};
      if(this.session.steps.length!==1) return {ok:false,message:"Review the initial evidence before choosing a response."};
      if(this.session.decision) return {ok:false,message:"A response strategy has already been selected."};
      const choice=decision.options.find(option=>option.id===choiceId);
      if(!choice) return {ok:false,message:"Unknown response strategy."};
      this.session.decision={choiceId,correct:choiceId===decision.bestChoice,label:choice.label,reason:choice.reason};
      this.state.stats.decisions++;
      this.save();
      return {ok:true,decision:clone(this.session.decision),message:choiceId===decision.bestChoice?"Evidence supports this response.":"Response recorded. Review the consequences in the debrief."};
    }
    perform(action) {
      if(!this.session) return {ok:false,message:"No active mission."};
      const s=this.session, m=this.getMission(s.missionId);
      if(s.steps.includes(action)) return {ok:false,message:"This objective is already complete."};
      const order=["inspect","isolate","scan","restore"], expected=order[s.steps.length];
      if(action!==expected) return {ok:false,message:"Complete the objectives in sequence to preserve evidence."};
      if(action==="inspect") { s.evidence.push("Process telemetry captured","Origin and privilege profile recorded"); }
      if(action==="isolate") { if(m.decision&&!s.decision) return {ok:false,message:"Choose a response strategy before containment."}; s.contained=true; s.evidence.push("Suspicious activity contained"); }
      if(action==="scan") { if(!s.contained) return {ok:false,message:"Contain the incident before running the integrity verification."}; s.integrityVerified=true; s.evidence.push("Integrity report generated"); }
      if(action==="restore") { if(!s.integrityVerified) return {ok:false,message:"A verified integrity report is required before recovery."}; s.restored=true; s.evidence.push("Known-good recovery point applied"); }
      s.steps.push(action); this.state.stats.decisions++; this.save();
      if(s.steps.length===4) return this.completeMission();
      return {ok:true,session:this.getSession(),message:"Objective complete."};
    }
    completeMission() {
      const m=this.getMission(this.session.missionId), reward={...m.reward};
      const decisionScore=this.session.decision?(this.session.decision.correct?100:60):100;
      if(this.session.decision&&this.session.decision.correct){reward.xp+=50;reward.credits+=35;this.state.stats.perfectMissions++;}
      this.state.completed.push(m.id); this.state.activeMission=null;
      this.state.player.xp+=reward.xp; this.state.player.credits+=reward.credits; this.state.player.research+=reward.research;
      this.state.player.reputation+=10; this.state.stats.incidentsResolved++; this.state.stats.systemsRestored++;
      const oldLevel=this.state.player.level;
      this.state.player.level=Math.floor(this.state.player.xp/500)+1;
      if(this.state.player.level>oldLevel) this.state.player.credits+=100*(this.state.player.level-oldLevel);
      const result={ok:true,complete:true,mission:clone(m),reward:clone(reward),decision:clone(this.session.decision),score:decisionScore,grade:decisionScore===100?"A":"B",levelUp:this.state.player.level>oldLevel,session:this.getSession()};
      this.session=null; this.save(); return result;
    }
    buyUpgrade(id) {
      const catalog=UPGRADES.find(u=>u.id===id); if(!catalog) return {ok:false,message:"Upgrade not found."};
      if(this.state.upgrades.includes(id)) return {ok:false,message:"Upgrade already installed."};
      if(this.state.player.research<catalog.cost) return {ok:false,message:"Not enough research points."};
      this.state.player.research-=catalog.cost; this.state.upgrades.push(id); this.save();
      return {ok:true,message:catalog.name+" installed."};
    }
    reset() { this.state=initialState(); this.session=null; this.save(); }
  }
  const UPGRADES=[
    {id:"rapid-triage",name:"Rapid Triage",branch:"Aegis Guard",cost:2,description:"Improves incident assessment and reduces investigation uncertainty."},
    {id:"adaptive-firewall",name:"Adaptive Firewall",branch:"Aegis Shield",cost:3,description:"Unlocks more resilient simulated network containment policies."},
    {id:"integrity-chain",name:"Integrity Chain",branch:"AegisFS",cost:3,description:"Adds stronger snapshot verification to recovery operations."},
    {id:"mind-assist",name:"Aegis Mind Assist",branch:"AI Systems",cost:4,description:"Provides additional diagnostic context during complex incidents."},
    {id:"zero-trust-plus",name:"Zero-Trust Plus",branch:"Access Control",cost:5,description:"Improves capability auditing and least-privilege controls."},
    {id:"distributed-response",name:"Distributed Response",branch:"Network",cost:6,description:"Unlocks coordinated response planning for multi-node incidents."}
  ];
  root.AegisGameEngine=AegisGameEngine;
  root.AegisGameUpgrades=UPGRADES;
})(window);