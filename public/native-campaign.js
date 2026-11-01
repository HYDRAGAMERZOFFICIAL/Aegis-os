/* Native AegisOS campaign: event-driven missions embedded in the original desktop. */
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
      text: "🚨 SECURITY INCIDENT ALERT // CASE #01 ASSIGNED: At 02:13, an unauthorized service restart triggered low-volume encrypted egress on research subnet 4. The initial alert may be a decoy—do not shut down the host blindly. Open Neural Monitor or Terminal to establish a process baseline and identify anomalous parent-child relationships. Good luck, Investigator!",
      options: [
        { q: "Where should I look in Neural Monitor?", a: "In Neural Monitor, expand the process tree. Look for a worker whose parent PID does not match its declared service role. Click on the row to inspect process details!" },
        { q: "Can I use Terminal commands instead?", a: "Yes! Running `ps` in Terminal will expose the process ancestry, and `netstat` will reveal active sockets. Both will register forensic evidence." },
        { q: "Understood, investigating now.", a: "Stay sharp. Report back as soon as you find the masquerading process." }
      ]
    },
    steps: [
      {
        text: "Step 1 Secured! You've inspected the process baseline and confirmed the anomalous ancestry. Now we need socket telemetry. Open Terminal (`aegis_shell`) and run `ps` and `netstat` to correlate the PID with active network sockets.",
        options: [
          { q: "Which terminal commands are required?", a: "Run `ps` and `netstat`. You can also run `ss` and `journalctl` for extra forensic confirmation!" },
          { q: "What should I look for in the socket table?", a: "Look for foreign TCP connections on high ports originating from the suspicious worker process." },
          { q: "Opening Terminal now.", a: "Record the remote destination IP as soon as you see it!" }
        ]
      },
      {
        text: "Step 2 Secured! Command history and active sockets are correlated. Now open CyberSecurity Arsenal. We must inspect packet metadata and analyze repeated beacon intervals to confirm the external C2 channel.",
        options: [
          { q: "How do I inspect packets in CyberSec Arsenal?", a: "Launch CyberSec Arsenal, go to the Packet Sniffer tab, and click any suspicious packet row to inspect its destination headers." },
          { q: "What confirms an active C2 beacon?", a: "Automated beacons fire at consistent intervals to the same external IP address—such as 185.220.101.5!" },
          { q: "Analyzing packet telemetry now.", a: "Verify whether the traffic is encrypted and note the destination port." }
        ]
      },
      {
        text: "Step 3 Secured! Network evidence confirms regular beacons to untrusted relay 185.220.101.5:8443. Now open Files (AegisFS). A staged payload artifact was modified right before the restart. Inspect it—but do NOT delete it!",
        options: [
          { q: "Which file should I inspect?", a: "In Files & Storage Manager, look for `svc-sync-cache.bin` in the filesystem table and click it to inspect its hash and timestamps." },
          { q: "Why shouldn't I delete the suspicious file?", a: "Deleting it destroys cryptographic evidence required for command review! Maintain chain of custody." },
          { q: "Checking Files app now.", a: "Compare the modified timestamp with the 02:13 incident alert time." }
        ]
      },
      {
        text: "Step 4 Secured! Artifact metadata and file hash captured. Now consult me in Aegis Mind AI: ask me to correlate the timeline of events between the file modification, process spawn, and outbound beacon.",
        options: [
          { q: "Correlate timeline between file, process, and beacon.", a: "Timeline correlation complete: 1. Unauthorized service file modification (`svc-sync-cache.bin`) at 02:13. 2. Process spawn with spoofed parent. 3. Outbound beacon initiated to 185.220.101.5:8443!" },
          { q: "What prompt should I give Aegis Mind?", a: "Type 'correlate timeline and root cause' in the Aegis Mind AI chat box!" },
          { q: "Submitting correlation request.", a: "I will synthesize all telemetry points into a definitive event sequence." }
        ]
      },
      {
        text: "Step 5 Secured! The forensic timeline is fully reconstructed! We are now at a strategic decision gate: open your Operations panel and select your containment strategy. Beware: isolating the entire host causes service downtime, whereas targeted egress blocking preserves trusted services.",
        options: [
          { q: "What is the best containment choice?", a: "Targeted egress block! It neutralizes the attack path while keeping legitimate internal services running." },
          { q: "Will isolating the host damage trusted services?", a: "Host isolation contains the threat, but earns a lower operational readiness score due to collateral disruption." },
          { q: "Opening Operations panel to make the decision.", a: "Select 'Targeted egress block' when you are ready." }
        ]
      },
      {
        text: "Step 6 Secured! Containment strategy decided. Now implement the defensive policy: open Aegis Shield and activate the rule that denies untrusted ingress and blocks suspicious C2 beacons.",
        options: [
          { q: "Which rule in Aegis Shield should I toggle?", a: "In Aegis Shield, toggle Rule #7 (Untrusted Ingress / C2 Deny policy) to lock down the boundary." },
          { q: "Will this disrupt other network traffic?", a: "No, targeted rules preserve HTTPS, SSH, and trusted internal cluster traffic." },
          { q: "Toggling the shield policy now.", a: "Observe the packet drop counter increase in Shield!" }
        ]
      },
      {
        text: "Step 7 Secured! Defensive firewall policy applied. Now verify containment: run Aegis Guard scan in Shield, then issue `verify-containment` in the Terminal to confirm the hostile socket is dead.",
        options: [
          { q: "Where do I run the Guard scan?", a: "Click 'Run Aegis Guard AI Scan' inside Aegis Shield, then switch to Terminal." },
          { q: "What command verifies containment in Terminal?", a: "Type `verify-containment` in Terminal! It will confirm whether the session is severed." },
          { q: "Executing verification now.", a: "Confirm that zero active packets are reaching the C2 relay." }
        ]
      },
      {
        text: "Step 8 Secured! Containment verified in the running environment. Now validate filesystem integrity: open Merkle Filesystem and run an integrity scan to identify the modified branch.",
        options: [
          { q: "What am I looking for in Merkle Filesystem?", a: "Click 'Run Merkle Hash Scan' or inspect the block cards to locate the branch with hash divergence." },
          { q: "Does the Merkle tree catch unauthorized file edits?", a: "Yes! Every inode modification changes the parent Merkle hashes all the way up to the cryptographic root." },
          { q: "Opening Merkle Engine now.", a: "Identify the diverged block before triggering rollback." }
        ]
      },
      {
        text: "Step 9 Secured! Diverged Merkle branch identified. Now restore the clean snapshot created before the incident (`chkpt-0003`), and run `verify-root` in Terminal to prove ledger trust!",
        options: [
          { q: "Which checkpoint is the safe restore point?", a: "Select `chkpt-0003` (Alpha Restore Point) and click Rollback, then execute `verify-root` in Terminal." },
          { q: "Will rollback affect uncompromised files?", a: "Merkle rollback is surgical and atomic: only tainted blocks are repointed to clean snapshot inodes." },
          { q: "Executing rollback and root verification.", a: "Ensure `verify-root` reports a verified root hash in Terminal!" }
        ]
      },
      {
        text: "Step 10 Secured! Root hash verified and sealed! All 10 investigative milestones are complete! Now open the Incident Report in your Operations panel, answer the deduction questions (`185.220.101.5:8443`, `file > process > beacon`), and submit the sealed dossier to Command!",
        options: [
          { q: "What was the hostile remote endpoint?", a: "The remote endpoint was `185.220.101.5:8443`." },
          { q: "What was the reconstructed event sequence?", a: "The sequence was: `file > process > beacon` (file modification > process spawn > outbound beacon)." },
          { q: "Submitting report to Director Vance!", a: "Excellent work! Seal the case and claim your Grade S command honors!" }
        ]
      }
    ]
  },
  op2: {
    start: {
      text: "🚨 SECURITY INCIDENT ALERT // CASE #02 ASSIGNED: A signed system component (`kernel_sync_worker`) requested privileged capability `CAP_NET_RAW_SOCKET` outside its authorized authority profile. The signature is valid, but the behavior is anomalous! Open Neural Monitor to map the privilege boundary and identify the unexpected ring transition. Good luck, Investigator!",
      options: [
        { q: "What should I look for in Neural Monitor?", a: "In Neural Monitor, review the process memory map and address space events. Look for an unexpected ring-0 privilege transition!" },
        { q: "Is a valid signature safe?", a: "No! An adversary can hijack or repurpose a signed binary. Behavior and capability grants must match least-privilege policy." },
        { q: "Investigating the privilege boundary now.", a: "Be precise: identify the capability requested before modifying authority." }
      ]
    },
    steps: [
      {
        text: "Step 1 Secured! Memory map inspected; anomalous privilege transition identified. Now open Aegis Shield and inspect the Zero-Trust token table (`shield-token-audit`) to find the token with an unexpected grant.",
        options: [
          { q: "Where is the token table in Shield?", a: "In Aegis Shield, switch to the 'Zero-Trust Tokens' tab to audit all issued capabilities." },
          { q: "Which token is anomalous?", a: "Look for `TOK-KERNEL-ESCALATED` granted to a component that only needs sync access." },
          { q: "Auditing tokens now.", a: "Confirm which subject process owns the elevated token." }
        ]
      },
      {
        text: "Step 2 Secured! Token table audited. Now switch to Terminal: run `capability-audit` and `ps` to trace the requesting component and correlate the process with its granted token.",
        options: [
          { q: "What terminal command audits capabilities?", a: "Run `capability-audit` in Terminal! It prints the Zero-Trust grant matrix." },
          { q: "Which component is flagged?", a: "`kernel_sync_worker` is flagged for requesting unneeded raw socket capability." },
          { q: "Running capability-audit now.", a: "Observe the finding for `TOK-KERNEL-ESCALATED`." }
        ]
      },
      {
        text: "Step 3 Secured! Requesting component traced to `kernel_sync_worker`. Now inspect the binary provenance in Files (AegisFS): find `libaegis_kernel` and inspect its library metadata.",
        options: [
          { q: "Where is the library located in Files?", a: "In Files & Storage Manager, click on `libaegis_kernel.so` in the files table to inspect its metadata and signature." },
          { q: "What does library inspection show?", a: "It proves the binary signature is authentic, confirming the issue is runtime capability escalation, not a corrupted file." },
          { q: "Inspecting library metadata now.", a: "Capture the provenance hash." }
        ]
      },
      {
        text: "Step 4 Secured! Library provenance verified. Now open Merkle Filesystem and run an integrity scan to check if kernel policy metadata was tampered with.",
        options: [
          { q: "What does the Merkle scan check in Case #02?", a: "It verifies that the core filesystem is healthy and the anomaly is isolated to capability configuration." },
          { q: "Clicking Merkle integrity scan now.", a: "Confirm the baseline root hash." }
        ]
      },
      {
        text: "Step 5 Secured! Merkle baseline verified. Now consult Aegis Mind AI: ask me to correlate the token, process, and integrity evidence to establish the root cause.",
        options: [
          { q: "Correlate kernel root cause.", a: "Root cause correlation: `kernel_sync_worker` was granted `TOK-KERNEL-ESCALATED` exceeding its minimum profile, while binary signature remained valid." },
          { q: "What should I ask Aegis Mind?", a: "Ask 'correlate kernel and token root cause' in the AI chat!" },
          { q: "Requesting AI correlation.", a: "I will synthesize the capability audit trail." }
        ]
      },
      {
        text: "Step 6 Secured! Root cause correlated. Decision Gate: open Operations and select the minimum safe authority change. Revoking every service token destabilizes the system; revoke only the anomalous token!",
        options: [
          { q: "Which decision is safest?", a: "Select 'Revoke anomalous token' to uphold the principle of least privilege without taking down critical services." },
          { q: "Why not revoke all service tokens?", a: "Revoking every token disables trusted security logging and UI compositing." },
          { q: "Locking in least-privilege decision.", a: "Enforce the revocation in your Operations panel." }
        ]
      },
      {
        text: "Step 7 Secured! Command decision recorded. Now verify the kernel patch posture: open CyberSec Arsenal and run the vulnerability scan (`cybersec-vulnerability-scan`) to verify approved patch `AEGIS-KERNEL-2.0.4`.",
        options: [
          { q: "How do I check the patch posture?", a: "Open CyberSec Arsenal and click 'Run Heuristic Vulnerability Audit' to confirm patch build `AEGIS-KERNEL-2.0.4`." },
          { q: "What patch version is approved?", a: "Approved training patch build: `AEGIS-KERNEL-2.0.4`." },
          { q: "Running vulnerability scan now.", a: "Verify the zero-trust microkernel patch status." }
        ]
      },
      {
        text: "Step 8 Secured! Patch posture verified. Now quarantine the anomalous component: run an Aegis Guard scan in Shield, then issue `quarantine-component` in Terminal to isolate `kernel_sync_worker`.",
        options: [
          { q: "What terminal command isolates the component?", a: "Issue `quarantine-component` in Terminal right after running the Guard scan." },
          { q: "Will this affect host processes?", a: "No, quarantine is isolated within the simulated Aegis namespace." },
          { q: "Executing quarantine now.", a: "Confirm privileged capabilities are stripped." }
        ]
      },
      {
        text: "Step 9 Secured! Component quarantined. Now prove kernel trust: re-run Merkle integrity scan and issue `verify-root` in Terminal to confirm the root matches `AEGIS-ROOT-VERIFIED`.",
        options: [
          { q: "What command proves kernel trust?", a: "Run `verify-root` in Terminal after scanning Merkle integrity." },
          { q: "What root hash should appear?", a: "The root hash will report `AEGIS-ROOT-VERIFIED`." },
          { q: "Running verify-root now.", a: "Seal the proof in the audit ledger." }
        ]
      },
      {
        text: "Step 10 Secured! Kernel trust proven and sealed! Now open the Incident Analysis in Operations: answer the deduction quiz (`TOK-KERNEL-ESCALATED`, `kernel_sync_worker`) and submit to Command!",
        options: [
          { q: "Which anomalous token was revoked?", a: "The anomalous token was `TOK-KERNEL-ESCALATED`." },
          { q: "Which component requested the capability?", a: "The requesting component was `kernel_sync_worker`." },
          { q: "Submitting Case #02 analysis!", a: "Claim your Grade S honors, Investigator!" }
        ]
      }
    ]
  },
  op3: {
    start: {
      text: "🚨 SECURITY INCIDENT ALERT // CASE #03 ASSIGNED: Public API gateway is degrading under a severe distributed traffic surge! But priority service traffic is also getting delayed. A blunt global deny will lock out legitimate users. Open Task Optimizer to establish service impact across CPU, queue depth, and network. Good luck, Investigator!",
      options: [
        { q: "What should I look for in Task Optimizer?", a: "In Task Optimizer, click the Resource Audit or metric gauges to check queue depth and network bandwidth saturation." },
        { q: "Why not block all inbound traffic immediately?", a: "A total block guarantees an outage for our users! We must differentiate legitimate priority traffic." },
        { q: "Opening Task Optimizer now.", a: "Establish whether CPU or network queue is the bottleneck." }
      ]
    },
    steps: [
      {
        text: "Step 1 Secured! Resource audit complete: public gateway queue is elevated, but CPU is stable. Now open CyberSec Arsenal and inspect packet distribution to analyze source diversity and target ports.",
        options: [
          { q: "What packet pattern should I look for?", a: "In CyberSec Arsenal, check the Packet Sniffer table: look for distributed sources hitting the public gateway port." },
          { q: "Is it a single IP or distributed?", a: "It is a distributed asymmetric flood targeting the gateway endpoint." },
          { q: "Analyzing packet distribution now.", a: "Capture packet rate telemetry." }
        ]
      },
      {
        text: "Step 2 Secured! Packet distribution mapped. Now switch to Terminal: run `netstat` and `service-health` to map service dependencies and identify which route is degraded.",
        options: [
          { q: "What terminal commands map service health?", a: "Run `netstat` and `service-health` in Terminal!" },
          { q: "What does service-health show?", a: "It shows public API gateway is DEGRADED, while priority edge-02 is HEALTHY." },
          { q: "Running service-health now.", a: "Record the healthy edge route name." }
        ]
      },
      {
        text: "Step 3 Secured! Service health matrix captured. Now ask Aegis Mind AI to compare trusted client patterns with the flood and locate the legitimate traffic signature.",
        options: [
          { q: "Ask Aegis Mind to correlate traffic signature.", a: "Traffic correlation: Public gateway is saturated on edge-route-01. Resilient route edge-route-02 has reserved capacity and low queue depth." },
          { q: "What prompt should I use?", a: "Type 'correlate traffic signature and resilient route' in Aegis Mind AI!" },
          { q: "Requesting traffic correlation.", a: "I will identify the priority service path." }
        ]
      },
      {
        text: "Step 4 Secured! Priority traffic signature isolated. Now open Aegis Shield and inspect edge ingress rules (`shield-edge-audit`) to evaluate current rate policies.",
        options: [
          { q: "Where are edge policies in Shield?", a: "In Aegis Shield, switch to the Edge/Firewall rules tab and inspect Rule #8 (Rate Limiting)." },
          { q: "What is the goal of edge policy inspection?", a: "Verify whether the current rule is unbounded or dropping legitimate packets." },
          { q: "Inspecting edge policy in Shield.", a: "Prepare for adaptive rate control." }
        ]
      },
      {
        text: "Step 5 Secured! Edge policy audited. Decision Gate: open Operations and choose your mitigation profile: Adaptive ingress shaping, Global block, or No control.",
        options: [
          { q: "Which mitigation profile preserves priority traffic?", a: "Adaptive ingress shaping! It caps the flood while reserving priority ingress queues." },
          { q: "What happens if I choose global ingress block?", a: "A global deny shuts down the flood, but results in a severe user outage and lower grade." },
          { q: "Selecting Adaptive ingress shaping.", a: "Lock in the strategic choice in Operations." }
        ]
      },
      {
        text: "Step 6 Secured! Mitigation profile selected. Now enforce rate controls: toggle Rule #8 in Aegis Shield, then inspect CyberSec Arsenal packets to observe resulting packet flow.",
        options: [
          { q: "How do I apply the rate policy in Shield?", a: "Toggle Rule #8 (Adaptive Rate Shaping) in Aegis Shield, then click CyberSec Arsenal packet sniffer." },
          { q: "What should I observe in CyberSec Arsenal?", a: "Packets will drop to sustainable rate limits with priority packets flowing through." },
          { q: "Applying rate control and observing packets.", a: "Verify the traffic stabilization." }
        ]
      },
      {
        text: "Step 7 Secured! Rate controls active and verified. Now reroute critical services: run `route-health` in Terminal, compare edge routes, then issue `route-select edge-route-02`!",
        options: [
          { q: "What is the command to inspect routes?", a: "Type `route-health` in Terminal to compare queue saturation." },
          { q: "What is the exact reroute command?", a: "Issue `route-select edge-route-02` in Terminal to switch the priority path." },
          { q: "Executing route selection now.", a: "Confirm route change accepted message." }
        ]
      },
      {
        text: "Step 8 Secured! Critical service rerouted to `edge-route-02`. Now verify service recovery: run `service-health` in Terminal and check Task Optimizer resource telemetry.",
        options: [
          { q: "How do I verify recovery in Terminal?", a: "Run `service-health` in Terminal: gateway health will return to operational." },
          { q: "What should Task Optimizer show?", a: "Click Resource Audit in Task Optimizer: latency and queue depths will settle into the green zone." },
          { q: "Verifying recovery telemetry now.", a: "Confirm gateway queue is cleared." }
        ]
      },
      {
        text: "Step 9 Secured! Gateway recovery verified. Now run the final security audit: recheck edge policy in Shield and run a Merkle integrity scan to confirm no collateral disruption occurred.",
        options: [
          { q: "What is the final audit check?", a: "Click Edge Audit in Shield and Merkle Integrity Scan to prove the system is healthy and secure." },
          { q: "Are all services operational?", a: "Yes, availability is restored and zero-trust boundaries intact." },
          { q: "Executing final security audit.", a: "Ready for post-mortem submission." }
        ]
      },
      {
        text: "Step 10 Secured! Final security audit complete! Now open the Recovery Post-Mortem in Operations: answer the deduction quiz (`edge-route-01` degraded, `edge-route-02` healthy) and submit to Command!",
        options: [
          { q: "Which route was degraded before mitigation?", a: "The degraded route was `edge-route-01`." },
          { q: "Which route carries the critical service now?", a: "The healthy priority route is `edge-route-02`." },
          { q: "Submitting Case #03 Post-Mortem!", a: "Claim your campaign completion honors, Investigator!" }
        ]
      }
    ]
  }
};

