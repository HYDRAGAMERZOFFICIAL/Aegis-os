'use strict';
const THREATS = [
  { id: 'intrusion-probe', name: 'Credential reconnaissance', severity: 'medium', pressure: { stability: -3, reputation: -1 } },
  { id: 'ransomware-stager', name: 'Ransomware staging activity', severity: 'critical', pressure: { stability: -8, memory: -5 } },
  { id: 'data-exfiltration', name: 'Unusual outbound transfer', severity: 'high', pressure: { reputation: -5, bandwidth: -8 } }
];
class ThreatEngine {
  constructor(eventBus, resources) { this.events = eventBus; this.resources = resources; this.active = []; this.sequence = 0; this.turn = 0; }
  spawn(id) {
    const template = THREATS.find(item => item.id === id); if (!template) throw new Error('Unknown threat type.');
    const threat = { ...template, pressure: { ...template.pressure }, instanceId: `THR-${String(++this.sequence).padStart(4, '0')}`, status: 'active', detected: false };
    this.active.push(threat); this.events.emit('threat:spawned', threat); return { ...threat, pressure: { ...threat.pressure } };
  }
  advanceTurn() { this.turn += 1; for (const threat of this.active.filter(item => item.status === 'active')) this.resources.applyPressure(threat.pressure); this.events.emit('threat:turn', { turn: this.turn, active: this.active.length }); }
  detect(instanceId) { const threat = this.active.find(item => item.instanceId === instanceId && item.status === 'active'); if (!threat) return false; threat.detected = true; this.events.emit('threat:detected', threat); return true; }
  contain(instanceId) { const threat = this.active.find(item => item.instanceId === instanceId && item.status === 'active'); if (!threat) return false; threat.status = 'contained'; this.events.emit('threat:contained', threat); return true; }
  list() { return this.active.map(item => ({ ...item, pressure: { ...item.pressure } })); }
  serialize() { return { active: this.list(), sequence: this.sequence, turn: this.turn }; }
  restore(state = {}) { this.active = Array.isArray(state.active) ? state.active.map(item => ({ ...item, pressure: { ...item.pressure } })) : []; this.sequence = Number.isInteger(state.sequence) ? state.sequence : 0; this.turn = Number.isInteger(state.turn) ? state.turn : 0; }
}
ThreatEngine.TYPES = THREATS.map(item => ({ ...item, pressure: { ...item.pressure } }));
module.exports = ThreatEngine;
