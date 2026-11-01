const fs = require('fs');
const path = require('path');

const file = path.resolve('simulator/web/native-campaign.js');
let code = fs.readFileSync(file, 'utf8');

// 1. Add extra TIMED_SCENARIOS
const extraScenarios = `  {
    id: 'emg-portknock',
    title: 'PORT-KNOCK BRUTE FORCE ASSAULT',
    threatType: 'Stealth Ingress Probe',
    seconds: 45,
    brief: 'Adversary executing rapid sequence port knocking across ports 7000-9000 to trigger root backdoor listener! Deploy firewall rule or run verify-containment now!',
    defusalActions: ['change:shield-rule', 'terminal:verify-containment'],
    promptClue: 'Enforce Aegis Shield drop rule or run verify-containment in Terminal!'
  },
  {
    id: 'emg-kernel-leak',
    title: 'KERNEL MEMORY LEAK EXPLOITATION',
    threatType: 'Heap Overflow Attempt',
    seconds: 40,
    brief: 'Rogue worker leaking ASLR entropy pages to extract kernel capability tokens! Quarantine the component or execute Merkle rollback before ring-0 is compromised!',
    defusalActions: ['terminal:quarantine-component', 'click:merkle-rollback'],
    promptClue: 'Issue quarantine-component in Terminal or click Rollback in Merkle Filesystem!'
  },
  {
    id: 'emg-db-ransom',
    title: 'DATABASE RANSOM LOCK ATTEMPT',
    threatType: 'AegisDB Table Poisoning',
    seconds: 35,
    brief: 'Malicious process injecting corrupted payloads into user_case_files and user_audit_logs! Flush AegisDB cache and revoke rogue token before database locks!',
    defusalActions: ['click:shield-token-audit', 'terminal:capability-audit'],
    promptClue: 'Audit capability tokens in Aegis Shield or run db flush in Terminal!'
  },
  {
    id: 'emg-quantum-exfil',
    title: 'QUANTUM ASYMMETRIC KEY EXFILTRATION',
    threatType: 'Covert Channel Relay',
    seconds: 30,
    brief: 'Hostile C2 relay 198.51.100.22 exfiltrating post-quantum enclave keys on port 8443! Reroute edge path or sever ingress rule immediately!',
    defusalActions: ['change:shield-rate-policy', 'terminal:route-select'],
    promptClue: 'Issue route-select edge-route-02 in Terminal or toggle rate policy in Shield!'
  },
`;

if (!code.includes('emg-portknock')) {
    code = code.replace(
        "var TIMED_SCENARIOS = [",
        "var TIMED_SCENARIOS = [\r\n" + extraScenarios
    );
}