var ACHIEVEMENTS = [
  { id:'first_breach', title:'First Line of Defense', icon:'🛡️', desc:'Contain your first security breach and record forensic proof.', xp:100, credits:150 },
  { id:'timed_hero', title:'Crisis Responder', icon:'⏱️', desc:'Neutralize an active time-critical attack before system expiration!', xp:250, credits:300 },
  { id:'rapid_triage', title:'Sub-60s SecOps', icon:'⚡', desc:'Neutralize an active security breach in under 60 seconds.', xp:200, credits:200 },
  { id:'zero_trust_elite', title:'Zero-Trust Sentinel', icon:'🔐', desc:'Revoke an unauthorized capability token without disrupting services.', xp:150, credits:200 },
  { id:'merkle_savior', title:'Merkle Restorer', icon:'🧬', desc:'Perform atomic Merkle rollback and verify root hash integrity.', xp:150, credits:200 },
  { id:'packet_hunter', title:'Traffic Forensics', icon:'📡', desc:'Correlate external hostile C2 beacon relay from packet telemetry.', xp:150, credits:200 },
  { id:'forensic_genius', title:'Forensic Deduction Master', icon:'🧠', desc:'Submit a case incident report with 100% correct deduction answers.', xp:200, credits:250 },
  { id:'terminal_warrior', title:'Ring-0 Operator', icon:'💻', desc:'Execute at least 8 distinct forensic and defense commands in Terminal.', xp:100, credits:150 },
  { id:'ghost_protocol', title:'Ephemeral Airgap', icon:'🌐', desc:'Browse securely in Quantum Ghost Browser and shred the container on exit.', xp:100, credits:150 },
  { id:'crypt_inode', title:'Cryptographic Custody', icon:'📁', desc:'Create, edit, or AES-256 encrypt a critical evidence document in AegisFS.', xp:100, credits:150 },
  { id:'director_commendation', title:'Grade S Commander', icon:'🏆', desc:'Secure any operational case with the highest possible Grade S rank.', xp:300, credits:500 },
  { id:'citadel_sealed', title:'Cyber Fortress Vanguard', icon:'👑', desc:'Complete all 3 operational cases and repel the advanced persistent threat.', xp:500, credits:1000 }
];

