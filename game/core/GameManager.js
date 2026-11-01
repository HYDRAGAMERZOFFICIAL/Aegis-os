'use strict';
const EventBus = require('./EventBus');
const ResourceManager = require('../systems/ResourceManager');
const ResearchManager = require('../systems/ResearchManager');
const ThreatEngine = require('../systems/ThreatEngine');
const MissionManager = require('../campaign/MissionManager');
const researchTree = require('../data/research.json');
const firstMission = require('../missions/mission-01.json');
class GameManager {
  constructor(options = {}) {
    this.events = new EventBus();
    this.resources = new ResourceManager(options.resources);
    this.research = new ResearchManager(researchTree);
    this.threats = new ThreatEngine(this.events, this.resources);
    this.missions = new MissionManager(this.events, this.resources);
    this.missions.register(firstMission);
    this.startedAt = Date.now();
    this.difficulty = options.difficulty || 'normal';
  }
  startMission(id = 'mission-01') { const mission = this.missions.start(id); if (!this.threats.list().some(item => item.status === 'active')) this.threats.spawn('ransomware-stager'); return mission; }
  performAction(action, payload = {}) {
    const handlers = {
      inspect_process: () => { const threat = this.threats.list().find(item => item.status === 'active'); if (threat) this.threats.detect(threat.instanceId); return { ok: true, detail: 'Process telemetry inspected.', threatId: threat && threat.instanceId }; },
      trace_origin: () => ({ ok: true, detail: 'Attack path reconstructed from simulated network telemetry.' }),
      configure_firewall: () => ({ ok: true, detail: 'Aegis Shield policy updated: suspicious ingress denied.' }),
      quarantine_process: () => { const threat = this.threats.list().find(item => item.status === 'active' && item.detected); if (!threat) return { ok: false, reason: 'Detect an active threat before quarantining it.' }; this.threats.contain(threat.instanceId); return { ok: true, detail: 'Threat isolated in the simulated quarantine.' }; },
      restore_files: () => ({ ok: true, detail: 'Simulated clean checkpoint restored; no host files were accessed.' }),
      submit_report: () => ({ ok: true, detail: 'Incident report submitted to Aegis Command.' })
    };
    if (!Object.prototype.hasOwnProperty.call(handlers, action)) return { ok: false, reason: 'Unsupported game action.' };
    const result = handlers[action](payload); if (!result.ok) return result;
    const progress = this.missions.recordAction(action);
    this.threats.advanceTurn();
    return { ...result, progress, state: this.getState() };
  }
  unlockResearch(id) { const result = this.research.unlock(id, this.resources); if (result.ok) this.events.emit('research:unlocked', result.node); return result; }
  spawnThreat(id) { return this.threats.spawn(id); }
  getState() { return { version: 1, difficulty: this.difficulty, resources: this.resources.getSnapshot(), research: this.research.list(), threats: this.threats.list(), missions: this.missions.list(), currentMission: this.missions.getCurrent(), elapsedSeconds: Math.floor((Date.now() - this.startedAt) / 1000) }; }
  serialize() { return { schemaVersion: 1, difficulty: this.difficulty, resources: this.resources.getSnapshot(), research: this.research.serialize(), threats: this.threats.serialize(), missions: this.missions.serialize() }; }
  restore(state) {
    if (!state || state.schemaVersion !== 1) throw new Error('Unsupported save schema.');
    this.resources = new ResourceManager(state.resources);
    this.threats = new ThreatEngine(this.events, this.resources); this.threats.restore(state.threats);
    this.research.restore(state.research); this.missions.resources = this.resources; this.missions.restore(state.missions);
    this.difficulty = state.difficulty || 'normal'; return this.getState();
  }
}
module.exports = GameManager;