// 2. Add extra operations op4, op5, op6
const extraMissions = `,
{id:'op4',title:'ZERO-DAY RANSOMWARE HEIST',brief:'A fast-propagating lateral ransomware worm compromised research partition /userland/data/. Reconstruct wiped cryptographic inode headers with the Hex Carver, trace the encryption key in volatile memory, and revoke the malicious capability token.',pressure:'Delay will cause irreversible userland partition loss. Carve the wiped inode header using expected magic bytes.',steps:[
['Detect ransomware process ancestry','Inspect Neural Monitor. Compare process ancestry and identify the anomalous ransomware dropper thread.','Look for worker threads executing outside assigned memory arenas.',['open:neural','click:neural-process-detail']],
['Triage network beacon in CyberSec Arsenal','Inspect packet telemetry for outbound ransomware encryption key exchange.','Correlate high-entropy outbound payloads on port 8443.',['open:cybersec','click:cybersec-packet-inspect']],
['Inspect corrupted file inode in Files','Inspect the damaged user_vault.dat in Files & Storage. Note the corrupted header bytes.','Verify file size, permissions, and corrupted inode status.',['open:files','click:files-inspect-suspicious']],
['Carve wiped inode headers in File Recovery','Open Files & Storage, locate user_vault.dat.corrupt, and deploy Hex Carver with magic bytes 56 41 55 4c.','Carving valid magic bytes restores the cryptographic enclave.',['open:storage','click:storage-recovery-carve']],
['Reconstruct timeline with Aegis Mind','Ask Aegis Mind to correlate ransomware dropper spawn, socket beacon, and header wipe.','Dropper spawn preceded header wipe.',['open:ai','click:ai-correlate-timeline']],
['Choose containment strategy','Decide whether to revoke elevated tokens, sever edge, or wipe partition.','Preserve forensic trail while stopping cryptographic propagation.','choice',['Emergency capability revocation & hex restore','Full partition wipe','Ignore threat']],
['Revoke rogue bearer capability in Shield','Use Aegis Shield to revoke unauthorized raw I/O tokens held by the dropper.','Cut raw disk write access immediately.',['open:shield','change:shield-rule']],
['Verify zero-trust containment','Run Aegis Guard and verify that ransomware threads are neutralized.','Run verify-containment in Terminal to confirm quarantine.',['click:shield-guard-scan','terminal:verify-containment']],
['Commit Merkle snapshot','Validate restored filesystem integrity with Merkle tree scan.','Verify that root hash matches clean baseline.',['open:merkle','click:merkle-integrity-scan']],
['Submit incident report','Submit root cause, carved magic bytes, and proof of containment.','All investigative evidence and defensible choice required.','report']
],choices:['Emergency capability revocation & hex restore','Full partition wipe','Ignore threat'],outcomes:['Ransomware neutralized and inodes restored. Zero data loss recorded.','Partition wiped, but valuable research archives destroyed.','Ransomware propagated across all nodes. Critical failure.'],quiz:[{id:'magic',question:'What is the expected 4-byte magic header for user_vault.dat?',placeholder:'HEX BYTES',accept:['56 41 55 4c','5641554c','VAUL'],hint:'Inspect the Hex Carver recovery hint for user_vault.dat.corrupt.'},{id:'capability',question:'Which privileged capability was abused by the ransomware dropper?',placeholder:'CAP_...',accept:['CAP_SYS_RAWIO','CAP_NET_RAW_SOCKET','CAP_RAWIO'],hint:'Check capability-audit in Terminal or Shield token registry.'}]},
{id:'op5',title:'QUANTUM CIPHER EXFILTRATION',brief:'Adversary APT-41 is attempting covert asymmetric exfiltration via steganographic DNS-SEC tunneling. Intercept the exfiltration channel, deploy targeted firewall rules, and roll back tainted storage.',pressure:'Do not terminate primary edge gateways. Isolate the DNS tunnel without knocking trusted services offline.',steps:[
['Audit resource telemetry in Task Optimizer','Inspect Task Optimizer resource utilization, queue depth, and socket throughput.','Look for anomalous spikes in network queue depth.',['open:optimizer','click:optimizer-resource-audit']],
['Trace DNS-SEC packet anomalies in CyberSec Arsenal','Inspect deep packet inspection logs for covert DNS-SEC TXT record exfiltration.','High entropy payloads in port 53 traffic indicate tunneling.',['open:cybersec','click:cybersec-packet-inspect']],
['Inspect network listener states in Terminal','Use Terminal to inspect socket listeners and service routes.','Run netstat and service-health to pinpoint degraded listener.',['open:terminal','terminal:netstat','terminal:service-health']],
['Correlate covert exfiltration with Aegis Mind','Ask Aegis Mind to compare legitimate DNS traffic with the covert exfiltration channel.','Steganographic tunneling uses repetitive TXT queries.',['open:ai','click:ai-correlate-traffic']],
['Query AegisDB audit logs for anomalous telemetry','Inspect AegisDB user_audit_logs for unverified midnight access attempts.','Locate anomalous IP destination in audit logs.',['open:database','click:db-audit-inspect']],
['Choose exfiltration mitigation strategy','Balance edge availability, false positive rate, and containment speed.','Adaptive filtering isolates hostile endpoints without downtime.','choice',['Deploy DNS sinkhole rule & Merkle rollback','Sever all network interfaces','Log only']],
['Apply firewall drop rule against adversary relay','Deploy Aegis Shield rule blocking hostile relay 198.51.100.22.','Ensure legitimate edge routes remain online.',['open:shield','change:shield-rate-policy']],
['Execute Merkle Atomic Rollback on affected block','Roll back corrupted block using Merkle immutable root tree.','Verify rollback takes under 10 seconds with zero data loss.',['click:merkle-rollback','terminal:verify-root']],
['Verify restored route integrity','Use Terminal to verify that resilient edge-route-02 carries traffic cleanly.','Run route-select edge-route-02 and check service-health.',['terminal:route-health','terminal:route-select']],
['Submit forensic attribution report','Submit adversary IP, exfiltration protocol, and proof of recovery.','Full post-mortem report required for case sign-off.','report']
],choices:['Deploy DNS sinkhole rule & Merkle rollback','Sever all network interfaces','Log only'],outcomes:['Covert DNS tunnel severed. Edge services maintained 100% availability.','Network shutdown stopped exfiltration, but severed operational edge.','Exfiltration succeeded. Hostile nation-state captured quantum cipher keys.'],quiz:[{id:'relay_ip',question:'Which adversary C2 IP address was hosting the DNS covert tunnel?',placeholder:'IP ADDRESS',accept:['198.51.100.22','185.220.101.5'],hint:'Check packet inspection telemetry in CyberSec Arsenal.'},{id:'protocol',question:'Which protocol was abused for covert channel framing?',placeholder:'PROTOCOL',accept:['DNS-SEC','DNS','UDP 53'],hint:'Compare protocol column in packet inspection table.'}]},
{id:'op6',title:'FIRMWARE SUPPLY-CHAIN POISONING',brief:'A compromised signed peripheral firmware component injected a shadow interrupt handler into ring-0. Triage microkernel diagnostics, kill the rogue ingress PID, and verify hardware SPI root-of-trust.',pressure:'Shadow interrupts bypass normal userland monitors. Root-of-trust verification is mandatory.',steps:[
['Execute microkernel ASLR diagnostic in Troubleshoot','Run system diagnostic scanner to test microkernel memory isolation.','Page table isolation detects overlapping ring transitions.',['open:troubleshoot','click:troubleshoot-run-diagnostic']],
['Map memory ring transition in Neural Monitor','Inspect Neural Monitor memory view to locate the rogue interrupt vector.','Look for hooks modifying IDT entries.',['open:neural','click:neural-memory-map']],
['Audit AegisDB user sessions and capability grants','Inspect AegisDB user_sessions to find unauthorized elevated session tokens.','Cross-reference active sessions with rogue worker processes.',['open:database','click:db-sessions-inspect']],
['Trace and kill rogue ingress daemon PID 4819','Use Terminal to inspect capability grants and terminate rogue worker PID 4819.','Run capability-audit and kill-rogue-pid to sever ingress.',['open:terminal','terminal:capability-audit','terminal:ps']],
['Correlate firmware signature anomalies with Aegis Mind','Ask Aegis Mind to correlate diagnostic faults, IDT hooks, and hardware tokens.','Root cause is firmware supply chain tampering.',['open:ai','click:ai-correlate-kernel']],
['Choose firmware remediation strategy','Choose the remediation procedure that restores root-of-trust without bricking the board.','Reflash verified microcode and execute atomic Merkle rollback.','choice',['Hardware SPI Flash verification & service restart','Disable microkernel paging','Defer maintenance']],
['Quarantine rogue firmware driver','Run Aegis Guard and quarantine the untrusted hardware driver.','Run quarantine-component in Terminal to lock down driver.',['click:shield-guard-scan','terminal:quarantine-component']],
['Verify root of trust with Merkle scan','Run Merkle integrity verification to confirm root hash is verified clean.','Run verify-root after scan to complete cryptographic audit.',['open:merkle','click:merkle-integrity-scan','terminal:verify-root']],
['Restart hardened ingress gateway','Verify and restart degraded ingress services in Terminal.','Run service-restart to return gateway to HEALTHY state.',['terminal:service-health','click:optimizer-resource-audit']],
['Submit final Master Operator dossier','Submit full root cause, driver provenance, and cryptographic verification proof.','Final dossier required for Grandmaster Operator certification.','report']
],choices:['Hardware SPI Flash verification & service restart','Disable microkernel paging','Defer maintenance'],outcomes:['Microkernel root-of-trust re-established. Firmware integrity certified.','Disabling paging introduced severe privilege vulnerabilities.','Kernel compromised. System security compromised.'],quiz:[{id:'rogue_pid',question:'Which rogue daemon PID hooked the shadow interrupt vector?',placeholder:'PID',accept:['4819'],hint:'Inspect Terminal capability-audit output for the anomalous listener.'},{id:'subsystem',question:'Which security subsystem verified the hardware root of trust?',placeholder:'SUBSYSTEM',accept:['Merkle','ASLR','Microkernel Page Table Isolation'],hint:'Review Merkle integrity scanner and Troubleshoot diagnostic.'}]}
`;