var TIMED_SCENARIOS = [
  {
    id: 'emg-worm',
    title: 'CRITICAL ZERO-DAY RANSOMWARE EXFILTRATION',
    threatType: 'Ransomware Worm Payload',
    seconds: 75,
    brief: 'Ransomware worm payload has infiltrated research subnet 4! Memory integrity is dropping and exfiltration beacons have begun. Neutralize the hostile process or sever ingress before the clock runs out!',
    defusalActions: ['change:shield-rule', 'click:shield-guard-scan', 'terminal:verify-containment', 'terminal:quarantine-component'],
    promptClue: 'Enforce Shield firewall deny rule, run Guard Heuristic scan, or issue `quarantine-component` in Terminal!'
  },
  {
    id: 'emg-ddos',
    title: 'VOLUMETRIC SYN FLOOD SATURATION',
    threatType: 'Asymmetric Edge Flood',
    seconds: 60,
    brief: 'Volumetric packet flood hitting edge-route-01! Public gateway is approaching complete queue lockup. Deploy adaptive rate shaping in Shield or issue `route-select edge-route-02` before gateway drops!',
    defusalActions: ['change:shield-rate-policy', 'terminal:route-select'],
    promptClue: 'Toggle adaptive rate shaping in Shield or issue `route-select edge-route-02` in Terminal!'
  },
  {
    id: 'emg-merkle',
    title: 'STEALTH ROOTKIT CORRUPTION ATTACK',
    threatType: 'Kernel Inode Tampering',
    seconds: 80,
    brief: 'Adversary injected corrupted inode into root filesystem block 4! Execute atomic Merkle rollback to clean restore point before system integrity collapses!',
    defusalActions: ['click:merkle-rollback', 'terminal:verify-root'],
    promptClue: 'Open Merkle Filesystem and click Rollback Checkpoint, or run `verify-root` in Terminal!'
  }
];

