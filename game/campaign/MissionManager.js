'use strict';
class MissionManager {
  constructor(eventBus, resources) { this.events = eventBus; this.resources = resources; this.missions = new Map(); this.current = null; this.completed = new Set(); }
  register(mission) { if (!mission || !mission.id || !Array.isArray(mission.objectives)) throw new TypeError('Mission requires an id and objectives.'); this.missions.set(mission.id, JSON.parse(JSON.stringify(mission))); }
  start(id) {
    const mission = this.missions.get(id); if (!mission) throw new Error('Mission not found.');
    if (this.completed.has(id)) throw new Error('Mission already completed.');
    this.current = { id, startedAt: Date.now(), status: 'active', objectives: mission.objectives.map(item => ({ ...item, completed: false })), actions: [] };
    this.events.emit('mission:started', this.getCurrent()); return this.getCurrent();
  }
  recordAction(action) {
    if (!this.current || this.current.status !== 'active') return { ok: false, reason: 'No active mission.' };
    const nextObjective = this.current.objectives.find(item => !item.completed); const accepted = nextObjective && nextObjective.action === action ? [nextObjective] : [];
    if (!accepted.length) return { ok: false, reason: 'That action does not advance an outstanding objective.' };
    accepted.forEach(item => { item.completed = true; this.events.emit('mission:objective', { missionId: this.current.id, objectiveId: item.id }); });
    this.current.actions.push({ action, at: Date.now() });
    const done = this.current.objectives.every(item => item.completed);
    if (done) {
      this.current.status = 'completed'; this.completed.add(this.current.id);
      const definition = this.missions.get(this.current.id); const reward = definition.rewards || {};
      this.resources.add(reward); this.events.emit('mission:completed', { missionId: this.current.id, rewards: reward });
    }
    return { ok: true, completed: done, mission: this.getCurrent() };
  }
  getCurrent() { return this.current ? JSON.parse(JSON.stringify(this.current)) : null; }
  list() { return [...this.missions.values()].map(item => ({ id: item.id, title: item.title, description: item.description, difficulty: item.difficulty, completed: this.completed.has(item.id) })); }
  serialize() { return { current: this.getCurrent(), completed: [...this.completed] }; }
  restore(state = {}) { this.completed = new Set((state.completed || []).filter(id => this.missions.has(id))); this.current = state.current ? JSON.parse(JSON.stringify(state.current)) : null; }
}
module.exports = MissionManager;
