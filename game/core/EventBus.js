'use strict';
class EventBus {
  constructor() { this.listeners = new Map(); }
  on(event, listener) {
    if (typeof listener !== 'function') throw new TypeError('Listener must be a function');
    const bucket = this.listeners.get(event) || new Set(); bucket.add(listener); this.listeners.set(event, bucket);
    return () => this.off(event, listener);
  }
  once(event, listener) { const off = this.on(event, (...args) => { off(); listener(...args); }); return off; }
  off(event, listener) { const bucket = this.listeners.get(event); if (!bucket) return false; const removed = bucket.delete(listener); if (!bucket.size) this.listeners.delete(event); return removed; }
  emit(event, payload) { const bucket = this.listeners.get(event); if (!bucket) return 0; for (const listener of [...bucket]) listener(payload); return bucket.size; }
  clear() { this.listeners.clear(); }
}
module.exports = EventBus;