var missions=[
{id:'op1',title:'THE SILENT INTRUSION',brief:'At 02:13 an unauthorised service restart was followed by a low-volume encrypted session. The first alert may be a decoy. Establish what happened before containment.',pressure:'Containment can destroy volatile evidence. Separate the loudest signal from the root cause.',steps:[
['Establish a process baseline','Inspect Neural Monitor. Compare process ancestry and identify an anomalous parent-child relationship.','Look for a process whose parent does not match its declared service role.',['open:neural','click:neural-process-detail']],
['Interrogate command history','Use Terminal to inspect process and socket state. Record the suspicious PID and parent.','Run `ps` and `netstat`; compare the process tree with active sockets. `ss` and `journalctl` are optional extra checks.',['terminal:ps|terminal:netstat|terminal:ss|terminal:journalctl']],
['Correlate network telemetry','Inspect packet metadata in CyberSecurity Arsenal. Compare repeated destinations and timing.','One packet is weak evidence. Correlate destination, port and interval.',['open:cybersec','click:cybersec-packet-inspect']],
['Inspect the staged artefact','Inspect the suspicious service file and its metadata in Files. Do not delete it.','Compare modified time, owner, permissions and hash.',['open:files','click:files-inspect-suspicious']],
['Reconstruct the timeline','Ask Aegis Mind to correlate process, file and network observations.','File modification should precede process spawn, then outbound beacon.',['open:ai','click:ai-correlate-timeline']],
['Choose containment strategy','Decide whether to block the destination, isolate the host or continue observing.','Consider evidence preservation and blast radius. A broad shutdown harms trusted services.','choice',['Targeted egress block','Isolate the host','Continue observation']],
['Apply defensive policy','Use Aegis Shield to implement a narrow policy consistent with your evidence.','Block only the confirmed hostile destination. Preserve unrelated trusted traffic.',['open:shield','change:shield-rule']],
['Verify containment','Run Aegis Guard and confirm the suspicious process is no longer communicating.','Run `verify-containment` after the Guard scan; the terminal output must confirm the session is gone.',['click:shield-guard-scan','terminal:verify-containment']],
['Validate filesystem integrity','Inspect Merkle filesystem and compare the affected branch with a trusted checkpoint.','Identify the changed branch before restoring anything.',['open:merkle','click:merkle-integrity-scan']],
['Recover without erasing the trail','Restore the verified checkpoint and verify the root again.','Choose a checkpoint created before the suspicious service modification.',['click:merkle-rollback','terminal:verify-root']],
['Submit incident report','Submit the timeline, root cause, containment choice and recovery proof.','All evidence categories and a defensible decision are required.','report']
],choices:['Targeted egress block','Isolate the host','Continue observation'],outcomes:['Evidence-preserving containment. Host services remain available.','Threat contained, but service interruption reduced readiness.','Delayed containment allowed additional simulated exposure.'],quiz:[{id:'endpoint',question:'Which remote endpoint was contacted by the suspicious worker?',placeholder:'IP:PORT',accept:['185.220.101.5:8443'],hint:'Compare the svc-sync-cache artefact with repeated outbound packet destinations.'},{id:'sequence',question:'What is the most likely event sequence?',placeholder:'file > process > beacon',accept:['file>process>beacon','file modification>process spawn>outbound beacon'],hint:'Reconstruct the timeline from modified artefact, process ancestry and outbound beacon.'}]},
{id:'op2',title:'GHOST IN THE KERNEL',brief:'A signed system component requested a capability it has never needed. The signature is valid; the behaviour is not. Determine whether a trusted process has been repurposed.',pressure:'Revoking every privileged token may destabilise the desktop. Identify the exact authority boundary crossed.',steps:[
['Map the privilege boundary','Inspect Neural Monitor memory and process context. Identify the unexpected ring transition.','Compare declared privilege with the capability actually requested.',['open:neural','click:neural-memory-map']],
['Audit capability grants','Inspect the Zero-Trust token table in Aegis Shield.','Find a token whose subject and capability do not match its normal role.',['open:shield','click:shield-token-audit']],
['Trace the requesting component','Use Terminal to inspect process ownership and capability references.','Run `ps` and `capability-audit`; compare the requesting component with its granted token.',['terminal:capability-audit|terminal:ps']],
['Verify binary provenance','Inspect the system library and metadata in Files.','A valid signature does not prove the running component is behaving normally.',['open:files','click:files-inspect-library']],
['Compare trusted kernel baseline','Run a Merkle integrity scan and identify the branch that differs.','Look for a mismatch limited to component or policy metadata.',['open:merkle','click:merkle-integrity-scan']],
['Correlate the root cause','Ask Aegis Mind to correlate token, process and integrity evidence.','The best explanation accounts for all three sources.',['open:ai','click:ai-correlate-kernel']],
['Select minimum safe authority change','Choose the narrowest action that stops escalation without disabling security services.','Revoke the anomalous capability, not every token owned by the service.','choice',['Revoke anomalous token','Revoke every service token','Leave privileges unchanged']],
['Verify the kernel patch posture','Run the vulnerability scan and confirm the approved training patch is present.','Compare the reported kernel build with the approved training baseline.',['click:cybersec-vulnerability-scan']],
['Quarantine anomalous component','Run Aegis Guard and isolate the process after the authority change.','Run `quarantine-component` after the Guard scan; it is a simulated Aegis training command, not a host command.',['click:shield-guard-scan','terminal:quarantine-component']],
['Prove kernel trust','Re-run integrity verification and compare the resulting root.','Run `verify-root` after the Merkle check; compare the displayed root with the trusted checkpoint.',['click:merkle-integrity-scan','terminal:verify-root']],
['File kernel incident analysis','Submit root cause, affected capability, patch provenance and verification result.','A correct action with an incomplete audit trail earns a lower grade.','report']
],choices:['Revoke anomalous token','Revoke every service token','Leave privileges unchanged'],outcomes:['Least-privilege response accepted. Unrelated services remain operational.','Escalation stopped, but broad revocation disrupted trusted capabilities.','Kernel remains exposed. Command records a serious operational failure.'],quiz:[{id:'token',question:'Which anomalous token exceeded the component authority profile?',placeholder:'TOK-...',accept:['TOK-KERNEL-ESCALATED'],hint:'Read the capability audit output and compare the anomalous grant.'},{id:'component',question:'Which component requested the unexpected capability?',placeholder:'component_name',accept:['kernel_sync_worker'],hint:'Correlate the token audit with the requesting process in the terminal output.'}]},
{id:'op3',title:'BLACKOUT PROTOCOL',brief:'The public gateway is degrading under a traffic surge. The first dashboard suggests a flood, but priority traffic is also delayed. Determine whether traffic is uniform, distributed or selectively shaped.',pressure:'A blunt rate limit can protect the gateway while locking out legitimate users. Preserve the critical path.',steps:[
['Establish service impact','Inspect Task Optimizer: CPU, memory, queue depth and network utilisation.','High ingress does not automatically mean CPU exhaustion.',['open:optimizer','click:optimizer-resource-audit']],
['Inspect packet distribution','Compare source diversity, destination ports and packet timing.','A distributed source set with a narrow target differs from a simple port scan.',['open:cybersec','click:cybersec-packet-inspect']],
['Map service dependencies','Use Terminal to inspect listeners, routes and service health.','Run `netstat` and `service-health`; identify the degraded gateway and healthy priority edge.',['open:terminal','terminal:netstat','terminal:service-health']],
['Find legitimate traffic signature','Ask Aegis Mind to compare trusted client patterns with anomalous traffic.','Protect the known-good service path before broad controls.',['open:ai','click:ai-correlate-traffic']],
['Inspect edge policy','Inspect ingress rules and trusted route health in Aegis Shield.','A valid rule can still be operationally harmful.',['open:shield','click:shield-edge-audit']],
['Choose mitigation profile','Balance availability, false positives and containment speed.','Adaptive shaping is safer than global deny when priority traffic is preserved.','choice',['Adaptive ingress shaping','Global ingress block','No rate control']],
['Apply and observe rate controls','Apply the profile and inspect resulting packet flow.','Do not assume mitigation worked. Observe queue and packet behaviour.',['change:shield-rate-policy','click:cybersec-packet-inspect']],
['Reroute critical service','Use Terminal to inspect route health and select the resilient edge path.','Run `route-health`, compare edge-01/02/03, then issue `route-select edge-route-02` only after checking the priority path.',['terminal:route-health','terminal:route-select']],
['Verify service recovery','Use Terminal and Task Optimizer to verify gateway health and resource recovery.','Run `service-health` again and inspect Task Optimizer resource telemetry after rerouting.',['terminal:service-health','click:optimizer-resource-audit']],
['Run final security audit','Recheck edge policy and confirm no unrelated service was disabled.','The final state should be secure and usable, not merely quiet.',['click:shield-edge-audit','click:merkle-integrity-scan']],
['Submit recovery post-mortem','Record analysis, mitigation profile, route choice, service impact and residual risk.','Explain the trade-off and why it was appropriate.','report']
],choices:['Adaptive ingress shaping','Global ingress block','No rate control'],outcomes:['Adaptive shaping preserved the critical path while reducing the flood.','Gateway recovered, but global block caused a severe legitimate-user outage.','Service degradation continued. Availability was not restored in the operation window.'],quiz:[{id:'degraded',question:'Which edge route was degraded before mitigation?',placeholder:'edge-route-..',accept:['edge-route-01'],hint:'Compare the route-health table before selecting the healthy edge.'},{id:'healthy',question:'Which route should carry the critical service after rerouting?',placeholder:'edge-route-..',accept:['edge-route-02'],hint:'The route-health output shows which edge has priority capacity and low queue depth.'}]}
];
function fresh(){return {v:1,index:0,started:false,done:[],evidence:{},fragments:{},decision:{},answers:{},stats:{xp:0,credits:0,research:0,score:0,history:[]},achievements:[],log:[]};}
var state=fresh(),authenticated=false,syncing=false,saveTimer=null;
if(!state||state.v!==1)state=fresh();
function save(){if(!authenticated)return;try{localStorage.setItem(KEY,JSON.stringify(state));}catch(e){}if(saveTimer)clearTimeout(saveTimer);saveTimer=setTimeout(function(){fetch('/api/campaign/state',{method:'POST',credentials:'same-origin',headers:{'Content-Type':'application/json'},body:JSON.stringify({state:state})}).catch(function(){});},250);}
function mission(){return missions[Math.min(state.index,missions.length-1)];}
function evidence(){var m=mission();return state.evidence[m.id]||(state.evidence[m.id]=[]);}
function fragments(){var m=mission();if(!state.fragments)state.fragments={};return state.fragments[m.id]||(state.fragments[m.id]={});}
function answersCorrect(m){var values=state.answers&&state.answers[m.id]||{};return !m.quiz||m.quiz.every(function(q){var value=String(values[q.id]||'').toLowerCase().replace(/[^a-z0-9]+/g,'');return q.accept.some(function(a){return value===a.toLowerCase().replace(/[^a-z0-9]+/g,'');});});}
function ensureProfileState(){if(!state.answers)state.answers={};if(!state.stats)state.stats={xp:0,credits:0,research:0,score:0,history:[]};if(!Array.isArray(state.stats.history))state.stats.history=[];if(!state.fragments)state.fragments={};if(!Array.isArray(state.achievements))state.achievements=[];}
function record(s){state.log.push({id:mission().id,t:new Date().toLocaleTimeString(),s:s});if(state.log.length>200)state.log=state.log.slice(-200);save();}
function el(tag,cls,txt){var n=document.createElement(tag);if(cls)n.className=cls;if(txt!==undefined)n.textContent=txt;return n;}
var panel,body,count,sfxBtn;
var copilotPopup,copilotBody,copilotResponses,copilotOrb,copilotBadge;

