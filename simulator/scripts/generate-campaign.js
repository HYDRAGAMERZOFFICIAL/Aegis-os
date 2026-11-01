'use strict';
const fs = require('fs');
const path = require('path');

const targetPath = path.resolve('simulator/web/native-campaign.js');

const code = `/* Native AegisOS campaign: event-driven missions embedded in the original desktop. */
(function(){
'use strict';
var KEY='aegis.nativeCampaign.v1';
var soundOn = localStorage.getItem('aegis.sfx.enabled') !== 'false';
var audioCtx = null;

function playSfx(type){
  if(!soundOn)return;
  try{
    var Ctx=window.AudioContext||window.webkitAudioContext;
    if(!Ctx)return;
    if(!audioCtx)audioCtx=new Ctx();
    if(audioCtx.state==='suspended')audioCtx.resume();
    var t=audioCtx.currentTime;
    var osc=audioCtx.createOscillator();
    var gain=audioCtx.createGain();
    osc.connect(gain);
    gain.connect(audioCtx.destination);
    if(type==='clue'){
      osc.type='sine';
      osc.frequency.setValueAtTime(880,t);
      osc.frequency.exponentialRampToValueAtTime(1320,t+0.08);
      gain.gain.setValueAtTime(0.12,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.09);
      osc.start(t);osc.stop(t+0.09);
    }else if(type==='milestone'){
      osc.type='triangle';
      osc.frequency.setValueAtTime(523.25,t);
      osc.frequency.setValueAtTime(659.25,t+0.08);
      osc.frequency.setValueAtTime(783.99,t+0.16);
      gain.gain.setValueAtTime(0.18,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.35);
      osc.start(t);osc.stop(t+0.35);
    }else if(type==='decision'){
      osc.type='sawtooth';
      osc.frequency.setValueAtTime(220,t);
      osc.frequency.exponentialRampToValueAtTime(440,t+0.12);
      gain.gain.setValueAtTime(0.14,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.2);
      osc.start(t);osc.stop(t+0.2);
    }else if(type==='alert'){
      osc.type='square';
      osc.frequency.setValueAtTime(440,t);
      osc.frequency.setValueAtTime(880,t+0.12);
      gain.gain.setValueAtTime(0.09,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.25);
      osc.start(t);osc.stop(t+0.25);
    }else if(type==='victory'){
      osc.type='sine';
      osc.frequency.setValueAtTime(523.25,t);
      osc.frequency.setValueAtTime(659.25,t+0.09);
      osc.frequency.setValueAtTime(783.99,t+0.18);
      osc.frequency.setValueAtTime(1046.50,t+0.28);
      gain.gain.setValueAtTime(0.2,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.55);
      osc.start(t);osc.stop(t+0.55);
    }else if(type==='emergency'){
      osc.type='sawtooth';
      osc.frequency.setValueAtTime(650,t);
      osc.frequency.linearRampToValueAtTime(880,t+0.15);
      osc.frequency.linearRampToValueAtTime(650,t+0.3);
      gain.gain.setValueAtTime(0.15,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.32);
      osc.start(t);osc.stop(t+0.32);
    }else if(type==='achievement'){
      osc.type='sine';
      osc.frequency.setValueAtTime(523.25,t);
      osc.frequency.setValueAtTime(659.25,t+0.07);
      osc.frequency.setValueAtTime(783.99,t+0.14);
      osc.frequency.setValueAtTime(1046.50,t+0.21);
      osc.frequency.setValueAtTime(1318.51,t+0.30);
      gain.gain.setValueAtTime(0.22,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.65);
      osc.start(t);osc.stop(t+0.65);
    }
  }catch(e){}
}

var CASE_DIALOGUES = {
  op1: {
    start: {
      text: "🚨 SECURITY INCIDENT ALERT // CASE #01 ASSIGNED: At 02:13, an unauthorized service restart triggered low-volume encrypted egress on research subnet 4. The initial alert may be a decoy—do not shut down the host blindly. Follow each stage of the objective to uncover the root cause. Good luck, Investigator!",
      options: [
        { q: "Where should I look first?", a: "In Neural Monitor, expand the process tree. Look for a worker whose parent PID does not match its declared service role. Click process details to complete the stage!" },
        { q: "Can I use Terminal commands?", a: "Yes! Running \`ps\` and \`netstat\` in Terminal will register forensic stages across all objectives." },
        { q: "Understood, investigating now.", a: "Stay sharp. Report back as soon as you find the masquerading process." }
      ]
    },
    steps: [
      { text: "Milestone 1 Secured! Process baseline mapped. Now interrogate command history in Terminal: run \`ps\` and \`netstat\` to correlate active socket descriptors.", options: [{ q: "What should I look for?", a: "Find foreign TCP connections on high ports originating from the suspicious worker." }] },
      { text: "Milestone 2 Secured! Sockets correlated. Now open CyberSecurity Arsenal and inspect packet telemetry for high-entropy C2 beacons.", options: [{ q: "What confirms a C2 beacon?", a: "Automated beacons fire at consistent intervals to untrusted relay 185.220.101.5:8443!" }] },
      { text: "Milestone 3 Secured! Network evidence confirmed. Now inspect the staged artifact \`svc-sync-cache.bin\` in Files (AegisFS). Maintain chain of custody—do NOT delete it!", options: [{ q: "Why keep the file intact?", a: "Deleting it destroys cryptographic timestamps required for timeline correlation." }] },
      { text: "Milestone 4 Secured! File metadata captured. Consult Aegis Mind AI to correlate the event sequence between file edit, process spawn, and beacon.", options: [{ q: "Synthesize timeline with AI", a: "Sequence confirmed: Staged file edit at 02:13 -> Masquerading process spawn -> Outbound beacon to 185.220.101.5:8443." }] },
      { text: "Milestone 5 Secured! Timeline reconstructed. Decision Gate: open Operations and select your containment strategy.", options: [{ q: "Select containment strategy", a: "Targeted egress block preserves trusted services while neutralizing the attack path." }] },
      { text: "Milestone 6 Secured! Containment strategy decided. Open Aegis Shield and activate the deny policy blocking hostile relay 185.220.101.5.", options: [{ q: "Which rule to toggle?", a: "Toggle Rule #7 (Untrusted Ingress / C2 Deny policy) to sever the boundary." }] },
      { text: "Milestone 7 Secured! Boundary policy enforced. Run an Aegis Guard scan in Shield, then issue \`verify-containment\` in Terminal to verify socket termination.", options: [{ q: "Verifying containment", a: "Confirm zero active packets are reaching the external C2 relay." }] },
      { text: "Milestone 8 Secured! Containment verified. Open Merkle Filesystem and run an integrity scan to identify the diverged block branch.", options: [{ q: "Locate diverged block", a: "Merkle scan pinpoints cryptographic divergence on leaf block 4." }] },
      { text: "Milestone 9 Secured! Divergence mapped. Execute atomic rollback to clean checkpoint \`chkpt-0003\` and verify root hash integrity with \`verify-root\`.", options: [{ q: "Execute rollback", a: "Atomic sub-10s rollback executed. Merkle root hash verified clean." }] },
      { text: "Milestone 10 Secured! Cryptographic root verified! All investigation milestones complete. Submit your Incident Report in the Operations panel!", options: [{ q: "Submit report to Command", a: "Report sealed with Grade S! Commendation awarded." }] }
    ]
  },
  op2: {
    start: {
      text: "🚨 SECURITY INCIDENT ALERT // CASE #02 ASSIGNED: A signed system component (\`kernel_sync_worker\`) requested privileged capability \`CAP_NET_RAW_SOCKET\` outside its authorized profile. The signature is valid, but the behavior is anomalous! Map the privilege boundary in Neural Monitor. Good luck, Investigator!",
      options: [
        { q: "What should I look for in Neural Monitor?", a: "Inspect the memory map and address space events. Look for an unexpected ring-0 privilege transition!" },
        { q: "Is a valid signature trustworthy?", a: "No! Adversaries can hijack or repurpose signed binaries. Behavior must strictly adhere to least-privilege policy." },
        { q: "Investigating the privilege boundary now.", a: "Be precise: identify the capability requested before modifying authority." }
      ]
    },
    steps: [
      { text: "Milestone 1 Secured! Memory boundary mapped. Open Aegis Shield and inspect the Zero-Trust token table (\`shield-token-audit\`) for the anomalous grant.", options: [{ q: "Which token is anomalous?", a: "Look for \`TOK-KERNEL-ESCALATED\` held by \`kernel_sync_worker\`." }] },
      { text: "Milestone 2 Secured! Token audited. Switch to Terminal and run \`capability-audit\` to trace the requesting component.", options: [{ q: "Running capability-audit", a: "Review the grant matrix and note the anomalous raw socket capability." }] },
      { text: "Milestone 3 Secured! Component traced. Inspect binary library provenance in Files (\`libaegis_kernel.so\`) to prove code signature authenticity.", options: [{ q: "Verify library metadata", a: "Signature is authentic, proving runtime token escalation rather than binary tampering." }] },
      { text: "Milestone 4 Secured! Provenance verified. Run a Merkle integrity scan to check kernel policy baseline consistency.", options: [{ q: "Run Merkle scan", a: "Core filesystem is healthy; the issue is isolated to capability configuration." }] },
      { text: "Milestone 5 Secured! Baseline verified. Consult Aegis Mind AI to correlate the token, process, and integrity evidence.", options: [{ q: "Synthesize root cause", a: "Correlated: Component requested unneeded raw socket token while binary signature remained valid." }] },
      { text: "Milestone 6 Secured! Decision Gate: Select the minimum safe authority change in Operations. Revoke only the anomalous token!", options: [{ q: "Locking in least privilege", a: "Revoking only anomalous token preserves trusted system logging and compositing." }] },
      { text: "Milestone 7 Secured! Command decision recorded. Run CyberSec vulnerability scan to verify approved patch \`AEGIS-KERNEL-2.0.4\`.", options: [{ q: "Verify patch posture", a: "Patch build AEGIS-KERNEL-2.0.4 verified present in kernel repository." }] },
      { text: "Milestone 8 Secured! Patch posture verified. Run an Aegis Guard scan in Shield, then issue \`quarantine-component\` in Terminal.", options: [{ q: "Quarantine component", a: "Rogue worker component quarantined to secure enclave." }] },
      { text: "Milestone 9 Secured! Component quarantined. Run Merkle integrity verification and execute \`verify-root\` in Terminal.", options: [{ q: "Verify kernel root", a: "Cryptographic root hash matches clean trusted checkpoint." }] },
      { text: "Milestone 10 Secured! Kernel root certified! Submit your final Incident Analysis in the Operations panel.", options: [{ q: "Submit kernel analysis", a: "Excellent work! Least-privilege response verified." }] }
    ]
  },
  op3: {
    start: {
      text: "🚨 SECURITY INCIDENT ALERT // CASE #03 ASSIGNED: The public gateway is degrading under a traffic surge. Determine whether traffic is uniform, distributed, or selectively shaped, and preserve priority edge routes. Good luck, Investigator!",
      options: [
        { q: "Where should I start?", a: "Open Task Optimizer to audit CPU, memory, and network queue depth." },
        { q: "How to avoid locking out legitimate users?", a: "Use adaptive rate shaping and reroute critical traffic to resilient edge-route-02." }
      ]
    },
    steps: [
      { text: "Milestone 1 Secured! Resource impact established. Inspect packet distribution in CyberSec Arsenal.", options: [{ q: "Inspect packet timing", a: "Distributed sources targeting gateway on port 80." }] },
      { text: "Milestone 2 Secured! Distribution mapped. Use Terminal to run \`netstat\` and \`service-health\` to inspect degraded routes.", options: [{ q: "Check service health", a: "Gateway edge-route-01 is degraded; edge-route-02 is healthy." }] },
      { text: "Milestone 3 Secured! Dependencies mapped. Ask Aegis Mind AI to compare trusted client patterns with anomalous traffic.", options: [{ q: "Correlate traffic patterns", a: "Legitimate traffic uses predictable keep-alive intervals; flood traffic lacks session state." }] },
      { text: "Milestone 4 Secured! Traffic signatures identified. Inspect edge policy in Aegis Shield.", options: [{ q: "Inspect edge rules", a: "Review ingress rate policies and queue limits." }] },
      { text: "Milestone 5 Secured! Decision Gate: Select mitigation profile in Operations: Adaptive ingress shaping.", options: [{ q: "Commit mitigation", a: "Adaptive ingress shaping preserves priority paths." }] },
      { text: "Milestone 6 Secured! Mitigation profile selected. Apply rate controls in Shield and observe packet flow in CyberSec Arsenal.", options: [{ q: "Observe rate controls", a: "Queue depth stabilizing; packet drop rate within acceptable parameters." }] },
      { text: "Milestone 7 Secured! Rate controls applied. Use Terminal to inspect route health and execute \`route-select edge-route-02\`.", options: [{ q: "Select priority route", a: "Critical service traffic redirected to resilient edge-route-02." }] },
      { text: "Milestone 8 Secured! Route switched. Verify service recovery in Terminal (\`service-health\`) and Task Optimizer.", options: [{ q: "Verify recovery", a: "Gateway health restored to HEALTHY; queue latency normalized." }] },
      { text: "Milestone 9 Secured! Gateway restored. Perform final security audit in Shield and Merkle engine.", options: [{ q: "Audit final state", a: "Boundary policy validated; Merkle root unchanged." }] },
      { text: "Milestone 10 Secured! Recovery complete. Submit your recovery post-mortem dossier in Operations!", options: [{ q: "Submit post-mortem", a: "Outstanding! Critical service preserved with zero data loss." }] }
    ]
  },
  op7: {
    start: {
      text: "🚨 SECURITY INCIDENT ALERT // CASE #07 ASSIGNED: Hostile APT-28 hijacked an orbital satellite ground transceiver. Unauthenticated downlink telemetry and forged firmware are flooding subnet 5! Audit packet streams and carve the satellite microcode. Good luck, Operator!",
      options: [
        { q: "What frequency/port is affected?", a: "Inspect CyberSec Arsenal packet streams on port 9100 to intercept carrier frames." },
        { q: "How to recover the corrupted firmware?", a: "Open Files & Storage, locate satcom_fw.bin.corrupt, and carve magic bytes 53 41 54 43." }
      ]
    },
    steps: [
      { text: "Milestone 1 Secured! Ground telemetry downlink intercepted on port 9100. Open Neural Monitor to audit unverified ground station worker daemons.", options: [{ q: "Audit worker daemons", a: "Look for rogue process satcom_relay holding raw socket capabilities." }] },
      { text: "Milestone 2 Secured! Rogue worker identified. Open Terminal and run \`capability-audit\` and \`netstat\` to verify socket listeners.", options: [{ q: "Audit transceiver sockets", a: "Confirm foreign ground listener on port 9100 holding unverified tokens." }] },
      { text: "Milestone 3 Secured! Socket verified. Open Files & Storage and deploy Hex Carver on \`satcom_fw.bin.corrupt\` with magic bytes \`53 41 54 43\`.", options: [{ q: "Carve satellite firmware", a: "Valid magic bytes 53 41 54 43 (SATC) restore clean orbital microcode." }] },
      { text: "Milestone 4 Secured! Firmware carved and restored. Consult Aegis Mind AI to correlate the orbital attack telemetry.", options: [{ q: "Correlate orbital telemetry", a: "AI confirms adversary attempted to push unauthenticated microcode to satellite node 4." }] },
      { text: "Milestone 5 Secured! Timeline verified. Decision Gate: Commit your satcom containment strategy in Operations.", options: [{ q: "Select containment", a: "Targeted ground transceiver lock preserves earth observation feeds while severing C2." }] },
      { text: "Milestone 6 Secured! Strategy selected. Open Aegis Shield and revoke raw RF socket capability held by the rogue worker.", options: [{ q: "Revoke transceiver token", a: "Enforce boundary rule blocking foreign satellite uplink." }] },
      { text: "Milestone 7 Secured! Capability revoked. Run Aegis Guard scan in Shield, then issue \`verify-containment\` in Terminal.", options: [{ q: "Verify uplink containment", a: "Uplink severed: zero unauthorized carrier packets reaching orbital repeater." }] },
      { text: "Milestone 8 Secured! Containment verified. Open Merkle Filesystem and execute atomic rollback on the satcom telemetry block.", options: [{ q: "Execute Merkle rollback", a: "Telemetry block restored to clean baseline checkpoint." }] },
      { text: "Milestone 9 Secured! Block restored. Run \`verify-root\` in Terminal to certify hardware root-of-trust.", options: [{ q: "Verify root hash", a: "Satellite cryptographic root hash verified clean." }] },
      { text: "Milestone 10 Secured! Orbital uplink secured! Submit your Satcom Incident Post-Mortem in the Operations panel.", options: [{ q: "Submit Satcom dossier", a: "Orbital repeater certified safe! Commendation logged." }] }
    ]
  },
  op8: {
    start: {
      text: "🚨 SECURITY INCIDENT ALERT // CASE #08 ASSIGNED: Adversary APT-29 injected poisoned neural network weights into the biometric facial verification enclave. Deepfake synthetic avatars are generating unauthorized administrative credentials! Audit the enclave in Troubleshoot and Neural Monitor. Good luck, Operator!",
      options: [
        { q: "Where is the enclave located?", a: "Run Troubleshoot memory diagnostic to detect biometric enclave drift." },
        { q: "How to restore clean biometric weights?", a: "Carve biometric_model.weights.corrupt in File Recovery using magic bytes 42 49 4f 53 (BIOS)." }
      ]
    },
    steps: [
      { text: "Milestone 1 Secured! Memory enclave drift detected. Open Neural Monitor to inspect the biometric model memory arena.", options: [{ q: "Audit neural memory arena", a: "Identify anomalous tensors allocated outside declared sandbox boundary." }] },
      { text: "Milestone 2 Secured! Poisoned weights located. Open Aegis Shield and inspect the Zero-Trust token table for elevated biometric grants.", options: [{ q: "Inspect biometric tokens", a: "Locate unauthorized capability token TOK-BIOMETRIC-ROOT." }] },
      { text: "Milestone 3 Secured! Rogue token identified. Open Files & Storage and deploy Hex Carver on \`biometric_model.weights.corrupt\` with magic bytes \`42 49 4f 53\`.", options: [{ q: "Carve clean neural weights", a: "Magic bytes 42 49 4f 53 restore clean facial inference tensor." }] },
      { text: "Milestone 4 Secured! Model weights restored. Consult Aegis Mind AI to correlate the deepfake injection vector.", options: [{ q: "Correlate deepfake root cause", a: "Root cause: Poisoned training tensor injected into administrative verification pipeline." }] },
      { text: "Milestone 5 Secured! Root cause confirmed. Decision Gate: Commit your biometric remediation strategy in Operations.", options: [{ q: "Select remediation", a: "Biometric enclave isolation & weight restore preserves security with zero downtime." }] },
      { text: "Milestone 6 Secured! Strategy selected. Open Aegis Shield and revoke the rogue neural capability token.", options: [{ q: "Revoke rogue token", a: "Unauthorized administrative minting blocked." }] },
      { text: "Milestone 7 Secured! Token revoked. Run Aegis Guard scan in Shield, then issue \`verify-containment\` in Terminal.", options: [{ q: "Verify enclave containment", a: "Deepfake injection vector severed." }] },
      { text: "Milestone 8 Secured! Containment verified. Open Merkle Filesystem and run an integrity scan to verify enclave hashes.", options: [{ q: "Run Merkle integrity scan", a: "Biometric enclave block verified clean." }] },
      { text: "Milestone 9 Secured! Enclave verified. Run \`verify-root\` in Terminal to seal the cryptographic baseline.", options: [{ q: "Verify root of trust", a: "Cryptographic Merkle root hash sealed and verified." }] },
      { text: "Milestone 10 Secured! Biometric Citadel secured! Submit your Biometric Defense Post-Mortem in Operations.", options: [{ q: "Submit Biometric dossier", a: "Grandmaster Operator certification awarded!" }] }
    ]
  }
};

var ACHIEVEMENTS = [
  { id:'first_breach', title:'First Line of Defense', icon:'🛡️', desc:'Contain your first security breach and record forensic proof.', xp:100, credits:150 },
  { id:'timed_hero', title:'Crisis Responder', icon:'⏱️', desc:'Neutralize an active time-critical attack before system expiration!', xp:250, credits:300 },
  { id:'rapid_triage', title:'Sub-60s SecOps', icon:'⚡', desc:'Neutralize an active security breach in under 60 seconds.', xp:200, credits:200 },
  { id:'zero_trust_elite', title:'Zero-Trust Sentinel', icon:'🔑', desc:'Revoke an unauthorized capability token without disrupting services.', xp:150, credits:200 },
  { id:'merkle_savior', title:'Merkle Restorer', icon:'🌲', desc:'Perform atomic Merkle rollback and verify root hash integrity.', xp:150, credits:200 },
  { id:'packet_hunter', title:'Traffic Forensics', icon:'🛰️', desc:'Correlate external hostile C2 beacon relay from packet telemetry.', xp:150, credits:200 },
  { id:'forensic_genius', title:'Forensic Deduction Master', icon:'🧠', desc:'Submit a case incident report with 100% correct deduction answers.', xp:200, credits:250 },
  { id:'terminal_warrior', title:'Ring-0 Operator', icon:'💻', desc:'Execute at least 8 distinct forensic and defense commands in Terminal.', xp:100, credits:150 },
  { id:'ghost_protocol', title:'Ephemeral Airgap', icon:'👻', desc:'Browse securely in Quantum Ghost Browser and shred the container on exit.', xp:100, credits:150 },
  { id:'crypt_inode', title:'Cryptographic Custody', icon:'🔐', desc:'Create, edit, or AES-256 encrypt a critical evidence document in AegisFS.', xp:100, credits:150 },
  { id:'director_commendation', title:'Grade S Commander', icon:'🎖️', desc:'Secure any operational case with the highest possible Grade S rank.', xp:300, credits:500 },
  { id:'citadel_sealed', title:'Cyber Fortress Vanguard', icon:'🏆', desc:'Complete operational cases and repel advanced persistent threats.', xp:500, credits:1000 }
];

var TIMED_SCENARIOS = [
  {
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
    brief: 'Malicious process injecting corrupted payloads into user_case_files! Revoke rogue capability token in Shield or run capability-audit in Terminal!',
    defusalActions: ['click:shield-token-audit', 'terminal:capability-audit'],
    promptClue: 'Inspect Shield Zero-Trust Tokens or issue capability-audit in Terminal!'
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
  {
    id: 'emg-worm',
    title: 'CRITICAL ZERO-DAY RANSOMWARE EXFILTRATION',
    threatType: 'Ransomware Worm Payload',
    seconds: 75,
    brief: 'Ransomware worm payload has infiltrated research subnet 4! Neutralize the hostile process or sever ingress before the clock runs out!',
    defusalActions: ['change:shield-rule', 'click:shield-guard-scan', 'terminal:verify-containment', 'terminal:quarantine-component'],
    promptClue: 'Enforce Shield firewall deny rule, run Guard scan, or issue quarantine-component in Terminal!'
  },
  {
    id: 'emg-ddos',
    title: 'VOLUMETRIC SYN FLOOD SATURATION',
    threatType: 'Asymmetric Edge Flood',
    seconds: 60,
    brief: 'Volumetric packet flood hitting edge-route-01! Deploy adaptive rate shaping in Shield or issue route-select edge-route-02 before gateway drops!',
    defusalActions: ['change:shield-rate-policy', 'terminal:route-select'],
    promptClue: 'Toggle adaptive rate shaping in Shield or issue route-select edge-route-02 in Terminal!'
  },
  {
    id: 'emg-merkle',
    title: 'STEALTH ROOTKIT CORRUPTION ATTACK',
    threatType: 'Kernel Inode Tampering',
    seconds: 80,
    brief: 'Adversary injected corrupted inode into root filesystem block 4! Execute atomic Merkle rollback to clean restore point before system integrity collapses!',
    defusalActions: ['click:merkle-rollback', 'terminal:verify-root'],
    promptClue: 'Open Merkle Filesystem and click Rollback Checkpoint, or run verify-root in Terminal!'
  }
];

var CLUE_CARDS = {
  op1: [
    { id: 'clue_op1_01', stepIndex: 0, title: 'Anomalous Parent-Child Ancestry', cat: 'PROCESS_TREE', badge: 'PID 1337', ioc: 'Parent PID 1 (Init) vs svc_sync_worker', hexSnippet: '0x004018A0: CALL ring3_trampoline', hint: 'Neural Monitor identifies that svc_sync_worker was spawned without declared init daemon parameters.', guide: 'Open Neural Monitor, click on process PID 1337 to inspect process ancestry and observe unauthorized child threads.', app: 'neural' },
    { id: 'clue_op1_02', stepIndex: 1, title: 'Socket Listener NetFlow Beacon', cat: 'NETWORK_NETSTAT', badge: '185.220.101.5:8443', ioc: 'TCP SYN Outbound -> 185.220.101.5:8443', hexSnippet: 'ESTABLISHED -> 185.220.101.5:8443 (TLS 1.3)', hint: 'Terminal netstat command reveals persistent outbound socket connection to known Spectre C2 relay.', guide: 'Open Terminal and execute \`netstat\` and \`ps\`. Compare active socket descriptors with process PID 1337.', app: 'terminal' },
    { id: 'clue_op1_03', stepIndex: 2, title: 'High-Entropy Encrypted C2 Packets', cat: 'DPI_SNIFFER', badge: 'PORT 8443', ioc: 'Packet Entropy: 7.94 / 8.00 (Encrypted C2 Beacon)', hexSnippet: '17 03 03 00 28 88 f9 10 4c 88 e9 12 40 18 b8 71', hint: 'CyberSec Arsenal Deep Packet Inspector confirms repeated 60-second periodic beacon transmissions.', guide: 'Open CyberSec Arsenal, select Packet Sniffer, and click the suspicious stream destined for 185.220.101.5:8443.', app: 'cybersec' },
    { id: 'clue_op1_04', stepIndex: 3, title: 'Staged Payload Artifact', cat: 'FILESYSTEM_IOC', badge: 'svc-sync-cache.bin', ioc: '/userland/data/svc-sync-cache.bin (Modified: 02:13)', hexSnippet: 'SHA-256: 4a8e9b1f2c3d4e5f6a7b8c9d0e1f2a3b4c5d6e7f8a9b0c1d2e3f4a5b6c7d8e9f', hint: 'Files app reveals binary artifact dropped right before the unauthorized service restart.', guide: 'Open Files & Storage Manager, click svc-sync-cache.bin to inspect file metadata and cryptographic hash.', app: 'files' }
  ]
};

var STAGE_GUIDES = {
  op1: [
    { target: 'PID 1337 (svc_sync_worker) | Spoofed Parent', p1: 'Stage 1: Launch Neural Monitor or Troubleshoot from dock.', p2: 'Stage 2: Click process PID 1337 or execute ps in Terminal to reveal spoofed parent PPID 1.', p3: 'Stage 3: Run netstat or click CyberSec Arsenal to correlate outbound socket descriptors.', bg: 'Adversaries use process masquerading (MITRE T1036) to blend malicious workers into legitimate daemon trees.' },
    { target: 'Socket TCP 185.220.101.5:8443 | High Port Listener', p1: 'Stage 1: Launch Terminal (aegis_shell) from dock or desktop.', p2: 'Stage 2: Execute ps to audit running process arguments.', p3: 'Stage 3: Execute netstat to correlate process PID 1337 with active connection to 185.220.101.5:8443.', bg: 'Network connection enumeration (MITRE T1049) reveals active command-and-control channels.' },
    { target: 'Deep Packet Stream 185.220.101.5:8443 | High Shannon Entropy', p1: 'Stage 1: Open CyberSecurity Arsenal from dock.', p2: 'Stage 2: Click suspicious packet row destined for 185.220.101.5:8443 in Packet Sniffer.', p3: 'Stage 3: Correlate beacon timing with active Shield boundary telemetry.', bg: 'Shannon entropy analysis detects encrypted covert channel framing (MITRE T1573).' },
    { target: 'File /userland/data/svc-sync-cache.bin | Staged Payload', p1: 'Stage 1: Open Files & Storage Manager (AegisFS) from dock.', p2: 'Stage 2: Click svc-sync-cache.bin in the file list to inspect metadata and SHA-256 hash.', p3: 'Stage 3: Cross-check file modification timestamp with 02:13 alert time in Storage.', bg: 'Preserving file integrity metadata (chain of custody) is critical before containment.' },
    { target: 'Timeline Synthesis: File Edit -> Process Spawn -> Beacon', p1: 'Stage 1: Open Aegis Mind AI Controller from dock or top bar.', p2: 'Stage 2: Prompt Aegis Mind to synthesize the chronological timeline of events.', p3: 'Stage 3: Record the reconstructed sequence into your Case Notes journal.', bg: 'Multivariate timeline reconstruction establishes definitive proof of initial intrusion vector.' },
    { target: 'Decision Gate: Targeted Egress Block', p1: 'Stage 1: Review operational trade-offs in Operations panel.', p2: 'Stage 2: Select Targeted egress block to maintain internal cluster availability.', p3: 'Stage 3: Commit decision and observe readiness score preservation.', bg: 'Least-disruption containment isolates hostile actors without taking down business operations.' },
    { target: 'Shield Firewall Rule #7: Untrusted Ingress / C2 Deny', p1: 'Stage 1: Launch Aegis Shield Firewall from dock.', p2: 'Stage 2: Toggle Rule #7 (Untrusted Ingress / C2 Deny policy) to drop packets to 185.220.101.5.', p3: 'Stage 3: Verify packet drop counter increment in Shield edge audit.', bg: 'Stateful packet filtering enforces boundary defense (MITRE M1037).' },
    { target: 'Aegis Guard Ring-0 Telemetry | Severed Socket Verification', p1: 'Stage 1: Click Run Aegis Guard AI Scan in Aegis Shield.', p2: 'Stage 2: Open Terminal shell.', p3: 'Stage 3: Execute verify-containment to confirm hostile session is severed.', bg: 'Ring-0 telemetry verification proves malicious socket descriptors are purged.' },
    { target: 'Merkle Block #4 | Cryptographic Inode Divergence', p1: 'Stage 1: Open Merkle Filesystem Engine from dock.', p2: 'Stage 2: Click Run Merkle Hash Scan to locate diverged leaf block #4.', p3: 'Stage 3: Execute verify-root in Terminal to inspect the divergent parent hash.', bg: 'Merkle DAG filesystems guarantee cryptographic tamper-evidence.' },
    { target: 'Checkpoint chkpt-0003 | Atomic Snapshot Rollback', p1: 'Stage 1: Select clean checkpoint chkpt-0003 in Merkle Filesystem and trigger Rollback.', p2: 'Stage 2: Execute verify-root in Terminal to confirm cryptographic root hash integrity.', p3: 'Stage 3: Check service-health or Task Optimizer to verify normal system health.', bg: 'Atomic sub-10s snapshot rollback restores corrupted blocks with zero data loss.' }
  ]
};

var missions=[
{
  id:'op1',
  title:'THE SILENT INTRUSION',
  brief:'At 02:13 an unauthorised service restart was followed by a low-volume encrypted session. The first alert may be a decoy. Establish what happened before containment.',
  pressure:'Containment can destroy volatile evidence. Separate the loudest signal from the root cause.',
  steps:[
    ['Establish a process baseline','Inspect Neural Monitor. Compare process ancestry and identify an anomalous parent-child relationship.','Look for a process whose parent does not match its declared service role.',['open:neural|open:troubleshoot','click:neural-process-detail|terminal:ps','terminal:netstat|click:cybersec-packet-inspect']],
    ['Interrogate command history','Use Terminal to inspect process and socket state. Record the suspicious PID and parent.','Run ps and netstat; compare the process tree with active sockets.',['open:terminal','terminal:ps|terminal:ss','terminal:netstat|terminal:journalctl']],
    ['Correlate network telemetry','Inspect packet metadata in CyberSecurity Arsenal. Compare repeated destinations and timing.','One packet is weak evidence. Correlate destination, port and interval.',['open:cybersec','click:cybersec-packet-inspect','terminal:netstat|click:shield-edge-audit']],
    ['Inspect the staged artefact','Inspect the suspicious service file and its metadata in Files. Do not delete it.','Compare modified time, owner, permissions and hash.',['open:files','click:files-inspect-suspicious','open:storage|click:storage-recovery-carve']],
    ['Reconstruct the timeline','Ask Aegis Mind to correlate process, file and network observations.','File modification should precede process spawn, then outbound beacon.',['open:ai','click:ai-correlate-timeline','terminal:journalctl|open:notepad']],
    ['Choose containment strategy','Decide whether to block the destination, isolate the host or continue observing.','Consider evidence preservation and blast radius. A broad shutdown harms trusted services.','choice',['Targeted egress block','Isolate the host','Continue observation']],
    ['Apply defensive policy','Use Aegis Shield to implement a narrow policy consistent with your evidence.','Block only the confirmed hostile destination. Preserve unrelated trusted traffic.',['open:shield','change:shield-rule','click:shield-edge-audit|terminal:route-health']],
    ['Verify containment','Run Aegis Guard and confirm the suspicious process is no longer communicating.','Run verify-containment after the Guard scan; the terminal output must confirm the session is gone.',['click:shield-guard-scan','open:terminal','terminal:verify-containment']],
    ['Validate filesystem integrity','Inspect Merkle filesystem and compare the affected branch with a trusted checkpoint.','Identify the changed branch before restoring anything.',['open:merkle','click:merkle-integrity-scan','terminal:verify-root|open:troubleshoot']],
    ['Recover without erasing the trail','Restore the verified checkpoint and verify the root again.','Choose a checkpoint created before the suspicious service modification.',['click:merkle-rollback','terminal:verify-root','terminal:service-health|click:optimizer-resource-audit']],
    ['Submit incident report','Submit the timeline, root cause, containment choice and recovery proof.','All evidence categories and a defensible decision are required.','report']
  ],
  choices:['Targeted egress block','Isolate the host','Continue observation'],
  outcomes:['Evidence-preserving containment. Host services remain available.','Threat contained, but service interruption reduced readiness.','Delayed containment allowed additional simulated exposure.'],
  quiz:[
    {id:'endpoint',question:'Which remote endpoint was contacted by the suspicious worker?',placeholder:'IP:PORT',accept:['185.220.101.5:8443'],hint:'Compare the svc-sync-cache artefact with repeated outbound packet destinations.'},
    {id:'sequence',question:'What is the most likely event sequence?',placeholder:'file > process > beacon',accept:['file>process>beacon','file modification>process spawn>outbound beacon'],hint:'Reconstruct the timeline from modified artefact, process ancestry and outbound beacon.'}
  ]
},
{
  id:'op2',
  title:'GHOST IN THE KERNEL',
  brief:'A signed system component requested a capability it has never needed. The signature is valid; the behaviour is not. Determine whether a trusted process has been repurposed.',
  pressure:'Revoking every privileged token may destabilise the desktop. Identify the exact authority boundary crossed.',
  steps:[
    ['Map the privilege boundary','Inspect Neural Monitor memory and process context. Identify the unexpected ring transition.','Compare declared privilege with the capability actually requested.',['open:neural|open:troubleshoot','click:neural-memory-map|terminal:ps','click:neural-process-detail']],
    ['Audit capability grants','Inspect the Zero-Trust token table in Aegis Shield.','Find a token whose subject and capability do not match its normal role.',['open:shield','click:shield-token-audit','terminal:capability-audit']],
    ['Trace the requesting component','Use Terminal to inspect process ownership and capability references.','Run ps and capability-audit; compare the requesting component with its granted token.',['open:terminal','terminal:capability-audit','terminal:ps|terminal:netstat']],
    ['Verify binary provenance','Inspect the system library and metadata in Files.','A valid signature does not prove the running component is behaving normally.',['open:files','click:files-inspect-library','open:storage|click:files-inspect-suspicious']],
    ['Compare trusted kernel baseline','Run a Merkle integrity scan and identify the branch that differs.','Look for a mismatch limited to component or policy metadata.',['open:merkle','click:merkle-integrity-scan','terminal:verify-root']],
    ['Correlate the root cause','Ask Aegis Mind to correlate token, process and integrity evidence.','The best explanation accounts for all three sources.',['open:ai','click:ai-correlate-kernel','open:notepad|terminal:journalctl']],
    ['Select minimum safe authority change','Choose the narrowest action that stops escalation without disabling security services.','Revoke the anomalous capability, not every token owned by the service.','choice',['Revoke anomalous token','Revoke every service token','Leave privileges unchanged']],
    ['Verify the kernel patch posture','Run the vulnerability scan and confirm the approved training patch is present.','Compare the reported kernel build with the approved training baseline.',['open:cybersec','click:cybersec-vulnerability-scan','terminal:service-health']],
    ['Quarantine anomalous component','Run Aegis Guard and isolate the process after the authority change.','Run quarantine-component after the Guard scan; it is a simulated Aegis training command, not a host command.',['click:shield-guard-scan','open:terminal','terminal:quarantine-component']],
    ['Prove kernel trust','Re-run integrity verification and compare the resulting root.','Run verify-root after the Merkle check; compare the displayed root with the trusted checkpoint.',['click:merkle-integrity-scan','terminal:verify-root','click:optimizer-resource-audit']],
    ['File kernel incident analysis','Submit root cause, affected capability, patch provenance and verification result.','A correct action with an incomplete audit trail earns a lower grade.','report']
  ],
  choices:['Revoke anomalous token','Revoke every service token','Leave privileges unchanged'],
  outcomes:['Least-privilege response accepted. Unrelated services remain operational.','Escalation stopped, but broad revocation disrupted trusted capabilities.','Kernel remains exposed. Command records a serious operational failure.'],
  quiz:[
    {id:'token',question:'Which anomalous token exceeded the component authority profile?',placeholder:'TOK-...',accept:['TOK-KERNEL-ESCALATED'],hint:'Read the capability audit output and compare the anomalous grant.'},
    {id:'component',question:'Which component requested the unexpected capability?',placeholder:'component_name',accept:['kernel_sync_worker'],hint:'Correlate the token audit with the requesting process in the terminal output.'}
  ]
},
{
  id:'op3',
  title:'BLACKOUT PROTOCOL',
  brief:'The public gateway is degrading under a traffic surge. The first dashboard suggests a flood, but priority traffic is also delayed. Determine whether traffic is uniform, distributed or selectively shaped.',
  pressure:'A blunt rate limit can protect the gateway while locking out legitimate users. Preserve the critical path.',
  steps:[
    ['Establish service impact','Inspect Task Optimizer: CPU, memory, queue depth and network utilisation.','High ingress does not automatically mean CPU exhaustion.',['open:optimizer','click:optimizer-resource-audit','terminal:service-health']],
    ['Inspect packet distribution','Compare source diversity, destination ports and packet timing.','A distributed source set with a narrow target differs from a simple port scan.',['open:cybersec','click:cybersec-packet-inspect','terminal:netstat']],
    ['Map service dependencies','Use Terminal to inspect listeners, routes and service health.','Run netstat and service-health; identify the degraded gateway and healthy priority edge.',['open:terminal','terminal:netstat','terminal:service-health']],
    ['Find legitimate traffic signature','Ask Aegis Mind to compare trusted client patterns with anomalous traffic.','Protect the known-good service path before broad controls.',['open:ai','click:ai-correlate-traffic','open:notepad']],
    ['Inspect edge policy','Inspect ingress rules and trusted route health in Aegis Shield.','A valid rule can still be operationally harmful.',['open:shield','click:shield-edge-audit','terminal:route-health']],
    ['Choose mitigation profile','Balance availability, false positives and containment speed.','Adaptive shaping is safer than global deny when priority traffic is preserved.','choice',['Adaptive ingress shaping','Global ingress block','No rate control']],
    ['Apply and observe rate controls','Apply the profile and inspect resulting packet flow.','Do not assume mitigation worked. Observe queue and packet behaviour.',['change:shield-rate-policy','open:cybersec','click:cybersec-packet-inspect']],
    ['Reroute critical service','Use Terminal to inspect route health and select the resilient edge path.','Run route-health, compare edge-01/02/03, then issue route-select edge-route-02 only after checking the priority path.',['open:terminal','terminal:route-health','terminal:route-select']],
    ['Verify service recovery','Use Terminal and Task Optimizer to verify gateway health and resource recovery.','Run service-health again and inspect Task Optimizer resource telemetry after rerouting.',['terminal:service-health','open:optimizer','click:optimizer-resource-audit']],
    ['Run final security audit','Recheck edge policy and confirm no unrelated service was disabled.','The final state should be secure and usable, not merely quiet.',['click:shield-edge-audit','open:merkle','click:merkle-integrity-scan']],
    ['Submit recovery post-mortem','Record analysis, mitigation profile, route choice, service impact and residual risk.','Explain the trade-off and why it was appropriate.','report']
  ],
  choices:['Adaptive ingress shaping','Global ingress block','No rate control'],
  outcomes:['Adaptive shaping preserved the critical path while reducing the flood.','Gateway recovered, but global block caused a severe legitimate-user outage.','Service degradation continued. Availability was not restored in the operation window.'],
  quiz:[
    {id:'degraded',question:'Which edge route was degraded before mitigation?',placeholder:'edge-route-..',accept:['edge-route-01'],hint:'Compare the route-health table before selecting the healthy edge.'},
    {id:'healthy',question:'Which route should carry the critical service after rerouting?',placeholder:'edge-route-..',accept:['edge-route-02'],hint:'The route-health output shows which edge has priority capacity and low queue depth.'}
  ]
},
{
  id:'op4',
  title:'ZERO-DAY RANSOMWARE HEIST',
  brief:'A fast-propagating lateral ransomware worm compromised research partition /userland/data/. Reconstruct wiped cryptographic inode headers with the Hex Carver, trace the encryption key in volatile memory, and revoke the malicious capability token.',
  pressure:'Delay will cause irreversible userland partition loss. Carve the wiped inode header using expected magic bytes.',
  steps:[
    ['Detect ransomware process ancestry','Inspect Neural Monitor. Compare process ancestry and identify the anomalous ransomware dropper thread.','Look for worker threads executing outside assigned memory arenas.',['open:neural','click:neural-process-detail','terminal:ps']],
    ['Triage network beacon in CyberSec Arsenal','Inspect packet telemetry for outbound ransomware encryption key exchange.','Correlate high-entropy outbound payloads on port 8443.',['open:cybersec','click:cybersec-packet-inspect','terminal:netstat']],
    ['Inspect corrupted file inode in Files','Inspect the damaged user_vault.dat in Files & Storage. Note the corrupted header bytes.','Verify file size, permissions, and corrupted inode status.',['open:files','click:files-inspect-suspicious','open:storage']],
    ['Carve wiped inode headers in File Recovery','Open Files & Storage, locate user_vault.dat.corrupt, and deploy Hex Carver with magic bytes 56 41 55 4c.','Carving valid magic bytes restores the cryptographic enclave.',['open:storage','click:storage-recovery-carve','terminal:verify-root']],
    ['Reconstruct timeline with Aegis Mind','Ask Aegis Mind to correlate ransomware dropper spawn, socket beacon, and header wipe.','Dropper spawn preceded header wipe.',['open:ai','click:ai-correlate-timeline','open:notepad']],
    ['Choose containment strategy','Decide whether to revoke elevated tokens, sever edge, or wipe partition.','Preserve forensic trail while stopping cryptographic propagation.','choice',['Emergency capability revocation & hex restore','Full partition wipe','Ignore threat']],
    ['Revoke rogue bearer capability in Shield','Use Aegis Shield to revoke unauthorized raw I/O tokens held by the dropper.','Cut raw disk write access immediately.',['open:shield','change:shield-rule','click:shield-guard-scan']],
    ['Verify zero-trust containment','Run Aegis Guard and verify that ransomware threads are neutralized.','Run verify-containment in Terminal to confirm quarantine.',['click:shield-guard-scan','open:terminal','terminal:verify-containment']],
    ['Commit Merkle snapshot','Validate restored filesystem integrity with Merkle tree scan.','Verify that root hash matches clean baseline.',['open:merkle','click:merkle-integrity-scan','terminal:verify-root']],
    ['Submit incident report','Submit root cause, carved magic bytes, and proof of containment.','All investigative evidence and defensible choice required.','report']
  ],
  choices:['Emergency capability revocation & hex restore','Full partition wipe','Ignore threat'],
  outcomes:['Ransomware neutralized and inodes restored. Zero data loss recorded.','Partition wiped, but valuable research archives destroyed.','Ransomware propagated across all nodes. Critical failure.'],
  quiz:[
    {id:'magic',question:'What is the expected 4-byte magic header for user_vault.dat?',placeholder:'HEX BYTES',accept:['56 41 55 4c','5641554c','VAUL'],hint:'Inspect the Hex Carver recovery hint for user_vault.dat.corrupt.'},
    {id:'capability',question:'Which privileged capability was abused by the ransomware dropper?',placeholder:'CAP_...',accept:['CAP_SYS_RAWIO','CAP_NET_RAW_SOCKET','CAP_RAWIO'],hint:'Check capability-audit in Terminal or Shield token registry.'}
  ]
},
{
  id:'op5',
  title:'QUANTUM CIPHER EXFILTRATION',
  brief:'Adversary APT-41 is attempting covert asymmetric exfiltration via steganographic DNS-SEC tunneling. Intercept the exfiltration channel, deploy targeted firewall rules, and roll back tainted storage.',
  pressure:'Do not terminate primary edge gateways. Isolate the DNS tunnel without knocking trusted services offline.',
  steps:[
    ['Audit resource telemetry in Task Optimizer','Inspect Task Optimizer resource utilization, queue depth, and socket throughput.','Look for anomalous spikes in network queue depth.',['open:optimizer','click:optimizer-resource-audit','terminal:service-health']],
    ['Trace DNS-SEC packet anomalies in CyberSec Arsenal','Inspect deep packet inspection logs for covert DNS-SEC TXT record exfiltration.','High entropy payloads in port 53 traffic indicate tunneling.',['open:cybersec','click:cybersec-packet-inspect','terminal:netstat']],
    ['Inspect network listener states in Terminal','Use Terminal to inspect socket listeners and service routes.','Run netstat and service-health to pinpoint degraded listener.',['open:terminal','terminal:netstat','terminal:service-health']],
    ['Correlate covert exfiltration with Aegis Mind','Ask Aegis Mind to compare legitimate DNS traffic with the covert exfiltration channel.','Steganographic tunneling uses repetitive TXT queries.',['open:ai','click:ai-correlate-traffic','open:notepad']],
    ['Audit security diagnostic logs in Troubleshoot','Run Troubleshoot diagnostic to locate anomalous network egress records.','Locate anomalous IP destination in audit logs.',['open:troubleshoot','open:terminal','terminal:capability-audit']],
    ['Choose exfiltration mitigation strategy','Balance edge availability, false positive rate, and containment speed.','Adaptive filtering isolates hostile endpoints without downtime.','choice',['Deploy DNS sinkhole rule & Merkle rollback','Sever all network interfaces','Log only']],
    ['Apply firewall drop rule against adversary relay','Deploy Aegis Shield rule blocking hostile relay 198.51.100.22.','Ensure legitimate edge routes remain online.',['open:shield','change:shield-rate-policy','click:shield-edge-audit']],
    ['Execute Merkle Atomic Rollback on affected block','Roll back corrupted block using Merkle immutable root tree.','Verify rollback takes under 10 seconds with zero data loss.',['click:merkle-rollback','open:terminal','terminal:verify-root']],
    ['Verify restored route integrity','Use Terminal to verify that resilient edge-route-02 carries traffic cleanly.','Run route-select edge-route-02 and check service-health.',['open:terminal','terminal:route-health','terminal:route-select']],
    ['Submit forensic attribution report','Submit adversary IP, exfiltration protocol, and proof of recovery.','Full post-mortem report required for case sign-off.','report']
  ],
  choices:['Deploy DNS sinkhole rule & Merkle rollback','Sever all network interfaces','Log only'],
  outcomes:['Covert DNS tunnel severed. Edge services maintained 100% availability.','Network shutdown stopped exfiltration, but severed operational edge.','Exfiltration succeeded. Hostile nation-state captured quantum cipher keys.'],
  quiz:[
    {id:'relay_ip',question:'Which adversary C2 IP address was hosting the DNS covert tunnel?',placeholder:'IP ADDRESS',accept:['198.51.100.22','185.220.101.5'],hint:'Check packet inspection telemetry in CyberSec Arsenal.'},
    {id:'protocol',question:'Which protocol was abused for covert channel framing?',placeholder:'PROTOCOL',accept:['DNS-SEC','DNS','UDP 53'],hint:'Compare protocol column in packet inspection table.'}
  ]
},
{
  id:'op6',
  title:'FIRMWARE SUPPLY-CHAIN POISONING',
  brief:'A compromised signed peripheral firmware component injected a shadow interrupt handler into ring-0. Triage microkernel diagnostics, kill the rogue ingress PID, and verify hardware SPI root-of-trust.',
  pressure:'Shadow interrupts bypass normal userland monitors. Root-of-trust verification is mandatory.',
  steps:[
    ['Execute microkernel ASLR diagnostic in Troubleshoot','Run system diagnostic scanner to test microkernel memory isolation.','Page table isolation detects overlapping ring transitions.',['open:troubleshoot','click:troubleshoot-run-diagnostic','open:neural']],
    ['Map memory ring transition in Neural Monitor','Inspect Neural Monitor memory view to locate the rogue interrupt vector.','Look for hooks modifying IDT entries.',['open:neural','click:neural-memory-map','terminal:ps']],
    ['Audit capability grants in Shield','Inspect Shield Zero-Trust Tokens to find unauthorized elevated session tokens.','Cross-reference active sessions with rogue worker processes.',['open:shield','click:shield-token-audit','terminal:capability-audit']],
    ['Trace and kill rogue ingress daemon PID 4819','Use Terminal to inspect capability grants and terminate rogue worker PID 4819.','Run capability-audit and kill-rogue-pid to sever ingress.',['open:terminal','terminal:capability-audit','terminal:ps']],
    ['Correlate firmware signature anomalies with Aegis Mind','Ask Aegis Mind to correlate diagnostic faults, IDT hooks, and hardware tokens.','Root cause is firmware supply chain tampering.',['open:ai','click:ai-correlate-kernel','open:notepad']],
    ['Choose firmware remediation strategy','Choose the remediation procedure that restores root-of-trust without bricking the board.','Reflash verified microcode and execute atomic Merkle rollback.','choice',['Hardware SPI Flash verification & service restart','Disable microkernel paging','Defer maintenance']],
    ['Quarantine rogue firmware driver','Run Aegis Guard and quarantine the untrusted hardware driver.','Run quarantine-component in Terminal to lock down driver.',['click:shield-guard-scan','open:terminal','terminal:quarantine-component']],
    ['Verify root of trust with Merkle scan','Run Merkle integrity verification to confirm root hash is verified clean.','Run verify-root after scan to complete cryptographic audit.',['open:merkle','click:merkle-integrity-scan','terminal:verify-root']],
    ['Restart hardened ingress gateway','Verify and restart degraded ingress services in Terminal.','Run service-restart to return gateway to HEALTHY state.',['open:terminal','terminal:service-health','click:optimizer-resource-audit']],
    ['Submit final Master Operator dossier','Submit full root cause, driver provenance, and cryptographic verification proof.','Final dossier required for Grandmaster Operator certification.','report']
  ],
  choices:['Hardware SPI Flash verification & service restart','Disable microkernel paging','Defer maintenance'],
  outcomes:['Microkernel root-of-trust re-established. Firmware integrity certified.','Disabling paging introduced severe privilege vulnerabilities.','Kernel compromised. System security compromised.'],
  quiz:[
    {id:'rogue_pid',question:'Which rogue daemon PID hooked the shadow interrupt vector?',placeholder:'PID',accept:['4819'],hint:'Inspect Terminal capability-audit output for the anomalous listener.'},
    {id:'subsystem',question:'Which security subsystem verified the hardware root of trust?',placeholder:'SUBSYSTEM',accept:['Merkle','ASLR','Microkernel Page Table Isolation'],hint:'Review Merkle integrity scanner and Troubleshoot diagnostic.'}
  ]
},
{
  id:'op7',
  title:'SATCOM TELEMETRY INTERCEPT & FIRMWARE HIJACK',
  brief:'Hostile APT-28 hijacked an orbital satellite ground transceiver. Malicious downlink microcode was broadcast to orbital repeater node 4. Intercept telemetry, carve satellite microcode blob, revoke transceiver capability, and lock the orbital uplink.',
  pressure:'Orbital window closes in minutes. Restoring corrupted transceiver microcode requires surgical hex carving and Merkle rollback.',
  steps:[
    ['Audit ground transceiver telemetry in CyberSec Arsenal','Inspect high-entropy satcom telemetry downlink on port 9100.','Identify unauthenticated downlink carrier frames.',['open:cybersec','click:cybersec-packet-inspect','terminal:netstat|terminal:service-health']],
    ['Audit rogue satellite worker PID 5912 in Neural Monitor','Inspect Neural Monitor process tree for unverified uplink daemon.','Trace parent-child lineage of satcom_relay worker.',['open:neural','click:neural-process-detail','terminal:ps|terminal:capability-audit']],
    ['Analyze telemetry packet anomalies in Terminal','Run netstat and capability-audit to trace rogue ground transceiver sockets.','Confirm foreign ground station socket on port 9100.',['open:terminal','terminal:capability-audit','terminal:netstat|terminal:service-health']],
    ['Carve satcom firmware payload in File Recovery','Locate corrupted satellite firmware blob satcom_fw.bin.corrupt and carve with magic bytes 53 41 54 43.','Carve magic bytes 53 41 54 43 (SATC) to restore valid telemetry microcode.',['open:storage','click:storage-recovery-carve','open:files|click:files-inspect-suspicious']],
    ['Correlate orbital anomalies with Aegis Mind AI','Ask Aegis Mind to synthesize downlink packets, rogue daemon, and carved microcode.','AI confirms adversary pushed unauthorized telemetry patch.',['open:ai','click:ai-correlate-traffic','terminal:journalctl|open:notepad']],
    ['Choose satcom containment strategy','Select containment: Emergency uplink lock vs broad spectrum shutdown.','Targeted uplink lock preserves earth observation feeds while severing hostile C2.','choice',['Targeted ground transceiver lock','Global orbital satellite shutdown','Ignore orbital drift']],
    ['Revoke rogue transceiver capability in Shield','Open Aegis Shield and revoke raw RF socket capability held by PID 5912.','Deploy firewall rule blocking foreign ground relay.',['open:shield','change:shield-rule','click:shield-edge-audit']],
    ['Verify orbital uplink containment in Terminal','Run Aegis Guard scan and execute verify-containment to confirm socket termination.',['click:shield-guard-scan','open:terminal','terminal:verify-containment']],
    ['Execute atomic Merkle rollback on satellite storage block','Roll back corrupted satellite microcode block to clean checkpoint.','Run verify-root to prove cryptographic ground trust.',['open:merkle','click:merkle-rollback','terminal:verify-root']],
    ['Submit Satcom Incident Post-Mortem','Submit hostile frequency, carved magic bytes, and proof of uplink containment.','All evidence categories and defensible decision required.','report']
  ],
  choices:['Targeted ground transceiver lock','Global orbital satellite shutdown','Ignore orbital drift'],
  outcomes:['Orbital repeater secured and satellite firmware restored clean. Zero telemetry loss.','Global shutdown severed national weather telemetry feeds. Operational disruption.','Satellite hijacked and de-orbited. Mission catastrophic failure.'],
  quiz:[
    {id:'magic',question:'What was the 4-byte magic header carved for satcom_fw.bin?',placeholder:'HEX BYTES',accept:['53 41 54 43','53415443','SATC'],hint:'Inspect the Hex Carver recovery hint for satcom_fw.bin.corrupt.'},
    {id:'frequency',question:'Which listening port was exploited by the rogue transceiver daemon?',placeholder:'PORT',accept:['9100','PORT 9100'],hint:'Check the socket port in Terminal netstat and CyberSec Arsenal.'}
  ]
},
{
  id:'op8',
  title:'NEURAL BIOMETRIC BYPASS & DEEPFAKE MODEL INJECTION',
  brief:'An advanced persistent threat injected poisoned weight tensors into the facial biometric verification enclave. The model accepts deepfake synthetic avatars as root administrators. Audit tensor drift, quarantine the tainted model, carve clean model weights, and restore the biometric root-of-trust.',
  pressure:'Unauthorized administrative tokens are actively minting. Deepfake injection bypasses standard cryptographic sign-off without neural tensor verification.',
  steps:[
    ['Run microkernel memory diagnostic in Troubleshoot','Execute Troubleshoot system diagnostic to detect memory enclave drift.','Page table isolation identifies foreign tensor injection.',['open:troubleshoot','click:troubleshoot-run-diagnostic','open:neural|terminal:ps']],
    ['Audit biometric model memory arena in Neural Monitor','Inspect Neural Monitor memory map to locate poisoned biometric weights.','Look for anomalous weights outside the declared neural sandbox.',['open:neural','click:neural-memory-map','click:neural-process-detail']],
    ['Inspect unverified auth tokens in Aegis Shield','Switch to Zero-Trust Tokens in Shield to audit anomalous elevated biometric grants.','Locate unauthorized token TOK-BIOMETRIC-ROOT.',['open:shield','click:shield-token-audit','terminal:capability-audit']],
    ['Carve clean biometric model header in File Recovery','Locate biometric_model.weights.corrupt and deploy Hex Carver with magic bytes 42 49 4f 53.','Carving magic bytes 42 49 4f 53 (BIOS) restores clean neural inference tensor.',['open:storage','click:storage-recovery-carve','open:files|click:files-inspect-library']],
    ['Correlate deepfake root cause with Aegis Mind AI','Ask Aegis Mind to synthesize biometric auth faults, token escalation, and weight drift.','Root cause is poisoned deepfake training tensor weights.',['open:ai','click:ai-correlate-kernel','terminal:service-health']],
    ['Choose biometric remediation strategy','Decide whether to isolate biometric enclave or disable hardware biometrics entirely.','Enclave isolation and clean weight restore preserves authentication security.','choice',['Biometric enclave isolation & weight restore','Disable all user biometric verification','Bypass verification gates']],
    ['Revoke poisoned neural capability in Aegis Shield','Use Aegis Shield to revoke unauthorized raw neural inference capability.','Prevent rogue deepfakes from executing ring-0 system calls.',['open:shield','change:shield-rule','click:shield-guard-scan']],
    ['Verify enclave containment in Terminal','Run Aegis Guard scan and execute verify-containment in Terminal.','Confirm deepfake authentication vector is neutralized.',['click:shield-guard-scan','open:terminal','terminal:verify-containment']],
    ['Verify root of trust with Merkle scan','Execute Merkle integrity scan and restore clean baseline checkpoint.','Run verify-root in Terminal to seal the cryptographic enclave.',['open:merkle','click:merkle-integrity-scan','terminal:verify-root']],
    ['Submit Biometric Security Post-Mortem','Submit poisoned token name, carved model magic bytes, and proof of containment.','Final incident dossier required for Chief Security Officer sign-off.','report']
  ],
  choices:['Biometric enclave isolation & weight restore','Disable all user biometric verification','Bypass verification gates'],
  outcomes:['Biometric enclave restored and deepfake bypass neutralized with zero downtime.','Hardware biometrics disabled, forcing insecure fallback credentials.','Administrative credentials compromised across entire infrastructure. Catastrophic failure.'],
  quiz:[
    {id:'token',question:'Which elevated token was exploited by the deepfake injection?',placeholder:'TOK-...',accept:['TOK-BIOMETRIC-ROOT','TOK-BIOMETRIC-ESCALATED'],hint:'Inspect Shield Zero-Trust token registry for the anomalous biometric grant.'},
    {id:'magic',question:'What 4-byte magic header was carved for biometric_model.weights?',placeholder:'HEX BYTES',accept:['42 49 4f 53','42494f53','BIOS'],hint:'Check the Hex Carver hint for biometric_model.weights.corrupt.'}
  ]
}
];

function fresh(){return {v:1,index:0,started:false,done:[],evidence:{},fragments:{},decision:{},answers:{},stats:{xp:0,credits:0,research:0,score:0,history:[]},achievements:[],log:[]};}
var state=fresh(),authenticated=false,syncing=false,saveTimer=null;
if(!state||state.v!==1)state=fresh();

function save(){
  if(!authenticated)return;
  try{localStorage.setItem(KEY,JSON.stringify(state));}catch(e){}
  if(saveTimer)clearTimeout(saveTimer);
  saveTimer=setTimeout(function(){
    fetch('/api/campaign/state',{
      method:'POST',
      credentials:'same-origin',
      headers:{'Content-Type':'application/json'},
      body:JSON.stringify({state:state})
    }).catch(function(){});
  },250);
}

function mission(){return missions[Math.min(state.index,missions.length-1)];}
function evidence(){var m=mission();return state.evidence[m.id]||(state.evidence[m.id]=[]);}
function fragments(){var m=mission();if(!state.fragments)state.fragments={};return state.fragments[m.id]||(state.fragments[m.id]={});}
function answersCorrect(m){var values=state.answers&&state.answers[m.id]||{};return !m.quiz||m.quiz.every(function(q){var value=String(values[q.id]||'').toLowerCase().replace(/[^a-z0-9]+/g,'');return q.accept.some(function(a){return value===a.toLowerCase().replace(/[^a-z0-9]+/g,'');});});}

function ensureProfileState(){
  if(!state.answers)state.answers={};
  if(!state.stats)state.stats={xp:0,credits:0,research:0,score:0,history:[]};
  if(!Array.isArray(state.stats.history))state.stats.history=[];
  if(!state.fragments)state.fragments={};
  if(!Array.isArray(state.achievements))state.achievements=[];
}

function record(s){state.log.push({id:mission().id,t:new Date().toLocaleTimeString(),s:s});if(state.log.length>200)state.log=state.log.slice(-200);save();}
function el(tag,cls,txt){var n=document.createElement(tag);if(cls)n.className=cls;if(txt!==undefined)n.textContent=txt;return n;}

var panel,body,count,sfxBtn;
var copilotPopup,copilotBody,copilotResponses,copilotOrb,copilotBadge;

function toast(title,desc,grade){
  if(window.AegisSystemNotify){window.AegisSystemNotify(title,desc,'info');return;}
  var n=el('div','aegis-native-toast');
  var head=el('b','',title);
  if(grade){var badge=el('span','mission-grade-badge grade-'+grade.toLowerCase(),'GRADE '+grade);head.append(badge);}
  n.append(head,el('span','',desc));
  document.body.append(n);
  setTimeout(function(){n.remove();},5000);
}

function unlockAchievement(id){
  ensureProfileState();
  if(state.achievements.includes(id))return;
  var ach=ACHIEVEMENTS.find(function(a){return a.id===id;});
  if(!ach)return;
  state.achievements.push(id);
  state.stats.xp+=(ach.xp||100);
  state.stats.credits+=(ach.credits||100);
  playSfx('achievement');
  record('Achievement unlocked: '+ach.title+' ('+ach.icon+')');
  save();
  render();
  achievementToast(ach);
}

function achievementToast(ach){
  var n=el('div','aegis-native-toast achievement-toast');
  var head=el('b','',ach.icon+' ACHIEVEMENT UNLOCKED: '+ach.title);
  n.append(head,el('span','',ach.desc+' (+'+ach.xp+' XP, +'+ach.credits+' Credits)'));
  document.body.append(n);
  setTimeout(function(){n.remove();},5500);
}

var currentTab = 'operations';
var activeEmergency = null;
var emergencyInterval = null, emergencyTimeOut = null;

function render(){
  if(!body)return;
  body.innerHTML='';

  var tabNav = el('div', 'aegis-campaign-tabs');
  var opsTab = el('button', 'camp-tab-btn' + (currentTab==='operations'?' active':''), '📂 CASES & MISSIONS');
  var clueTab = el('button', 'camp-tab-btn' + (currentTab==='clues'?' active':''), '🎴 CLUE CARDS');
  var drillsTab = el('button', 'camp-tab-btn' + (currentTab==='drills'?' active':''), '⚡ EMERGENCY ATTACKS');
  var achTab = el('button', 'camp-tab-btn' + (currentTab==='achievements'?' active':''), '🏆 ACHIEVEMENTS ('+state.achievements.length+'/'+ACHIEVEMENTS.length+')');
  var closedTab = el('button', 'camp-tab-btn' + (currentTab==='closed'?' active':''), '🗄️ CLOSED CASES ('+state.done.length+')');

  opsTab.onclick = function(){ currentTab='operations'; render(); };
  clueTab.onclick = function(){ currentTab='clues'; render(); };
  drillsTab.onclick = function(){ currentTab='drills'; render(); };
  achTab.onclick = function(){ currentTab='achievements'; render(); };
  closedTab.onclick = function(){ currentTab='closed'; render(); };

  tabNav.append(opsTab, clueTab, drillsTab, achTab, closedTab);
  body.append(tabNav);

  if(currentTab==='achievements'){ renderAchievementsView(); return; }
  if(currentTab==='drills'){ renderDrillsView(); return; }
  if(currentTab==='clues'){ renderClueCardsView(); return; }
  if(currentTab==='closed'){ renderClosedCasesView(); return; }

  var m=mission();
  var head=el('div','mission-head');
  head.append(el('h2','',m.title));
  var comm=el('div','mission-comm-bubble');
  comm.append(el('small','','TACTICAL COMMS // DIRECTOR VANCE'),el('span','','"'+m.brief+'"'));
  head.append(comm,el('p','mission-pressure',m.pressure));

  var done=0;
  m.steps.forEach(function(s,i){
    var kind=s[3],complete=kind==='choice'?!!state.decision[m.id]:kind==='report'?state.done.includes(m.id):evidence().includes(i);
    if(complete)done++;
  });

  head.append(el('div','mission-progress-label',done+' / '+m.steps.length+' investigative milestones secured'));
  var bar=el('div','mission-progress-native'),fill=el('i','');
  fill.style.width=Math.round(done/m.steps.length*100)+'%';
  bar.append(fill);
  head.append(bar);

  var stats=state.stats||{xp:0,credits:0,research:0,score:0};
  head.append(el('div','mission-meta','COMMAND SCORE '+stats.score+'  ·  XP '+stats.xp+'  ·  CREDITS '+stats.credits+'  ·  RESEARCH '+stats.research));
  body.append(head);

  var list=el('ol','mission-steps');
  m.steps.forEach(function(s,i){
    var kind=s[3];
    var complete=kind==='choice'?!!state.decision[m.id]:kind==='report'?state.done.includes(m.id):evidence().includes(i);
    var partial=Array.isArray(s[3])?(fragments()[i]||[]).length:0;
    var li=el('li','mission-objective'+(complete?' is-done':partial?' is-progress':''));
    li.append(el('span','mission-step',String(i+1).padStart(2,'0')),el('div','mission-objective-copy',s[0]));

    if(!complete&&kind!=='choice'&&kind!=='report'){
      li.append(el('small','',s[1]));

      // Multi-stage visual badges
      if(Array.isArray(s[3])){
        var stageTrack = el('div', 'step-stage-track');
        var progressArr = fragments()[i] || [];
        var stageLabels = ['1. Reconnaissance', '2. Deep Triage', '3. Verification & Seal', '4. Defense Check'];

        s[3].forEach(function(stgToken, si){
          var isDone = progressArr.includes(si);
          var isActive = !isDone && (si === 0 || progressArr.includes(si - 1));
          var pill = el('span', 'stage-pill' + (isDone ? ' is-stage-done' : isActive ? ' is-stage-active' : ''), (isDone ? '✓ ' : isActive ? '⏳ ' : '🔒 ') + (stageLabels[si] || ('Stage ' + (si+1))));
          stageTrack.append(pill);
        });
        li.append(stageTrack);
      }

      // Elaborated Tactical Intelligence & Multi-Stage Guide
      var guideWrap = el('div', 'step-guide-wrap');
      var guideToggle = el('button', 'btn-step-guide-toggle', '💡 Tactical Intelligence & Stages ▼');
      guideToggle.type = 'button';
      var guideContent = el('div', 'step-guide-content hidden');

      var guidesList = STAGE_GUIDES[m.id] || [];
      var guideData = guidesList[i] || {
        target: s[1],
        p1: 'Stage 1: Open required tool from dock or top bar.',
        p2: 'Stage 2: Execute query or forensic inspection on indicated anomaly.',
        p3: 'Stage 3: Corroborate telemetry across security subsystems.',
        bg: 'Targeted verification ensures zero false positives before applying containment.'
      };

      var cluesForStep = (CLUE_CARDS[m.id] || []).filter(function(c){ return c.stepIndex === i; });
      var toolApp = cluesForStep[0] ? cluesForStep[0].app : 'terminal';

      var elabBox = el('div', 'step-elaborated-box');
      elabBox.append(
        el('div', 'elab-ioc', '🎯 TARGET IOC: ' + guideData.target),
        el('div', 'elab-proc', '🔹 ' + guideData.p1),
        el('div', 'elab-proc', '🔹 ' + guideData.p2),
        el('div', 'elab-proc', '🔹 ' + guideData.p3),
        el('div', 'elab-bg', '🛡️ Forensic Context: ' + guideData.bg)
      );
      guideContent.append(elabBox);

      var jumpBtn = el('button', 'btn-action-small btn-step-jump', '🚀 Open ' + toolApp.toUpperCase());
      jumpBtn.type = 'button';
      jumpBtn.onclick = function() {
        var btn = document.querySelector('[data-app="' + toolApp + '"]');
        if (btn) btn.click();
        toast('TOOL DISPATCHED', 'Opening ' + toolApp.toUpperCase() + ' for milestone ' + (i + 1), 'I');
      };
      guideContent.append(jumpBtn);

      guideToggle.onclick = function() {
        guideContent.classList.toggle('hidden');
        guideToggle.textContent = guideContent.classList.contains('hidden') ? '💡 Tactical Intelligence & Stages ▼' : '💡 Hide Tactical Guide ▲';
      };
      guideWrap.append(guideToggle, guideContent);
      li.append(guideWrap);
    }

    list.append(li);
  });
  body.append(list);

  var choiceStep=m.steps.find(function(s){return s[3]==='choice';});
  if(choiceStep&&!state.decision[m.id]){
    var c=el('div','mission-choice');
    c.append(el('h3','',choiceStep[0]),el('p','',choiceStep[1]));
    var choiceIndex=m.steps.indexOf(choiceStep),requiredBefore=m.steps.slice(0,choiceIndex).filter(function(s){return Array.isArray(s[3]);}).length,decisionUnlocked=evidence().length>=requiredBefore;
    m.choices.forEach(function(label,i){
      var b=el('button','mission-choice-btn');
      b.type='button';
      b.disabled=!decisionUnlocked;
      b.append(el('strong','',label),el('small','',choiceStep[2]));
      b.addEventListener('click',function(){
        if(!decisionUnlocked)return;
        state.decision[m.id]=i;
        playSfx('decision');
        record('Command decision: '+label);
        render();
        toast('Decision recorded',label);
      });
      c.append(b);
    });
    if(!decisionUnlocked)c.append(el('small','mission-choice-lock','Complete earlier investigation milestones before selecting a response.'));
    body.append(c);
  }

  var report=m.steps.find(function(s){return s[3]==='report';});
  if(report){
    var quiz=el('div','mission-quiz');
    quiz.append(el('h3','','FINAL DEDUCTION // INCIDENT REPORT'));
    (m.quiz||[]).forEach(function(q){
      var label=el('label','mission-quiz-label',q.question);
      var input=el('input','mission-quiz-input');
      input.type='text';
      input.autocomplete='off';
      input.placeholder=q.placeholder;
      input.value=(state.answers&&state.answers[m.id]&&state.answers[m.id][q.id])||'';
      input.addEventListener('input',function(){
        if(!state.answers)state.answers={};
        if(!state.answers[m.id])state.answers[m.id]={};
        state.answers[m.id][q.id]=input.value;
        save();
        submit.disabled=!ready();
      });
      var hint=el('small','',q.hint);
      label.append(input,hint);
      quiz.append(label);
    });
    body.append(quiz);
    var submit=el('button','mission-report-btn',state.done.includes(m.id)?'OPERATION SECURED':'SUBMIT EVIDENCE & CLOSE OPERATION');
    submit.type='button';
    function ready(){
      return evidence().length>=m.steps.filter(function(s){return Array.isArray(s[3]);}).length&&state.decision[m.id]!==undefined&&answersCorrect(m);
    }
    submit.disabled=!ready();
    submit.addEventListener('click',complete);
    body.append(submit);
    body.append(el('small','mission-report-note',ready()?'Deductions verified. Your incident report is ready for command review.':'The report stays sealed until every evidence source, strategic decision and final deduction is complete.'));
  }

  var logs=el('div','mission-log');
  logs.append(el('h3','','RECENT FIELD NOTES'));
  state.log.filter(function(x){return x.id===m.id;}).slice(-5).reverse().forEach(function(x){logs.append(el('p','',x.t+'  '+x.s));});
  body.append(logs);

  var hint=el('details','mission-hint');
  hint.append(el('summary','','Request a field hint'));
  var next=m.steps.find(function(s,i){return Array.isArray(s[3])&&!evidence().includes(i);});
  hint.append(el('p','',next?next[2]:'Review your decision and final report.'));
  body.append(hint);

  if(count)count.textContent=done+'/'+m.steps.length;
}

function renderAchievementsView(){
  var box = el('div', 'camp-view-container');
  var header = el('div', 'camp-view-header');
  header.append(el('h3', '', 'OPERATOR SECURITY ACHIEVEMENTS & COMMENDATIONS'));
  header.append(el('p', 'camp-view-desc', 'Unlock badges and earn Defense XP & Tactical Credits across operational investigations and emergency incident triage.'));
  box.append(header);

  var grid = el('div', 'achievements-grid');
  ACHIEVEMENTS.forEach(function(a){
    var unlocked = state.achievements.includes(a.id);
    var card = el('div', 'achievement-card' + (unlocked ? ' is-unlocked' : ' is-locked'));
    var icon = el('div', 'achievement-icon', a.icon);
    var info = el('div', 'achievement-info');
    info.append(el('h4', '', a.title), el('p', '', a.desc));
    var meta = el('div', 'achievement-meta', '+'+a.xp+' XP  ·  +'+a.credits+' CREDITS' + (unlocked ? '  ✓ EARNED' : '  🔒 LOCKED'));
    info.append(meta);
    card.append(icon, info);
    grid.append(card);
  });
  box.append(grid);
  body.append(box);
}

function renderDrillsView(){
  var box = el('div', 'camp-view-container');
  var header = el('div', 'camp-view-header');
  header.append(el('h3', '', 'CRITICAL EMERGENCY ATTACK SCENARIOS'));
  header.append(el('p', 'camp-view-desc', 'Simulate high-pressure live cyber breaches. Respond under countdown before hostile adversaries exfiltrate data or destabilize the microkernel!'));
  box.append(header);

  var list = el('div', 'drills-list');
  TIMED_SCENARIOS.forEach(function(sc){
    var card = el('div', 'drill-card');
    var top = el('div', 'drill-card-top');
    top.append(el('span', 'drill-type-badge', sc.threatType), el('span', 'drill-time-badge', '⏱️ '+sc.seconds+'s TIMEOUT'));
    card.append(top, el('h4', '', sc.title), el('p', 'drill-brief', sc.brief));

    var btnRow = el('div', 'drill-btn-row');
    var triggerBtn = el('button', 'btn-trigger-drill', '🔥 SIMULATE ATTACK');
    triggerBtn.type = 'button';
    triggerBtn.onclick = function(){ triggerTimedAttack(sc.id); };
    btnRow.append(triggerBtn);
    card.append(btnRow);
    list.append(card);
  });
  box.append(list);
  body.append(box);
}

function renderClueCardsView(){
  var box = el('div', 'camp-view-container');
  var header = el('div', 'camp-view-header');
  header.append(el('h3', '', 'FORENSIC CLUE CARDS & EVIDENCE DOSSIER'));
  header.append(el('p', 'camp-view-desc', 'Deep forensic telemetry and cryptographic indicators recovered from active and past operations.'));
  box.append(header);

  var grid = el('div', 'clue-cards-grid');
  var cards = CLUE_CARDS[mission().id] || [];
  if(cards.length === 0){
    var empty = el('div', 'coming-soon-banner');
    empty.append(el('div', 'cs-icon', '🎴'), el('h4', '', 'CLUE DOSSIER INITIALIZING'), el('p', '', 'Evidence will be populated as you uncover forensic clues in the active operation.'));
    box.append(empty);
  } else {
    cards.forEach(function(c){
      var card = el('div', 'clue-card-item');
      card.append(
        el('div', 'clue-badge-row', c.badge),
        el('h4', '', c.title),
        el('p', '', c.hint)
      );
      card.onclick = function(){
        if(window.AegisDialog){
          window.AegisDialog.alert(c.guide, c.title);
        }
      };
      grid.append(card);
    });
    box.append(grid);
  }
  body.append(box);
}

function renderClosedCasesView(){
  var box = el('div', 'camp-view-container');
  var header = el('div', 'camp-view-header');
  header.append(el('h3', '', 'ARCHIVED & CLOSED INCIDENT DOSSIERS'));
  header.append(el('p', 'camp-view-desc', 'Official post-mortems of resolved operations, completed bounties, and neutralized emergency threats.'));
  box.append(header);

  var sec1 = el('div', 'closed-section');
  sec1.append(el('h4', 'closed-section-title', '1. SECURED CAMPAIGN OPERATIONS'));
  var closedOps = missions.filter(function(m){ return state.done.includes(m.id); });
  if(closedOps.length === 0){
    var emptyOps = el('div', 'coming-soon-banner');
    emptyOps.append(el('div', 'cs-icon', '📁'), el('h4', '', 'NO CLOSED OPERATIONS YET'), el('p', '', 'Complete all investigative milestones and submit your Incident Report to close an operation.'));
    sec1.append(emptyOps);
  } else {
    var opList = el('div', 'closed-ops-list');
    closedOps.forEach(function(op){
      var item = el('div', 'closed-op-card');
      item.append(
        el('div', 'closed-op-header', 'CASE RESOLVED: ' + op.title),
        el('p', 'closed-op-desc', op.brief)
      );
      opList.append(item);
    });
    sec1.append(opList);
  }
  box.append(sec1);
  body.append(box);
}

function triggerTimedAttack(scenarioId){
  if(activeEmergency)return;
  var sc = TIMED_SCENARIOS.find(function(s){return s.id===scenarioId;}) || TIMED_SCENARIOS[0];
  activeEmergency = {
    id: sc.id,
    title: sc.title,
    secondsLeft: sc.seconds,
    totalSeconds: sc.seconds,
    defusalActions: sc.defusalActions,
    startedAt: Date.now()
  };

  playSfx('emergency');
  toast('RED ALERT: '+sc.title, sc.brief, 'EMERGENCY');
  showAiDialogue('🚨 CRITICAL ATTACK IN PROGRESS: '+sc.title, sc.brief + ' TIMEOUT: '+sc.seconds+' SECONDS! Defusal Clue: '+sc.promptClue, [
    { q: 'Deploy Emergency Defusal', a: sc.promptClue },
    { q: 'Open CyberSec Arsenal', a: 'Opening CyberSec Arsenal...' },
    { q: 'Open Aegis Shield', a: 'Opening Shield firewall...' }
  ]);

  renderEmergencyBanner();
  emergencyInterval = setInterval(function(){
    if(!activeEmergency)return;
    activeEmergency.secondsLeft--;
    updateEmergencyBanner();
    if(activeEmergency.secondsLeft<=0){
      clearInterval(emergencyInterval);
      triggerEmergencyFailure();
    }
  }, 1000);
}

function renderEmergencyBanner(){
  var old = document.getElementById('aegis-emergency-banner');
  if(old)old.remove();
  var banner = el('div', 'aegis-emergency-banner');
  banner.id = 'aegis-emergency-banner';
  var inner = el('div', 'aegis-emergency-inner');
  inner.append(
    el('span', 'emg-pulsing-icon', '🚨'),
    el('strong', 'emg-title', activeEmergency.title),
    el('span', 'emg-timer', activeEmergency.secondsLeft+'s REMAINING')
  );
  var defuseHint = el('span', 'emg-hint', 'DEFUSAL: '+activeEmergency.defusalActions.join(' or '));
  inner.append(defuseHint);
  banner.append(inner);
  document.body.prepend(banner);
}

function updateEmergencyBanner(){
  var timer = document.querySelector('#aegis-emergency-banner .emg-timer');
  if(timer && activeEmergency) timer.textContent = activeEmergency.secondsLeft+'s REMAINING';
}

function removeEmergencyBanner(){
  var b = document.getElementById('aegis-emergency-banner');
  if(b) b.remove();
}

function triggerEmergencyFailure(){
  removeEmergencyBanner();
  var title = activeEmergency ? activeEmergency.title : 'SYSTEM BREACH';
  activeEmergency = null;
  playSfx('alert');
  triggerCriticalBreachCountdown(title);
}

function triggerCriticalBreachCountdown(threatTitle){
  var overlay = el('div', 'aegis-breach-overlay');
  overlay.id = 'aegis-critical-breach-screen';
  var box = el('div', 'aegis-breach-box');
  box.append(
    el('div', 'breach-skull', '☠️'),
    el('h1', 'breach-heading', 'YOU HAVE BEEN HACKED'),
    el('p', 'breach-sub', 'CRITICAL BREACH DETECTED: ' + (threatTitle || 'HOSTILE ROOTKIT INTRUSION')),
    el('p', 'breach-warning', 'Zero-day payload has compromised ring-0 microkernel state. All local userland storage blocks and dossier notes will be wiped in 30 seconds!')
  );

  var timerEl = el('div', 'breach-timer', '30.0s');
  box.append(timerEl);

  var btnFix = el('button', 'btn-fix-system', '🛠️ FIX THE SYSTEM (EMERGENCY KERNEL RESTORE)');
  btnFix.type = 'button';
  box.append(btnFix);
  overlay.append(box);
  document.body.append(overlay);

  var secondsLeft = 30.0;
  var answered = false;

  var interval = setInterval(function(){
    secondsLeft = Math.max(0, secondsLeft - 0.1);
    timerEl.textContent = secondsLeft.toFixed(1) + 's';
    if(secondsLeft <= 0 && !answered){
      clearInterval(interval);
      answered = true;
      executeSystemWipe(overlay);
    }
  }, 100);

  btnFix.onclick = function(){
    if(answered) return;
    answered = true;
    clearInterval(interval);
    overlay.remove();
    playSfx('victory');
    toast('SYSTEM SAVED!', 'Emergency kernel recovery completed with ' + secondsLeft.toFixed(1) + 's remaining!', 'S');
    showAiDialogue('SYSTEM RESTORED // THREAT REPELLED', 'Phenomenal reflexes, Operator! You defused the critical breach in time. All storage blocks and memory pages have been verified clean.', [
      { q: 'Resume Operations', a: 'Standing by.' }
    ]);
  };
}

function executeSystemWipe(overlay){
  overlay.innerHTML = '<div class="aegis-breach-box"><div class="breach-skull">💥</div><h1>SYSTEM BREACHED // STORAGE WIPED</h1><p>Filesystem wiped by ransomware payload. Initializing emergency file recovery manifest...</p></div>';
  fetch('/api/system/trigger-wiped', { method: 'POST' }).catch(function(){});
  setTimeout(function(){
    overlay.remove();
    toast('FILES WIPED', 'All user files corrupted! Use File Recovery & Hex Carver to reconstruct them.', 'ALERT');
    showAiDialogue('🚨 CRITICAL DATA LOSS // RECOVERY REQUIRED', 'The system breach wiped userland storage partitions! You must now open Files & Storage Manager -> File Recovery to carve and recover the corrupted inodes.', [
      { q: 'Open File Recovery (Carver)', a: 'Opening File Recovery...' }
    ]);
  }, 3000);
}

function observe(type,value){
  // Check active emergency defusal
  if(activeEmergency){
    var matchedEmg=activeEmergency.defusalActions.some(function(act){
      var p=act.split(':');
      return p[0]===type&&(p[1]==='*'||p[1]===value);
    });
    if(matchedEmg){
      var elapsed=Math.round((Date.now()-activeEmergency.startedAt)/1000);
      clearInterval(emergencyInterval);
      playSfx('victory');
      toast('CRISIS NEUTRALIZED!',activeEmergency.title+' stopped with '+activeEmergency.secondsLeft+'s left!','S');
      showAiDialogue('CRISIS NEUTRALIZED // MISSION SUCCESS','Outstanding reaction time, Investigator! The attack was neutralized in '+elapsed+' seconds. System telemetry has stabilized.',[
        { q:'Resume standard operations ↗', a:'Active case dossier is ready for inspection.' }
      ]);
      unlockAchievement('timed_hero');
      if(elapsed<=60)unlockAchievement('rapid_triage');
      state.stats.xp+=150;
      state.stats.credits+=200;
      activeEmergency=null;
      removeEmergencyBanner();
      save();
      render();
    }
  }

  // System-wide automatic achievements
  if(type==='browser'&&value==='shred')unlockAchievement('ghost_protocol');
  if(type==='files'&&(value==='encrypt'||value==='save'))unlockAchievement('crypt_inode');
  if(type==='shield'&&(value==='shield-token-audit'||value==='shield-rule'))unlockAchievement('zero_trust_elite');
  if(type==='merkle'&&value==='merkle-rollback')unlockAchievement('merkle_savior');

  if(!authenticated||!state.started||state.done.includes(mission().id))return;
  var m=mission(),changed=false,milestoneIndex=-1;

  // Strictly enforce sequential step progression: only the first incomplete step is active!
  var activeStepIndex = m.steps.findIndex(function(s, i) {
    return Array.isArray(s[3]) && !evidence().includes(i);
  });
  if (activeStepIndex !== -1) {
    var s = m.steps[activeStepIndex];
    var progress = fragments()[activeStepIndex] || (fragments()[activeStepIndex] = []);
    s[3].forEach(function(group, gi) {
      if (progress.includes(gi)) return;
      var matched = group.split('|').some(function(token) {
        var p = token.split(':');
        return p[0] === type && (p.slice(1).join(':') === '*' || p.slice(1).join(':') === value);
      });
      if (matched) {
        progress.push(gi);
        record('Evidence source recorded for: ' + s[0]);
        changed = true;
      }
    });
    if (progress.length===s[3].length) {
      evidence().push(activeStepIndex);
      record('Evidence milestone secured: ' + s[0]);
      milestoneIndex = activeStepIndex;
    }
  }

  if(changed){
    var toggle=document.querySelector('.aegis-mission-toggle');
    if(toggle){toggle.classList.add('has-pulse');setTimeout(function(){toggle.classList.remove('has-pulse');},1600);}
    if(milestoneIndex>=0){
      playSfx('milestone');
      if(evidence().length===1)unlockAchievement('first_breach');
      var dlg=CASE_DIALOGUES[m.id]&&CASE_DIALOGUES[m.id].steps&&CASE_DIALOGUES[m.id].steps[milestoneIndex];
      if(dlg)showAiDialogue('STEP '+(milestoneIndex+1)+' COMPLETED',dlg.text,dlg.options);
    }else{
      playSfx('clue');
    }
    render();
    toast('Field evidence recorded','A new clue has been added to the operation dossier.');
  }
}

function complete(){
  var m=mission();
  if(evidence().length<m.steps.filter(function(s){return Array.isArray(s[3]);}).length||state.decision[m.id]===undefined||!answersCorrect(m))return;
  var decision=state.decision[m.id],points=[100,60,15][decision],grade=points>=90?'S':points>=60?'A':points>=30?'C':'D';
  if(!state.stats)state.stats={xp:0,credits:0,research:0,score:0,history:[]};
  state.stats.xp+=250;state.stats.credits+=[500,250,75][decision];state.stats.research+=25;state.stats.score+=points;
  state.stats.history.push({mission:m.id,decision:decision,grade:grade,score:points,outcome:m.outcomes[decision],time:new Date().toISOString()});
  state.done.push(m.id);
  record('Evidence-backed incident report sealed. Grade '+grade+' / '+points+' command points.');
  if(answersCorrect(m))unlockAchievement('forensic_genius');
  if(grade==='S')unlockAchievement('director_commendation');
  if(state.done.length===3)unlockAchievement('citadel_sealed');
  playSfx('victory');
  var outcome=m.outcomes[decision];
  toast('CASE SECURED // GRADE '+grade,outcome,grade);
  showAiDialogue('CASE SECURED // GRADE '+grade,'🎉 INCREDIBLE WORK, INVESTIGATOR! Case #0'+(state.index+1)+' has been successfully solved and sealed with Grade '+grade+'. '+outcome+' Rewards have been transferred to your operator account.',[
    { q:'Proceed to next case →', a:'Directing Aegis Command to assign the next priority operational theater. Prepare your tools!' }
  ]);
  state.index++;
  state.started=false;
  save();
  render();
}

function start(){
  state.started=true;
  record('Investigation operational theater deployed: '+mission().title);
  playSfx('clue');
  save();
  render();
  var cd = CASE_DIALOGUES[mission().id];
  if(cd && cd.start){
    showAiDialogue('NEW CASE ASSIGNED // '+mission().title, cd.start.text, cd.start.options);
  }
}

function build(){
  panel=el('aside','aegis-mission-panel');
  panel.hidden=true;
  var header=el('div','aegis-mission-panel-head');
  header.append(el('h2','','OPERATIONS // CYBER DEFENSE CAMPAIGN'));

  sfxBtn=el('button','btn-sfx-toggle', soundOn ? '🔊 SFX' : '🔇 Muted');
  sfxBtn.type='button';
  sfxBtn.onclick=function(){
    soundOn = !soundOn;
    localStorage.setItem('aegis.sfx.enabled', soundOn ? 'true' : 'false');
    sfxBtn.textContent = soundOn ? '🔊 SFX' : '🔇 Muted';
  };
  header.append(sfxBtn);

  var close=el('button','aegis-mission-close','✕');
  close.type='button';
  close.onclick=function(){panel.hidden=true;};
  header.append(close);

  body=el('div','aegis-mission-panel-body');
  panel.append(header,body);
  document.body.append(panel);

  var toggle=el('button','aegis-mission-toggle');
  toggle.type='button';
  toggle.append(el('span','','CASE DOSSIER'));
  count=el('small','','0/0');
  toggle.append(count);
  toggle.onclick=function(){
    panel.hidden=!panel.hidden;
    if(!panel.hidden)render();
  };
  document.body.append(toggle);

  buildAiDialoguePopup();
}

function buildAiDialoguePopup(){
  copilotPopup = el('div', 'aegis-copilot-popup');
  copilotPopup.hidden = true;

  var header = el('div', 'copilot-header');
  var brand = el('div', 'copilot-brand');
  brand.append(el('span', 'copilot-icon', '🛡️'), el('h3', 'copilot-title', 'AEGIS MIND // COPILOT'));

  var actions = el('div', 'copilot-actions');
  var btnMin = el('button', 'btn-copilot-min', '—');
  btnMin.type = 'button';
  var btnClose = el('button', 'btn-copilot-close', '✕');
  btnClose.type = 'button';

  actions.append(btnMin, btnClose);
  header.append(brand, actions);

  copilotBody = el('div', 'copilot-body');
  copilotResponses = el('div', 'copilot-responses');

  copilotPopup.append(header, copilotBody, copilotResponses);
  document.body.append(copilotPopup);

  copilotOrb = el('button', 'copilot-orb hidden');
  copilotOrb.type = 'button';
  copilotOrb.append(el('span', 'orb-inner', '🛡️'));
  copilotBadge = el('span', 'orb-badge hidden', '1');
  copilotOrb.append(copilotBadge);
  document.body.append(copilotOrb);

  btnMin.onclick = function(){
    copilotPopup.classList.add('is-minimized');
    copilotPopup.hidden = true;
    copilotOrb.classList.remove('hidden');
    copilotBadge.classList.remove('hidden');
  };

  copilotOrb.onclick = function(){
    copilotPopup.hidden = false;
    copilotPopup.classList.remove('is-minimized');
    copilotOrb.classList.add('hidden');
    copilotBadge.classList.add('hidden');
  };

  btnClose.onclick = function(){
    copilotPopup.hidden = true;
    copilotOrb.classList.add('hidden');
  };
}

function showAiDialogue(title, text, options){
  if(!copilotPopup) buildAiDialoguePopup();
  var titleEl = copilotPopup.querySelector('.copilot-title');
  if(titleEl) titleEl.textContent = 'AEGIS MIND // ' + (title || 'TACTICAL COPILOT');

  copilotBody.innerHTML = '';
  copilotResponses.innerHTML = '';

  var msg = el('div', 'copilot-msg ai-msg');
  msg.append(el('span', 'sender-tag', 'AEGIS MIND'), el('p', '', text));
  copilotBody.append(msg);

  if(Array.isArray(options)){
    options.forEach(function(opt){
      var btn = el('button', 'btn-copilot-option', opt.q);
      btn.type = 'button';
      btn.onclick = function(){
        handleCopilotOptionChoice(opt);
      };
      copilotResponses.append(btn);
    });
  }

  copilotPopup.hidden = false;
  copilotPopup.classList.remove('is-minimized');
  if(copilotOrb) copilotOrb.classList.add('hidden');
  playSfx('clue');
}

function handleCopilotOptionChoice(opt){
  var userMsg = el('div', 'copilot-msg user-msg');
  userMsg.append(el('span', 'sender-tag', 'OPERATOR'), el('p', '', opt.q));
  copilotBody.append(userMsg);

  copilotResponses.innerHTML = '';

  setTimeout(function(){
    var reply = el('div', 'copilot-msg ai-msg');
    reply.append(el('span', 'sender-tag', 'AEGIS MIND'), el('p', '', opt.a));
    copilotBody.append(reply);
    copilotBody.scrollTop = copilotBody.scrollHeight;
    playSfx('clue');
  }, 400);
}

function openTutorial(){
  var ov = document.getElementById('aegis-tutorial-overlay');
  if(ov) ov.classList.remove('hidden');
}

window.addEventListener('keydown',function(e){if(e.key==='F4'){e.preventDefault();panel.hidden=!panel.hidden;if(!panel.hidden)render();}});
window.addEventListener('aegis:terminal-command',function(e){var c=e.detail&&e.detail.command||'';observe('terminal',c);});
document.addEventListener('click',function(e){var b=e.target.closest('[data-aegis-action]');if(b)observe('click',b.dataset.aegisAction);});
document.addEventListener('change',function(e){if(e.target.matches('.rule-toggle')){if(e.target.dataset.id==='7')observe('change','shield-rule');if(e.target.dataset.id==='8')observe('change','shield-rate-policy');}});
window.addEventListener('aegis:wm-open',function(e){observe('open',e.detail&&e.detail.app||'');});

build();

document.addEventListener('aegis:authenticated',async function(e){
  authenticated=true;
  syncing=true;
  var user = (e && e.detail && e.detail.user) || (window.AegisAuth && window.AegisAuth.user) || { username: 'Operator' };
  try{
    var response=await fetch('/api/campaign/state',{credentials:'same-origin'});
    if(response.ok){
      var payload=await response.json();
      state=payload.state&&payload.state.v===1?payload.state:fresh();
      ensureProfileState();
    }else state=fresh();
  }catch(err){state=fresh();}
  ensureProfileState();
  syncing=false;
  save();
  panel.hidden=true;
  render();
  if(!state.started&&!state.done.includes(mission().id))start();

  var visitedKey = 'aegis_visited_' + (user.id || user.username);
  var hasVisited = false;
  try { hasVisited = !!localStorage.getItem(visitedKey); } catch(ex){}

  var activeStepIdx = mission().steps.findIndex(function(s, idx) { return Array.isArray(s[3]) && !evidence().includes(idx); });
  var activeStepName = activeStepIdx !== -1 ? mission().steps[activeStepIdx][0] : 'All milestones secured';
  var activeStepClue = activeStepIdx !== -1 ? mission().steps[activeStepIdx][1] : 'Submit your case incident report.';

  if (!hasVisited) {
    try { localStorage.setItem(visitedKey, 'true'); } catch(ex){}
    showAiDialogue('WELCOME, ' + (user.displayName || user.username).toUpperCase(), '👋 Welcome to your AegisOS Virtual Cyber Defense Station! I am Aegis Mind, your tactical case copilot. All microkernel defenses, forensic carvers, and diagnostic consoles are online. Review the Cyber Defense Tutorial before taking on live incidents. Good luck, Operator!', [
      { q: 'Start Cyber Defense Tutorial', a: 'Opening tactical briefing...' },
      { q: 'Open Case Dossier (F4)', a: 'Case operations dossier initialized.' },
      { q: 'Ready for duty', a: 'Standing by.' }
    ]);
  } else {
    showAiDialogue('WELCOME BACK, ' + (user.displayName || user.username).toUpperCase(), '👋 Welcome back, Operator! Let\\'s continue where you had left off. Active theater: Case #' + (state.index + 1) + ' (' + mission().title + '). Current priority objective: ' + activeStepName + '. ⚠️ URGENT: System telemetry is live—solve pending diagnostics swiftly!', [
      { q: 'Show active objective details', a: activeStepClue },
      { q: 'Open Operator Case Notes', a: 'Opening Case Notes...' },
      { q: 'Review Defense Tutorial', a: 'Opening tutorial...' }
    ]);
  }
});

window.AegisCampaign={
  open:function(){panel.hidden=false;render();},
  openAchievements:function(){currentTab='achievements';panel.hidden=false;render();},
  openDrills:function(){currentTab='drills';panel.hidden=false;render();},
  triggerTimedAttack:triggerTimedAttack,
  unlockAchievement:unlockAchievement,
  openCopilot:function(){if(copilotPopup){copilotPopup.hidden=false;copilotPopup.classList.remove('is-minimized');if(copilotOrb)copilotOrb.hidden=true;}},
  openTutorial: openTutorial,
  triggerCriticalBreach: triggerCriticalBreachCountdown
};

})();
`;

fs.writeFileSync(targetPath, code, 'utf8');
console.log('Successfully generated complete native-campaign.js with 8 operations and multi-stage steps');
