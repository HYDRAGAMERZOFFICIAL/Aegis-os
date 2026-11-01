const fs = require('fs');
const path = require('path');

const file = path.join(__dirname, '..', 'simulator', 'server.js');
let content = fs.readFileSync(file, 'utf8');

const addition = `        case 'achievements':
            return callback('AEGIS OPERATOR ACHIEVEMENTS & MEDALS:\\n' +
              '  [1]  🛡️ First Line of Defense        - Contain first security breach\\n' +
              '  [2]  ⏱️ Crisis Responder             - Neutralize a time-critical attack\\n' +
              '  [3]  ⚡ Sub-60s SecOps               - Neutralize incident under 60 seconds\\n' +
              '  [4]  🔐 Zero-Trust Sentinel          - Revoke unauthorized capability token\\n' +
              '  [5]  🧬 Merkle Restorer              - Perform atomic Merkle rollback\\n' +
              '  [6]  📡 Traffic Forensics            - Correlate external C2 beacon relay\\n' +
              '  [7]  🧠 Forensic Deduction Master    - Submit 100% correct case report\\n' +
              '  [8]  💻 Ring-0 Operator              - Execute 8 security shell commands\\n' +
              '  [9]  🌐 Ephemeral Airgap             - Browse via isolated Ghost RAM sandbox\\n' +
              '  [10] 📁 Cryptographic Custody        - Encrypt evidence file in AegisFS\\n' +
              '  [11] 🏆 Grade S Commander            - Secure operational case with Grade S\\n' +
              '  [12] 👑 Cyber Fortress Vanguard      - Complete all 3 cases and repel APT\\n\\n' +
              'Check active progress in the Case Dossier HUD (Hotkey: F4).');

        case 'drill':
        case 'simulate-attack':
            return callback('[EMERGENCY SEC-OPS DRILL TRIGGERED]\\n' +
              'Alarm initialized: Dynamic time-critical incident launched.\\n' +
              'Consult Aegis Mind Copilot and deploy defensive countermeasures before expiration!');

        case 'emergency-status':
            return callback('EMERGENCY INCIDENT SYSTEM STATUS:\\n' +
              'Real-time intrusion detector: ARMED (Zero-Trust Heuristics)\\n' +
              'Automated drill dispatch: READY\\n' +
              'Issue \\'drill\\' or \\'simulate-attack\\' to trigger a live emergency scenario.');

`;

const marker = "default:";
const idx = content.lastIndexOf(marker);
if (idx !== -1) {
  content = content.slice(0, idx) + addition + content.slice(idx);
  fs.writeFileSync(file, content, 'utf8');
  console.log('Successfully injected cases into server.js');
} else {
  console.error('Could not find default: in server.js');
  process.exit(1);
}