function toast(title,desc,grade){if(window.AegisSystemNotify){window.AegisSystemNotify(title,desc,'info');return;}var n=el('div','aegis-native-toast');var head=el('b','',title);if(grade){var badge=el('span','mission-grade-badge grade-'+grade.toLowerCase(),'GRADE '+grade);head.append(badge);}n.append(head,el('span','',desc));document.body.append(n);setTimeout(function(){n.remove();},5000);}

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
  var badge=el('span','mission-grade-badge grade-s','+'+ach.xp+' XP');
  head.append(badge);
  n.append(head,el('span','',ach.desc+' · +'+ach.credits+' Credits'));
  document.body.append(n);
  setTimeout(function(){n.remove();},6000);
}

var activeEmergency=null,emergencyInterval=null,currentTab='cases',tabNav;
var uniqueTerminalCommands=new Set();

function triggerTimedAttack(scenarioId){
  if(activeEmergency)return;
  var chosen=TIMED_SCENARIOS.find(function(s){return s.id===scenarioId;})||TIMED_SCENARIOS[Math.floor(Math.random()*TIMED_SCENARIOS.length)];
  activeEmergency={
    id:chosen.id,
    title:chosen.title,
    threatType:chosen.threatType,
    seconds:chosen.seconds,
    secondsLeft:chosen.seconds,
    brief:chosen.brief,
    defusalActions:chosen.defusalActions,
    promptClue:chosen.promptClue,
    startedAt:Date.now()
  };
  playSfx('emergency');
  toast('🚨 EMERGENCY BREACH IN PROGRESS',chosen.title+' — '+chosen.seconds+'s to failure!','D');
  showAiDialogue('EMERGENCY ATTACK ALERT // '+chosen.threatType,'🚨 EMERGENCY BREACH DETECTED! '+chosen.brief+' '+chosen.promptClue+' Good luck, Investigator!',[
    { q:'How do I neutralize this immediately?', a:chosen.promptClue },
    { q:'Can I use terminal commands?', a:'Yes! Running `quarantine-component`, `verify-containment`, or `route-select edge-route-02` directly counters the attack.' },
    { q:'Deploying defensive countermeasures now!', a:'Hurry! Watch the emergency countdown timer.' }
  ]);
  renderEmergencyBanner();
  if(emergencyInterval)clearInterval(emergencyInterval);
  emergencyInterval=setInterval(function(){
    if(!activeEmergency){clearInterval(emergencyInterval);return;}
    activeEmergency.secondsLeft--;
    updateEmergencyBanner();
    if(activeEmergency.secondsLeft<=0){
      clearInterval(emergencyInterval);
      emergencyTimeOut();
    }
  },1000);
}

function renderEmergencyBanner(){
  var b=document.getElementById('aegis-emergency-banner');
  if(!b){
    b=el('div','');
    b.id='aegis-emergency-banner';
    document.body.append(b);
  }
  updateEmergencyBanner();
}