if (!code.includes("id:'op4'")) {
    const op3Marker = "hint:'The route-health output shows which edge has priority capacity and low queue depth.'}]} \r\n];";
    const op3MarkerAlt = "hint:'The route-health output shows which edge has priority capacity and low queue depth.'}]}\r\n];";
    if (code.includes(op3MarkerAlt)) {
        code = code.replace(op3MarkerAlt, "hint:'The route-health output shows which edge has priority capacity and low queue depth.'}]}" + extraMissions + "\r\n];");
    } else if (code.includes(op3Marker)) {
        code = code.replace(op3Marker, "hint:'The route-health output shows which edge has priority capacity and low queue depth.'}]}" + extraMissions + "\r\n];");
    } else {
        code = code.replace(
            /\{id:'op3'[\s\S]*?\}\s*\];/,
            (match) => match.slice(0, -2) + extraMissions + "\r\n];"
        );
    }
}

// 3. Add Side Quests System
const sideQuestsCode = `
var SIDE_QUESTS = [
  {
    id: 'quest_airgap',
    title: 'Ghost in the Air-Gap: Steganography Analysis',
    cat: 'FORENSICS',
    difficulty: 'HARD',
    xp: 450,
    credits: 300,
    desc: 'Recover hidden exfiltration payload concealed in air-gapped system memory block 0xDEADBEEF.',
    toolHint: 'Open Files & Storage, inspect corrupted file manifest, and execute Hex Carve with 56 41 55 4c.'
  },
  {
    id: 'quest_crypto',
    title: 'Crypto-Heist Intercept: Rogue Token Revocation',
    cat: 'ZERO_TRUST',
    difficulty: 'MEDIUM',
    xp: 350,
    credits: 200,
    desc: 'Audit live capability tokens in Aegis Guard and purge unauthorized privilege grants.',
    toolHint: 'Open Aegis Shield, click Token Audit, and revoke anomalous bearer token.'
  },
  {
    id: 'quest_dns',
    title: 'DNS Poisoning Triage: Cache Flush & Sinkhole',
    cat: 'NETWORK',
    difficulty: 'MEDIUM',
    xp: 300,
    credits: 250,
    desc: 'Detect tainted resolver records on port 53, flush the resolver cache, and deploy a sinkhole rule.',
    toolHint: 'Open CyberSec Arsenal, inspect DNS packets, and configure Aegis Shield drop rule.'
  },
  {
    id: 'quest_insider',
    title: 'Insider Threat Audit: Midnight Escalation Hunt',
    cat: 'AUDIT',
    difficulty: 'HARD',
    xp: 500,
    credits: 350,
    desc: 'Correlate AegisDB user audit logs to expose anomalous credential escalation occurring at 03:00 UTC.',
    toolHint: 'Open AegisDB Studio, run query SELECT * FROM user_audit_logs, and inspect results.'
  },
  {
    id: 'quest_rootkit',
    title: 'Firmware Rootkit Purge: Merkle Root Restoration',
    cat: 'KERNEL',
    difficulty: 'EXTREME',
    xp: 600,
    credits: 400,
    desc: 'Perform microkernel ASLR diagnostic and execute sub-10s Merkle atomic rollback on corrupted blocks.',
    toolHint: 'Open Merkle Filesystem and execute Self-Healing Rollback Checkpoint.'
  }
];

function renderSideQuestsView() {
  state.sideQuests = state.sideQuests || {};
  var topBox = el('div', 'aegis-achievement-summary');
  var completedCount = Object.keys(state.sideQuests).filter(k => state.sideQuests[k]).length;
  topBox.append(
    el('h3', '', '🎯 SPECIALIZED SIDE QUESTS & BOUNTIES // ' + completedCount + ' / ' + SIDE_QUESTS.length + ' COMPLETED'),
    el('p', '', 'Undertake real-time tactical side operations to earn bonus XP and Command Credits. Complete forensic tasks and claim rewards directly into your operator profile and AegisDB.')
  );
  body.append(topBox);

  var grid = el('div', 'aegis-sidequest-grid');
  SIDE_QUESTS.forEach(function(sq) {
    var isDone = !!state.sideQuests[sq.id];
    var card = el('div', 'aegis-sidequest-card' + (isDone ? ' is-completed' : ''));
    var header = el('div', 'sq-header');
    header.append(
      el('span', 'sq-cat-badge sq-cat-' + sq.cat.toLowerCase(), sq.cat),
      el('span', 'sq-diff-badge', sq.difficulty)
    );
    var title = el('h4', '', sq.title);
    var desc = el('p', '', sq.desc);
    var hint = el('small', 'sq-hint', '💡 Tactical Objective: ' + sq.toolHint);
    
    var footer = el('div', 'sq-footer');
    var reward = el('span', 'sq-reward-pill', '+' + sq.xp + ' XP · +' + sq.credits + ' Credits');
    var claimBtn = el('button', 'btn-action-small ' + (isDone ? 'btn-claimed' : 'btn-claim'), isDone ? '✓ BOUNTY CLAIMED' : 'CLAIM BOUNTY ⚡');
    claimBtn.disabled = isDone;

    claimBtn.addEventListener('click', async function() {
      if (isDone) return;
      try {
        const res = await fetch('/api/db/side-quest/claim', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ questId: sq.id })
        });
        const data = await res.json();
        if (data.success) {
          state.sideQuests[sq.id] = true;
          state.stats.xp = (state.stats.xp || 0) + sq.xp;
          state.stats.credits = (state.stats.credits || 0) + sq.credits;
          save();
          playSfx('milestone');
          toast('🎯 BOUNTY SECURED!', data.message || ('+' + sq.xp + ' XP and +' + sq.credits + ' Credits awarded.'), 'S');
          showAiDialogue('BOUNTY COMMENDATION // ' + sq.title, 'Outstanding investigation, Operator! You successfully resolved the side objective: ' + sq.title + '. Your operator profile has been credited in AegisDB.', [
            { q: 'View updated operator dossier', a: 'Dossier and database records updated.' },
            { q: 'Proceed to next operation', a: 'Good hunting!' }
          ]);
          render();
        }
      } catch (err) {
        toast('CLAIM ERROR', err.message, 'D');
      }
    });

    footer.append(reward, claimBtn);
    card.append(header, title, desc, hint, footer);
    grid.append(card);
  });
  body.append(grid);
}
`;

