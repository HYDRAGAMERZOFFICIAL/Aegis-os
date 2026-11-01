'use strict';
const fs = require('fs');
const path = require('path');
const SAVE_VERSION = 1;
class SaveManager {
  constructor(directory) { this.directory = path.resolve(directory); }
  _target(slot = 'profile') {
    if (!/^[a-zA-Z0-9_-]{1,32}$/.test(slot)) throw new Error('Invalid save slot name.');
    const target = path.resolve(this.directory, `${slot}.json`);
    if (!target.startsWith(this.directory + path.sep)) throw new Error('Invalid save path.');
    return target;
  }
  save(game, slot = 'profile') {
    if (!game || typeof game.serialize !== 'function') throw new TypeError('A serializable game instance is required.');
    fs.mkdirSync(this.directory, { recursive: true });
    const target = this._target(slot); const temporary = `${target}.tmp`;
    const payload = { saveVersion: SAVE_VERSION, savedAt: new Date().toISOString(), game: game.serialize() };
    fs.writeFileSync(temporary, JSON.stringify(payload, null, 2), { encoding: 'utf8', mode: 0o600 });
    fs.renameSync(temporary, target); return { slot, savedAt: payload.savedAt };
  }
  load(slot = 'profile') {
    const payload = JSON.parse(fs.readFileSync(this._target(slot), 'utf8'));
    if (!payload || payload.saveVersion !== SAVE_VERSION || !payload.game) throw new Error('Save file is invalid or incompatible.');
    return payload;
  }
  list() { if (!fs.existsSync(this.directory)) return []; return fs.readdirSync(this.directory).filter(name => /^[a-zA-Z0-9_-]{1,32}\.json$/.test(name)).map(name => ({ slot: name.slice(0, -5), modifiedAt: fs.statSync(path.join(this.directory, name)).mtime.toISOString() })); }
}
module.exports = SaveManager;
