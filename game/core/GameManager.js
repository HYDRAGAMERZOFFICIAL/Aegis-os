'use strict';
const EventBus = require('./EventBus');
const ResourceManager = require('../systems/ResourceManager');
const ResearchManager = require('../systems/ResearchManager');
const ThreatEngine = require('../systems/ThreatEngine');
const MissionManager = require('../campaign/MissionManager');
const researchTree = require('../data/research.json');
const missions = [require('../missions/mission-01.json'), require('../missions/mission-02.json'), require('../missions/mission-03.json')];
class GameManager {
  constructor(options = {}) {
    this.events = new EventBus();
    this.resources = new ResourceManager(options.resources);
    this.research = new ResearchManager(researchTree);
    this.threats = new ThreatEngine(this.events, this.resources);
    this.missions = new MissionManager(this.events, this.resources);
    missions.forEach(mission => this.missions.register(mission));
    this.startedAt = Date.now();
    this.difficulty = options.difficulty || 'normal';
  }
  startMission(id = 'mission-01') { const mission = this.missions.start(id); if (!this.threats.list().some(item => item.status === 'active')) { const threatByMission = { 'mission-01': 'ransomware-stager', 'mission-02': 'privilege-escalation', 'mission-03': 'ddos-flood' }; this.threats.spawn(threatByMission[id] || 'intrusion-probe'); } return mission; }
  performAction(action, payload = {}) {
    const handlers = {
      inspect_process: () => { const threat = this.threats.list().find(item => item.status === 'active'); if (threat) this.threats.detect(threat.instanceId); return { ok: true, detail: 'Process telemetry inspected.', threatId: threat && threat.instanceId }; },
      trace_origin: () => ({ ok: true, detail: 'Attack path reconstructed from simulated network telemetry.' }),
      configure_firewall: () => ({ ok: true, detail: 'Aegis Shield policy updated: suspicious ingress denied.' }),
      quarantine_process: () => { const threat = this.threats.list().find(item => item.status === 'active' && item.detected); if (!threat || !this.threats.contain(threat.instanceId)) return { ok: false, reason: 'Detect an active threat before quarantining it.' }; return { ok: true, detail: 'Threat isolated in the simulated quarantine.' }; },
      inspect_memory: () => { const threat = this.threats.list().find(item => item.status === 'active'); if (threat) this.threats.detect(threat.instanceId); return { ok: true, detail: 'Memory map inspected; anomalous privilege transition identified.' }; },
      revoke_token: () => ({ ok: true, detail: 'Compromised capability token revoked in the simulation.' }),
      apply_patch: () => ({ ok: true, detail: 'Verified training patch applied to the simulated kernel.' }),
      audit_kernel: () => ({ ok: true, detail: 'Kernel integrity audit passed; evidence sealed in the local incident record.' }),
      analyze_traffic: () => { const threat = this.threats.list().find(item => item.status === 'active'); if (threat) this.threats.detect(threat.instanceId); return { ok: true, detail: 'Traffic telemetry analysed; coordinated flood pattern identified.' }; },
      rate_limit: () => { const threat = this.threats.list().find(item => item.status === 'active' && item.detected); if (!threat || !this.threats.contain(threat.instanceId)) return { ok: false, reason: 'Analyse an active traffic threat before applying rate limits.' }; return { ok: true, detail: 'Ingress rate limits applied; simulated flood contained.' }; },
      reroute_traffic: () => ({ ok: true, detail: 'Critical service traffic rerouted through the resilient simulated route.' }),
      restore_service: () => ({ ok: true, detail: 'Service health restored in the isolated simulation.' }),
      restore_files: () => ({ ok: true, detail: 'Simulated clean checkpoint restored; no host files were accessed.' }),
      submit_report: () => ({ ok: true, detail: 'Incident report submitted to Aegis Command.' })
    };
    if (!Object.prototype.hasOwnProperty.call(handlers, action)) return { ok: false, reason: 'Unsupported game action.' };
    if (!this.missions.canPerform(action)) return { ok: false, reason: 'Action is unavailable until the previous objective is completed.' };
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