if (!code.includes('var SIDE_QUESTS = [')) {
    code = code.replace(
        'function renderDrillsView(){',
        sideQuestsCode + '\r\nfunction renderDrillsView(){'
    );
}

// 4. Update tab navigation to include SIDE QUESTS
if (!code.includes("data-tab='sidequests'")) {
    code = code.replace(
        "var bCase=el('button','aegis-dossier-tab active','📁 CASES');",
        "var bCase=el('button','aegis-dossier-tab active','📁 CASES');\r\n  var bQuest=el('button','aegis-dossier-tab','🎯 SIDE QUESTS');"
    );
    code = code.replace(
        "bCase.addEventListener('click',function(){currentTab='cases';render();});",
        "bCase.addEventListener('click',function(){currentTab='cases';render();});\r\n  bQuest.addEventListener('click',function(){currentTab='sidequests';render();});"
    );
    code = code.replace(
        "tabNav.append(bCase,bAch,bDrill);",
        "tabNav.append(bCase,bQuest,bAch,bDrill);"
    );
}

// 5. Update updateTabNav for 4 tabs
const oldUpdateTab = `function updateTabNav(){
  if(!tabNav)return;
  var tabs=tabNav.querySelectorAll('.aegis-dossier-tab');
  if(tabs.length===3){
    tabs[0].className='aegis-dossier-tab'+(currentTab==='cases'?' active':'');
    tabs[1].className='aegis-dossier-tab'+(currentTab==='achievements'?' active':'');
    tabs[1].textContent='🎖️ MEDALS ('+(state.achievements||[]).length+'/'+ACHIEVEMENTS.length+')';
    tabs[2].className='aegis-dossier-tab'+(currentTab==='drills'?' active':'');
  }
}`;