function updateEmergencyBanner(){
  var b=document.getElementById('aegis-emergency-banner');
  if(!b||!activeEmergency)return;
  b.replaceChildren();
  var content=el('div','emergency-content');
  content.append(
    el('span','emergency-kicker','⚠️ LIVE EMERGENCY BREACH IN PROGRESS'),
    el('span','emergency-title',activeEmergency.title)
  );
  var timerBox=el('div','emergency-timer-box');
  var m=Math.floor(activeEmergency.secondsLeft/60);
  var s=activeEmergency.secondsLeft%60;
  var timeStr=String(m).padStart(2,'0')+':'+String(s).padStart(2,'0');
  timerBox.append(el('span','','⏱️'),el('span','emergency-clock',timeStr));
  var actionBtn=el('button','emergency-action-btn','DISARM // DEFEND');
  actionBtn.type='button';
  actionBtn.addEventListener('click',function(){
    if(window.AegisCampaign&&window.AegisCampaign.openCopilot){window.AegisCampaign.openCopilot();}
  });
  b.append(content,timerBox,actionBtn);
}

function removeEmergencyBanner(){
  var b=document.getElementById('aegis-emergency-banner');
  if(b)b.remove();
}

function emergencyTimeOut(){
  playSfx('alert');
  toast('CRISIS BREACH SUSTAINED','The emergency attack timer expired before mitigation was completed. Corrupted block detected.','D');
  showAiDialogue('EMERGENCY BREACH SUSTAINED','The timer expired before countermeasures were deployed! Adversary managed to compromise filesystem state. Open Merkle Filesystem and execute an atomic rollback to restore clean integrity.',[
    { q:'Initiating emergency rollback now.', a:'Click Rollback Checkpoint in Merkle Filesystem or run `verify-root` in Terminal.' }
  ]);
  activeEmergency=null;
  removeEmergencyBanner();
  render();
}

function updateTabNav(){
  if(!tabNav)return;
  var tabs=tabNav.querySelectorAll('.aegis-dossier-tab');
  if(tabs.length===3){
    tabs[0].className='aegis-dossier-tab'+(currentTab==='cases'?' active':'');
    tabs[1].className='aegis-dossier-tab'+(currentTab==='achievements'?' active':'');
    tabs[1].textContent='🏅 MEDALS ('+(state.achievements||[]).length+'/'+ACHIEVEMENTS.length+')';
    tabs[2].className='aegis-dossier-tab'+(currentTab==='drills'?' active':'');
  }
}

function renderAchievementsView(){
  ensureProfileState();
  var unlocked=state.achievements||[];
  var summary=el('div','aegis-achievement-summary');
  summary.append(
    el('h3','','OPERATOR MEDALS & ACHIEVEMENTS // '+unlocked.length+' / '+ACHIEVEMENTS.length+' UNLOCKED'),
    el('p','','Earn XP and Command Credits by executing defensive actions, detecting intrusions, and solving assigned cases.')
  );
  body.append(summary);
  var grid=el('div','aegis-achievement-grid');
  ACHIEVEMENTS.forEach(function(ach){
    var isUnlocked=unlocked.includes(ach.id);
    var card=el('div','achievement-card'+(isUnlocked?' is-unlocked':''));
    card.append(el('span','achievement-icon',ach.icon));
    var info=el('div','achievement-info');
    info.append(el('strong','',ach.title),el('small','',ach.desc));
    var meta=el('div','achievement-meta');
    meta.append(
      el('span','mission-grade-badge '+(isUnlocked?'grade-s':'grade-d'),'+'+ach.xp+' XP'),
      el('span','achievement-status-badge '+(isUnlocked?'status-unlocked':'status-locked'),isUnlocked?'UNLOCKED ✓':'LOCKED 🔒')
    );
    card.append(info,meta);
    grid.append(card);
  });
  body.append(grid);
  if(count)count.textContent=unlocked.length+'/'+ACHIEVEMENTS.length+' MEDALS';
}

function renderDrillsView(){
  var topBox=el('div','aegis-achievement-summary');
  topBox.append(
    el('h3','','⚡ LIVE EMERGENCY DEFENSE SIMULATOR'),
    el('p','','Simulate real-time cybersecurity zero-day attacks and test your response speed under pressure.')
  );
  body.append(topBox);
  TIMED_SCENARIOS.forEach(function(scn){
    var card=el('div','aegis-drill-card');
    card.append(
      el('h4','',scn.threatType+' // '+scn.title),
      el('p','',scn.brief+' Time limit: '+scn.seconds+' seconds.')
    );
    var btn=el('button','aegis-drill-launch-btn','LAUNCH ATTACK DRILL ('+scn.seconds+'s) ⚠️');
    btn.type='button';
    btn.addEventListener('click',function(){
      triggerTimedAttack(scn.id);
      panel.hidden=true;
    });
    card.append(btn);
    body.append(card);
  });
}


function showAiDialogue(badgeText, speech, options){
  if(!copilotPopup)return;
  copilotPopup.hidden=false;
  copilotPopup.classList.remove('is-minimized');
  if(copilotOrb)copilotOrb.hidden=true;
  if(copilotBadge)copilotBadge.textContent=badgeText||('CASE #0'+(state.index+1));
  copilotBody.replaceChildren();
  var msg=el('div','ai-dialogue-msg from-ai');
  msg.append(el('span','ai-msg-sender','AEGIS MIND // TACTICAL CASE COPILOT'),el('div','ai-msg-text',speech));
  copilotBody.append(msg);
  copilotResponses.replaceChildren();
  (options||[]).forEach(function(opt){
    var pill=el('button','ai-suggested-pill',opt.q);
    pill.type='button';
    pill.addEventListener('click',function(){
      var userMsg=el('div','ai-dialogue-msg from-user');
      userMsg.append(el('span','ai-msg-sender user-sender','INVESTIGATOR'),el('div','ai-msg-text',opt.q));
      copilotBody.append(userMsg);
      playSfx('clue');
      setTimeout(function(){
        var aiReply=el('div','ai-dialogue-msg from-ai');
        aiReply.append(el('span','ai-msg-sender','AEGIS MIND // FORENSIC CLUE'),el('div','ai-msg-text',opt.a));
        copilotBody.append(aiReply);
        copilotBody.scrollTop=copilotBody.scrollHeight;
        copilotResponses.replaceChildren();
        var doneBtn=el('button','ai-suggested-pill','Understood, investigating now →');
        doneBtn.type='button';
        doneBtn.addEventListener('click',function(){
          copilotPopup.classList.add('is-minimized');
        });
        copilotResponses.append(doneBtn);
      },160);
    });
    copilotResponses.append(pill);
  });
  copilotBody.scrollTop=copilotBody.scrollHeight;
}

