'use strict';
const DEFAULTS = Object.freeze({ credits: 500, research: 0, cpu: 100, memory: 100, bandwidth: 100, stability: 100, reputation: 50 });
class ResourceManager {
  constructor(initial = {}) { this.values = { ...DEFAULTS, ...initial }; this._validateAll(); }
  _validateAll() { for (const [key, value] of Object.entries(this.values)) if (!Number.isFinite(value) || value < 0) throw new RangeError(`Invalid resource: ${key}`); }
  getSnapshot() { return { ...this.values }; }
  get(key) { if (!(key in this.values)) throw new Error(`Unknown resource: ${key}`); return this.values[key]; }
  canAfford(cost = {}) { return Object.entries(cost).every(([key, amount]) => Number.isFinite(amount) && amount >= 0 && key in this.values && this.values[key] >= amount); }
  spend(cost = {}) { if (!this.canAfford(cost)) return false; for (const [key, amount] of Object.entries(cost)) this.values[key] -= amount; return true; }
  add(reward = {}) { for (const [key, amount] of Object.entries(reward)) { if (!(key in this.values) || !Number.isFinite(amount)) throw new Error(`Invalid resource change: ${key}`); this.values[key] = Math.max(0, this.values[key] + amount); } return this.getSnapshot(); }
  set(key, value) { if (!(key in this.values) || !Number.isFinite(value) || value < 0) throw new Error(`Invalid resource value: ${key}`); this.values[key] = value; }
  applyPressure(changes = {}) { for (const [key, amount] of Object.entries(changes)) { if (!(key in this.values) || !Number.isFinite(amount)) continue; this.values[key] = Math.max(0, Math.min(100, this.values[key] + amount)); } return this.getSnapshot(); }
  static defaults() { return { ...DEFAULTS }; }
}
module.exports = ResourceManager;