const newUpdateTab = `function updateTabNav(){
  if(!tabNav)return;
  var tabs=tabNav.querySelectorAll('.aegis-dossier-tab');
  if(tabs.length>=3){
    tabs[0].className='aegis-dossier-tab'+(currentTab==='cases'?' active':'');
    if(tabs.length===4){
      tabs[1].className='aegis-dossier-tab'+(currentTab==='sidequests'?' active':'');
      tabs[2].className='aegis-dossier-tab'+(currentTab==='achievements'?' active':'');
      tabs[2].textContent='🎖️ MEDALS ('+(state.achievements||[]).length+'/'+ACHIEVEMENTS.length+')';
      tabs[3].className='aegis-dossier-tab'+(currentTab==='drills'?' active':'');
    } else {
      tabs[1].className='aegis-dossier-tab'+(currentTab==='achievements'?' active':'');
      tabs[1].textContent='🎖️ MEDALS ('+(state.achievements||[]).length+'/'+ACHIEVEMENTS.length+')';
      tabs[2].className='aegis-dossier-tab'+(currentTab==='drills'?' active':'');
    }
  }
}`;

code = code.replace(oldUpdateTab.replace(/\n/g, '\r\n'), newUpdateTab.replace(/\n/g, '\r\n'));

// 6. Update render() to support sidequests
if (!code.includes("if(currentTab==='sidequests'){renderSideQuestsView();return;}")) {
    code = code.replace(
        "if(currentTab==='achievements'){renderAchievementsView();return;}",
        "if(currentTab==='sidequests'){renderSideQuestsView();return;}\r\n if(currentTab==='achievements'){renderAchievementsView();return;}"
    );
}