function render(){
 if(!body)return;updateTabNav();body.replaceChildren();
 if(currentTab==='achievements'){renderAchievementsView();return;}
 if(currentTab==='drills'){renderDrillsView();return;}
 var m=mission(),list=el('ol','mission-objectives');
 var head=el('div','mission-operation-top');
 head.append(el('span','mission-kicker','AEGIS COMMAND // CASE #0'+(state.index+1)+' OF 3'),el('h2','',m.title));
 var comm=el('div','mission-comm-dispatch');
 comm.append(el('small','','TACTICAL COMMS // DIRECTOR VANCE'),el('span','','"'+m.brief+'"'));
 head.append(comm,el('p','mission-pressure',m.pressure));
 var done=0;m.steps.forEach(function(s,i){var kind=s[3],complete=kind==='choice'?!!state.decision[m.id]:kind==='report'?state.done.includes(m.id):evidence().includes(i);if(complete)done++;});
 head.append(el('div','mission-progress-label',done+' / '+m.steps.length+' investigative milestones secured'));
 var bar=el('div','mission-progress-native'),fill=el('i','');fill.style.width=Math.round(done/m.steps.length*100)+'%';bar.append(fill);head.append(bar);
 var stats=state.stats||{xp:0,credits:0,research:0,score:0};head.append(el('div','mission-meta','COMMAND SCORE '+stats.score+'  ·  XP '+stats.xp+'  ·  CREDITS '+stats.credits+'  ·  RESEARCH '+stats.research));body.append(head);
 m.steps.forEach(function(s,i){var kind=s[3],complete=kind==='choice'?!!state.decision[m.id]:kind==='report'?state.done.includes(m.id):evidence().includes(i);var partial=Array.isArray(s[3])?(fragments()[i]||[]).length:0;var li=el('li','mission-objective'+(complete?' is-done':partial?' is-progress':''));li.append(el('span','mission-step',String(i+1).padStart(2,'0')),el('div','mission-objective-copy',s[0]));if(!complete&&kind!=='choice'&&kind!=='report'){li.append(el('small','',s[1]));if(partial)li.append(el('small','mission-fragment-note','Evidence sources: '+partial+'/'+s[3].length));}list.append(li);});
 body.append(list);
 var choiceStep=m.steps.find(function(s){return s[3]==='choice';});
 if(choiceStep&&!state.decision[m.id]){var c=el('div','mission-choice');c.append(el('h3','',choiceStep[0]),el('p','',choiceStep[1]));var choiceIndex=m.steps.indexOf(choiceStep),requiredBefore=m.steps.slice(0,choiceIndex).filter(function(s){return Array.isArray(s[3]);}).length,decisionUnlocked=evidence().length>=requiredBefore;m.choices.forEach(function(label,i){var b=el('button','mission-choice-btn');b.type='button';b.disabled=!decisionUnlocked;b.append(el('strong','',label),el('small','',choiceStep[2]));b.addEventListener('click',function(){if(!decisionUnlocked)return;state.decision[m.id]=i;playSfx('decision');record('Command decision: '+label);render();toast('Decision recorded',label);});c.append(b);});if(!decisionUnlocked)c.append(el('small','mission-choice-lock','Complete the earlier investigation milestones before selecting a response.'));body.append(c);}
 var report=m.steps.find(function(s){return s[3]==='report';});if(report){var quiz=el('div','mission-quiz');quiz.append(el('h3','','FINAL DEDUCTION // INCIDENT REPORT'));(m.quiz||[]).forEach(function(q){var label=el('label','mission-quiz-label',q.question);var input=el('input','mission-quiz-input');input.type='text';input.autocomplete='off';input.placeholder=q.placeholder;input.value=(state.answers&&state.answers[m.id]&&state.answers[m.id][q.id])||'';input.addEventListener('input',function(){if(!state.answers)state.answers={};if(!state.answers[m.id])state.answers[m.id]={};state.answers[m.id][q.id]=input.value;save();submit.disabled=!ready();});var hint=el('small','',q.hint);label.append(input,hint);quiz.append(label);});body.append(quiz);var submit=el('button','mission-report-btn',state.done.includes(m.id)?'OPERATION SECURED':'SUBMIT EVIDENCE & CLOSE OPERATION');submit.type='button';function ready(){return evidence().length>=m.steps.filter(function(s){return Array.isArray(s[3]);}).length&&state.decision[m.id]!==undefined&&answersCorrect(m);}submit.disabled=!ready();submit.addEventListener('click',complete);body.append(submit);body.append(el('small','mission-report-note',ready()?'Deductions verified. Your incident report is ready for command review.':'The report stays sealed until every evidence source, strategic decision and final deduction is complete.'));}
 var logs=el('div','mission-log');logs.append(el('h3','','RECENT FIELD NOTES'));state.log.filter(function(x){return x.id===m.id;}).slice(-5).reverse().forEach(function(x){logs.append(el('p','',x.t+'  '+x.s));});body.append(logs);
 var hint=el('details','mission-hint');hint.append(el('summary','','Request a field hint'));var next=m.steps.find(function(s,i){return Array.isArray(s[3])&&!evidence().includes(i);});hint.append(el('p','',next?next[2]:'Review your decision and final report.'));body.append(hint);
 if(count)count.textContent=done+'/'+m.steps.length;
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
        { q:'Resume standard operations →', a:'Active case dossier is ready for inspection.' }
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
  if(type==='cybersec'&&value==='cybersec-packet-inspect')unlockAchievement('packet_hunter');

  if(!authenticated||!state.started||state.done.includes(mission().id))return;
  var m=mission(),changed=false,milestoneIndex=-1;
  m.steps.forEach(function(s,i){
    if(!Array.isArray(s[3])||evidence().includes(i))return;
    var progress=fragments()[i]||(fragments()[i]=[]);
    s[3].forEach(function(group,gi){
      if(progress.includes(gi))return;
      var matched=group.split('|').some(function(token){var p=token.split(':');return p[0]===type&&(p.slice(1).join(':')==='*'||p.slice(1).join(':')===value);});
      if(matched){progress.push(gi);record('Evidence source recorded for: '+s[0]);changed=true;}
    });
    if(progress.length===s[3].length){evidence().push(i);record('Evidence milestone secured: '+s[0]);milestoneIndex=i;}
  });
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
  if(state.done.length===3){render();toast('ALL CASES SOLVED','All three operations secured. XP '+state.stats.xp+' · Credits '+state.stats.credits+' · Research '+state.stats.research+'.','S');return;}
  state.index++;state.started=false;save();render();setTimeout(start,1600);
}

function start(){
  if(!authenticated||state.started||state.done.includes(mission().id))return;
  state.started=true;save();playSfx('alert');render();
  var dlg=CASE_DIALOGUES[mission().id]&&CASE_DIALOGUES[mission().id].start;
  if(dlg){
    showAiDialogue('NEW CASE ASSIGNED',dlg.text,dlg.options);
  }
  toast('NEW CASE ASSIGNED // '+mission().title,'An attack has occurred. Use your system tools to investigate and connect the dots.');
}

function build(){
  var link=document.createElement('link');link.rel='stylesheet';link.href='/native-campaign.css';document.head.append(link);
  var host=el('aside','aegis-mission-hud');host.id='aegis-mission-hud';
  var toggle=el('button','aegis-mission-toggle');toggle.type='button';toggle.title='Toggle Case Operations Dossier (Hotkey: F4)';
  toggle.innerHTML='<span class="mission-pulse"></span> CASE DOSSIER <b id="aegis-mission-count"></b>';
  count=toggle.querySelector('b');
  panel=el('section','aegis-mission-panel');panel.hidden=true;
  var top=el('div','aegis-mission-head'),brand=el('div','');
  brand.append(el('small','','AEGIS DEFENCE COMMAND // ACTIVE CASE'),el('strong','','INVESTIGATION DOSSIER'));
  var actions=el('div','aegis-head-actions');
  sfxBtn=el('button','mission-audio-toggle',soundOn?'🔊 SFX':'🔇 SFX');sfxBtn.type='button';sfxBtn.title='Toggle Sound Effects (Hotkey: M)';
  sfxBtn.addEventListener('click',function(){
    soundOn=!soundOn;localStorage.setItem('aegis.sfx.enabled',String(soundOn));
    sfxBtn.textContent=soundOn?'🔊 SFX':'🔇 SFX';
    if(soundOn)playSfx('clue');
  });
  var close=el('button','aegis-mission-close','×');close.type='button';
  actions.append(sfxBtn,close);
  top.append(brand,actions);
  tabNav=el('div','aegis-dossier-nav');
  var bCase=el('button','aegis-dossier-tab active','📁 CASES');
  var bAch=el('button','aegis-dossier-tab','🏅 MEDALS');
  var bDrill=el('button','aegis-dossier-tab','⚡ LIVE DRILLS');
  bCase.addEventListener('click',function(){currentTab='cases';render();});
  bAch.addEventListener('click',function(){currentTab='achievements';render();});
  bDrill.addEventListener('click',function(){currentTab='drills';render();});
  tabNav.append(bCase,bAch,bDrill);
  body=el('div','aegis-mission-body');panel.append(top,tabNav,body);
  host.append(toggle,panel);document.body.append(host);
  toggle.addEventListener('click',function(){panel.hidden=!panel.hidden;render();});
  close.addEventListener('click',function(){panel.hidden=true;});

  // AI Copilot Interactive Dialogue Popup
  copilotPopup=el('aside','aegis-ai-copilot-popup');
  copilotPopup.id='aegis-ai-copilot-popup';
  copilotPopup.hidden=true;
  var cHead=el('div','ai-copilot-head');
  var cBrand=el('div','ai-copilot-brand');
  copilotBadge=el('span','ai-copilot-badge','CASE COPILOT');
  cBrand.append(el('div','ai-copilot-waveform'),el('span','ai-copilot-title','AEGIS MIND // CASE PARTNER'),copilotBadge);
  var cCtrls=el('div','ai-copilot-controls');
  var cMin=el('button','ai-copilot-ctrl-btn','_');cMin.type='button';cMin.title='Minimize to orb';
  var cClose=el('button','ai-copilot-ctrl-btn','×');cClose.type='button';cClose.title='Hide dialog';
  cCtrls.append(cMin,cClose);
  cHead.append(cBrand,cCtrls);
  copilotBody=el('div','ai-copilot-body');
  copilotResponses=el('div','ai-copilot-responses');
  copilotPopup.append(cHead,copilotBody,copilotResponses);
  document.body.append(copilotPopup);

  // Floating Minimized Orb
  copilotOrb=el('div','ai-minimized-orb');
  copilotOrb.hidden=true;
  copilotOrb.innerHTML='<span class="ai-copilot-waveform"></span> AEGIS MIND [💬 CASE CLUES]';
  copilotOrb.title='Click to open interactive Case Copilot';
  document.body.append(copilotOrb);

  cMin.addEventListener('click',function(){copilotPopup.hidden=true;copilotOrb.hidden=false;});
  cClose.addEventListener('click',function(){copilotPopup.hidden=true;copilotOrb.hidden=false;});
  copilotOrb.addEventListener('click',function(){copilotOrb.hidden=true;copilotPopup.hidden=false;copilotPopup.classList.remove('is-minimized');});

  window.addEventListener('keydown',function(e){
    if(e.key==='F4'){e.preventDefault();panel.hidden=!panel.hidden;render();}
    if(e.key==='m'||e.key==='M'){
      var tag=document.activeElement&&document.activeElement.tagName;
      if(tag!=='INPUT'&&tag!=='TEXTAREA'){sfxBtn.click();}
    }
  });
  render();
}

