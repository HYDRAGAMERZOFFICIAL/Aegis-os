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
      trace_origin: payload => payload.answer === '185.220.101.5' ? ({ ok: true, detail: 'Correct source correlated: hostile relay 185.220.101.5 identified.' }) : ({ ok: false, reason: 'That address is not the hostile relay. Re-check the packet evidence.' }),
      configure_firewall: payload => payload.answer === 'deny-untrusted' ? ({ ok: true, detail: 'Aegis Shield policy updated: untrusted ingress denied.' }) : ({ ok: false, reason: 'Policy rejected. The rule must deny untrusted inbound traffic.' }),
      quarantine_process: () => { const threat = this.threats.list().find(item => item.status === 'active' && item.detected); if (!threat || !this.threats.contain(threat.instanceId)) return { ok: false, reason: 'Detect an active threat before quarantining it.' }; return { ok: true, detail: 'Threat isolated in the simulated quarantine.' }; },
      inspect_memory: () => { const threat = this.threats.list().find(item => item.status === 'active'); if (threat) this.threats.detect(threat.instanceId); return { ok: true, detail: 'Memory map inspected; anomalous privilege transition identified.' }; },
      revoke_token: payload => payload.answer === 'TOK-KERNEL-ESCALATED' ? ({ ok: true, detail: 'Compromised kernel capability token revoked.' }) : ({ ok: false, reason: 'Token mismatch. Verify which process requested elevated kernel access.' }),
      apply_patch: payload => payload.answer === 'AEGIS-KERNEL-2.0.4' ? ({ ok: true, detail: 'Verified AEGIS-KERNEL-2.0.4 training patch applied.' }) : ({ ok: false, reason: 'Patch signature is not approved for this kernel build.' }),
      audit_kernel: payload => payload.answer === 'AEGIS-ROOT-VERIFIED' ? ({ ok: true, detail: 'Kernel integrity audit passed; verified root evidence sealed.' }) : ({ ok: false, reason: 'Integrity proof does not match the trusted Aegis root.' }),
      analyze_traffic: () => { const threat = this.threats.list().find(item => item.status === 'active'); if (threat) this.threats.detect(threat.instanceId); return { ok: true, detail: 'Traffic telemetry analysed; coordinated flood pattern identified.' }; },
      rate_limit: payload => { if (payload.answer !== 'adaptive-750') return { ok: false, reason: 'That traffic profile will not protect priority services. Select the adaptive ingress profile.' }; const threat = this.threats.list().find(item => item.status === 'active' && item.detected); if (!threat || !this.threats.contain(threat.instanceId)) return { ok: false, reason: 'Analyse an active traffic threat before applying rate limits.' }; return { ok: true, detail: 'Adaptive 750-request ingress rate limit applied; simulated flood contained.' }; },
      reroute_traffic: payload => payload.answer === 'edge-route-02' ? ({ ok: true, detail: 'Critical service traffic rerouted through resilient edge route 02.' }) : ({ ok: false, reason: 'Route health check failed. Choose the resilient edge route.' }),
      restore_service: payload => payload.answer === 'api-gateway' ? ({ ok: true, detail: 'Aegis API gateway restored in the isolated simulation.' }) : ({ ok: false, reason: 'Select the degraded API gateway service to restore availability.' }),
      restore_files: payload => payload.answer === 'chkpt-0003' ? ({ ok: true, detail: 'Clean checkpoint chkpt-0003 restored; no host files were accessed.' }) : ({ ok: false, reason: 'That checkpoint is not the verified clean recovery point.' }),
      submit_report: payload => { const expected = { 'mission-01': 'critical-contained', 'mission-02': 'privilege-contained', 'mission-03': 'availability-restored' }[this.missions.getCurrent().id]; return payload.answer === expected ? ({ ok: true, detail: 'Incident report accepted by Aegis Command.' }) : ({ ok: false, reason: 'The incident report does not match the evidence and recovery outcome.' }); }
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