// 7. Update header kicker from OF 3 to OF missions.length
code = code.replace(
    "head.append(el('span','mission-kicker','AEGIS COMMAND // CASE #0'+(state.index+1)+' OF 3'),el('h2','',m.title));",
    "head.append(el('span','mission-kicker','AEGIS COMMAND // CASE #0'+(state.index+1)+' OF '+missions.length),el('h2','',m.title));"
);

// 8. Add CSS styles for side quests
const sideQuestCss = `
.aegis-sidequest-grid {
  display: flex;
  flex-direction: column;
  gap: 10px;
  margin-top: 10px;
}
.aegis-sidequest-card {
  background: rgba(14, 24, 38, 0.85);
  border: 1px solid rgba(0, 243, 255, 0.2);
  border-radius: 6px;
  padding: 12px 14px;
  transition: all 0.2s ease;
}
.aegis-sidequest-card:hover {
  border-color: #00f3ff;
  box-shadow: 0 0 12px rgba(0, 243, 255, 0.15);
}
.aegis-sidequest-card.is-completed {
  border-color: rgba(0, 255, 128, 0.35);
  background: rgba(8, 20, 16, 0.85);
}
.sq-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 6px;
}
.sq-cat-badge {
  font-size: 9px;
  font-family: var(--font-mono, monospace);
  padding: 2px 6px;
  border-radius: 3px;
  font-weight: 700;
  text-transform: uppercase;
}
.sq-cat-forensics { background: rgba(0, 243, 255, 0.15); color: #00f3ff; border: 1px solid rgba(0, 243, 255, 0.3); }
.sq-cat-zero_trust { background: rgba(157, 0, 255, 0.15); color: #bf55ec; border: 1px solid rgba(157, 0, 255, 0.3); }
.sq-cat-network { background: rgba(0, 180, 255, 0.15); color: #00b4ff; border: 1px solid rgba(0, 180, 255, 0.3); }
.sq-cat-audit { background: rgba(255, 165, 0, 0.15); color: #ffa500; border: 1px solid rgba(255, 165, 0, 0.3); }
.sq-cat-kernel { background: rgba(255, 50, 80, 0.15); color: #ff3250; border: 1px solid rgba(255, 50, 80, 0.3); }
.sq-diff-badge {
  font-size: 9px;
  font-family: var(--font-mono, monospace);
  color: var(--text-muted);
}
.aegis-sidequest-card h4 {
  margin: 0 0 4px 0;
  font-size: 13px;
  color: var(--text-main);
}
.aegis-sidequest-card p {
  margin: 0 0 6px 0;
  font-size: 11px;
  color: var(--text-muted);
  line-height: 1.4;
}
.sq-hint {
  display: block;
  font-size: 10px;
  font-family: var(--font-mono, monospace);
  color: #00f3ff;
  margin-bottom: 8px;
}
.sq-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.sq-reward-pill {
  font-size: 10px;
  font-family: var(--font-mono, monospace);
  color: #00ff80;
  background: rgba(0, 255, 128, 0.08);
  padding: 3px 8px;
  border-radius: 3px;
}
.btn-claim {
  color: #00f3ff;
  border-color: #00f3ff;
}
.btn-claimed {
  color: #00ff80;
  border-color: #00ff80;
  opacity: 0.7;
}
`;

fs.writeFileSync(path.resolve('simulator/web/native-campaign.css'), fs.readFileSync(path.resolve('simulator/web/native-campaign.css'), 'utf8') + '\n' + sideQuestCss);

fs.writeFileSync(file, code, 'utf8');
console.log('Successfully updated native-campaign.js with operations 4-6, side quests, and real-time timed attacks!');

