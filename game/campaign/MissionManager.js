'use strict';
class MissionManager {
  constructor(eventBus, resources) { this.events = eventBus; this.resources = resources; this.missions = new Map(); this.current = null; this.completed = new Set(); }
  register(mission) { if (!mission || !mission.id || !Array.isArray(mission.objectives)) throw new TypeError('Mission requires an id and objectives.'); this.missions.set(mission.id, JSON.parse(JSON.stringify(mission))); }
  start(id) {
    const mission = this.missions.get(id); if (!mission) throw new Error('Mission not found.');
    if (this.completed.has(id)) throw new Error('Mission already completed.');
    const previous = [...this.missions.values()].filter(item => (item.chapter || 0) < (mission.chapter || 0)).sort((a, b) => (b.chapter || 0) - (a.chapter || 0))[0];
    if (previous && !this.completed.has(previous.id)) throw new Error('Complete ' + previous.title + ' before deploying this operation.');
    if (this.current && this.current.status === 'active') throw new Error('Complete or abandon the active mission first.');
    this.current = { id, title: mission.title, description: mission.description, briefing: mission.briefing || '', startedAt: Date.now(), status: 'active', objectives: mission.objectives.map(item => ({ ...item, completed: false })), actions: [] };
    this.events.emit('mission:started', this.getCurrent()); return this.getCurrent();
  }
  canPerform(action) { if (!this.current || this.current.status !== 'active') return false; const next = this.current.objectives.find(item => !item.completed); return !!next && next.action === action; }
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
  list() {
    return [...this.missions.values()].map(item => {
      const previous = [...this.missions.values()].filter(candidate => (candidate.chapter || 0) < (item.chapter || 0)).sort((a, b) => (b.chapter || 0) - (a.chapter || 0))[0];
      const unlocked = !previous || this.completed.has(previous.id);
      return { id: item.id, chapter: item.chapter || 1, title: item.title, description: item.description, briefing: item.briefing || '', difficulty: item.difficulty || 'standard', objectiveCount: item.objectives.length, objectives: item.objectives.map(objective => ({ id: objective.id, title: objective.title, description: objective.description })), completed: this.completed.has(item.id), unlocked };
    });
  }
  serialize() { return { current: this.getCurrent(), completed: [...this.completed] }; }
  restore(state = {}) { this.completed = new Set((state.completed || []).filter(id => this.missions.has(id))); this.current = state.current ? JSON.parse(JSON.stringify(state.current)) : null; }
}
module.exports = MissionManager;
