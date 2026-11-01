'use strict';
class ResearchManager {
  constructor(tree = []) { this.nodes = new Map(tree.map(node => [node.id, { ...node, prerequisites: [...(node.prerequisites || [])] }])); this.unlocked = new Set(); }
  list() { return [...this.nodes.values()].map(node => ({ ...node, prerequisites: [...node.prerequisites], unlocked: this.unlocked.has(node.id), available: this.isAvailable(node.id) })); }
  isAvailable(id) { const node = this.nodes.get(id); return !!node && !this.unlocked.has(id) && node.prerequisites.every(dep => this.unlocked.has(dep)); }
  unlock(id, resources) {
    const node = this.nodes.get(id); if (!node) return { ok: false, reason: 'Unknown research node.' };
    if (this.unlocked.has(id)) return { ok: false, reason: 'Research already completed.' };
    if (!this.isAvailable(id)) return { ok: false, reason: 'Prerequisites are not complete.' };
    if (!resources.spend({ research: node.cost || 0 })) return { ok: false, reason: 'Not enough research points.' };
    this.unlocked.add(id); return { ok: true, node: { ...node } };
  }
  serialize() { return [...this.unlocked]; }
  restore(ids = []) { this.unlocked = new Set(ids.filter(id => this.nodes.has(id))); }
}
module.exports = ResearchManager;