document.addEventListener('aegis:desktop-event',function(e){
  var d=e.detail||{};
  var type=d.type||'';
  var val=String(d.value||'');
  observe(type,val);
  if(type==='token-revoked'){
    observe('click','shield-token-audit');
    unlockAchievement('zero_trust_elite');
  }
  if(type==='file-recovered'){
    observe('click','files-inspect-suspicious');
    unlockAchievement('crypt_inode');
  }
  if(type==='server-fixed'){
    observe('click','optimizer-resource-audit');
    observe('terminal','service-health');
  }
});
document.addEventListener('aegis:ai-prompt',function(e){var p=String(e.detail&&e.detail.prompt||'').toLowerCase();if(/correlat|timeline|root cause/.test(p)){observe('click','ai-correlate-timeline');observe('click','ai-correlate-kernel');observe('click','ai-correlate-traffic');}});
document.addEventListener('aegis:terminal-command',function(e){
  var c=String(e.detail&&e.detail.command||'').trim().toLowerCase();
  uniqueTerminalCommands.add(c.split(' ')[0]);
  if(uniqueTerminalCommands.size>=8)unlockAchievement('terminal_warrior');
  if(c.indexOf('drill')>=0||c.indexOf('simulate-attack')>=0){
    triggerTimedAttack();
  }
  if(c.indexOf('achievements')>=0){
    currentTab='achievements';
    panel.hidden=false;
    render();
  }
  ['ps','netstat','ss','journalctl','capability-audit','verify-containment','verify-root','quarantine-component','service-health','route-health','route-select','carve-file','recover-file','inspect-hex','kill','patch-config','service-restart','revoke-token'].forEach(function(k){if(c.indexOf(k)>=0)observe('terminal',k);});
  if(c.indexOf('revoke-token')>=0){unlockAchievement('zero_trust_elite');observe('click','shield-token-audit');}
  if(c.indexOf('carve-file')>=0||c.indexOf('recover-file')>=0){unlockAchievement('crypt_inode');observe('click','files-inspect-suspicious');}
  if(c.indexOf('service-restart')>=0||c.indexOf('kill 4819')>=0){observe('terminal','service-health');}
});
document.addEventListener('click',function(e){var b=e.target.closest('button,[role="button"],tr,.packet-row,.process-row,.process-card,.event-row,.merkle-block-card,.metric-gauge');if(!b)return;var id=b.id||'',txt=(b.textContent||'').toLowerCase(),app=b.closest('.aegis-window')&&b.closest('.aegis-window').dataset.appId;var map={'btn-run-vuln-scan':'cybersec-vulnerability-scan','btn-scan-guard':'shield-guard-scan','btn-rollback-merkle':'merkle-rollback','btn-corrupt-merkle':'merkle-corrupt','btn-refresh-files':'files-refresh','btn-run-trim-optimize':'optimizer-resource-audit'};if(map[id])observe('click',map[id]);if(app==='neural'){if(b.closest('.process-row,.process-card,.event-row')||/process detail|inspect process/.test(txt))observe('click','neural-process-detail');if(b.closest('.event-row')||/memory|privilege|address space/.test(txt))observe('click','neural-memory-map');}if(app==='cybersec'){if(b.closest('.packet-row')||/packet|sniffer|traffic feed/.test(txt)||id==='tab-packets')observe('click','cybersec-packet-inspect');if(/kernel patch|apply patch|patch signature/.test(txt))observe('click','cybersec-apply-kernel-patch');}if(app==='shield'){if(/token|capability|zero.trust/.test(txt)||id==='tab-zt-tokens')observe('click','shield-token-audit');if(/firewall|edge|rule/.test(txt)||id==='tab-firewall')observe('click','shield-edge-audit');if(/reroute|route 02|resilient route/.test(txt))observe('click','shield-reroute');}if(app==='merkle'&&(/integrity|merkle root|verify|block hash/.test(txt)||b.classList.contains('merkle-block-card')))observe('click','merkle-integrity-scan');if(app==='files'&&b.closest('tr')&&/suspicious|service|stager|sync-cache/.test(b.closest('tr').textContent.toLowerCase()))observe('click','files-inspect-suspicious');if(app==='files'&&b.closest('tr')&&/libaegis_kernel/.test(b.closest('tr').textContent))observe('click','files-inspect-library');if(app==='ai'&&/correlate|analyse|analyze|timeline|root cause/.test(txt)){observe('click','ai-correlate-timeline');observe('click','ai-correlate-kernel');observe('click','ai-correlate-traffic');}if(app==='optimizer'&&(b.closest('.metric-gauge')||/resource|performance|telemetry|refresh/.test(txt)))observe('click','optimizer-resource-audit');},true);
document.addEventListener('change',function(e){if(e.target.matches('.rule-toggle')){if(e.target.dataset.id==='7')observe('change','shield-rule');if(e.target.dataset.id==='8')observe('change','shield-rate-policy');}});
window.addEventListener('aegis:wm-open',function(e){observe('open',e.detail&&e.detail.app||'');});
build();
document.addEventListener('aegis:authenticated',async function(){authenticated=true;syncing=true;try{var response=await fetch('/api/campaign/state',{credentials:'same-origin'});if(response.ok){var payload=await response.json();state=payload.state&&payload.state.v===1?payload.state:fresh();ensureProfileState();}else state=fresh();}catch(e){state=fresh();}ensureProfileState();syncing=false;save();panel.hidden=true;render();if(!state.started&&!state.done.includes(mission().id))start();});
window.AegisCampaign={
  open:function(){panel.hidden=false;render();},
  openAchievements:function(){currentTab='achievements';panel.hidden=false;render();},
  openDrills:function(){currentTab='drills';panel.hidden=false;render();},
  triggerTimedAttack:triggerTimedAttack,
  unlockAchievement:unlockAchievement,
  openCopilot:function(){if(copilotPopup){copilotPopup.hidden=false;copilotPopup.classList.remove('is-minimized');if(copilotOrb)copilotOrb.hidden=true;}},
  get state(){return JSON.parse(JSON.stringify(state));},
  reset:function(){if(confirm('Reset local campaign progress?')){state=fresh();save();render();start();}}
};
})();
