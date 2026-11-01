const fs = require('fs');
const path = require('path');

const campaignFile = path.resolve('simulator/web/native-campaign.js');
let code = fs.readFileSync(campaignFile, 'utf8');

// 1. Define CLUE_CARDS database
const clueCardsDefinition = `
var CLUE_CARDS = {
  op1: [
    {
      id: 'clue_op1_01',
      stepIndex: 0,
      title: 'Anomalous Parent-Child Ancestry',
      cat: 'PROCESS_TREE',
      badge: 'PID 1337',
      ioc: 'Parent PID 1 (Init) vs svc_sync_worker',
      hexSnippet: '0x004018A0: CALL ring3_trampoline',
      hint: 'Neural Monitor identifies that svc_sync_worker was spawned without declared init daemon parameters.',
      guide: 'Open Neural Monitor, click on process PID 1337 to inspect process ancestry and observe unauthorized child threads.',
      app: 'neural'
    },
    {
      id: 'clue_op1_02',
      stepIndex: 1,
      title: 'Socket Listener NetFlow Beacon',
      cat: 'NETWORK_NETSTAT',
      badge: '185.220.101.5:8443',
      ioc: 'TCP SYN Outbound -> 185.220.101.5:8443',
      hexSnippet: 'ESTABLISHED -> 185.220.101.5:8443 (TLS 1.3)',
      hint: 'Terminal netstat command reveals persistent outbound socket connection to known Spectre C2 relay.',
      guide: 'Open Terminal and execute \`netstat\` and \`ps\`. Compare active socket descriptors with process PID 1337.',
      app: 'terminal'
    },
    {
      id: 'clue_op1_03',
      stepIndex: 2,
      title: 'High-Entropy Encrypted C2 Packets',
      cat: 'DPI_PACKET',
      badge: 'TLS 1.3 ENCRYPTED',
      ioc: 'Destination: 185.220.101.5:8443 (Entropy 7.94)',
      hexSnippet: '17 03 03 00 80 4F 8A 2C ... [TLS Handshake]',
      hint: 'CyberSec Arsenal DPI inspection verifies repetitive beacon intervals occurring exactly every 30.0 seconds.',
      guide: 'Open CyberSecurity Arsenal, navigate to Packet Inspector, and click on outbound packets to destination 185.220.101.5:8443.',
      app: 'cybersec'
    },
    {
      id: 'clue_op1_04',
      stepIndex: 3,
      title: 'Staged Executable Artefact in Filesystem',
      cat: 'FILESYSTEM_INODE',
      badge: 'svc-sync-cache.bin',
      ioc: 'Inode: 0x4A10 / SHA256: 4f8a2c7e...',
      hexSnippet: '7F 45 4C 46 02 01 01 00 ... [ELF 64-bit]',
      hint: 'Modified timestamp precedes outbound beacon by 4 minutes, establishing deployment order.',
      guide: 'Open Files & Storage, inspect /var/cache/ directory, and check properties of svc-sync-cache.bin.',
      app: 'files'
    }
  ],
  op2: [
    {
      id: 'clue_op2_01',
      stepIndex: 0,
      title: 'Ring-0 Transition Trampoline',
      cat: 'MEMORY_MAP',
      badge: 'OFFSET 0x7FFF0000',
      ioc: 'Ring-3 -> Ring-0 Unauthorized Page Fault',
      hexSnippet: '0x7FFF0000: 0F 01 F8 (SWAPGS) ; SYSENTER',
      hint: 'Neural Monitor memory map flags non-contiguous page table allocation with execution flags in ring-0.',
      guide: 'Open Neural Monitor, toggle Memory Map view, and locate anomalous memory block at 0x7FFF0000.',
      app: 'neural'
    },
    {
      id: 'clue_op2_02',
      stepIndex: 1,
      title: 'Elevated Zero-Trust Capability Token',
      cat: 'TOKEN_REGISTRY',
      badge: 'TOK-KERNEL-ESCALATED',
      ioc: 'Subject: kernel_sync_worker / Scope: CAP_SYS_ADMIN',
      hexSnippet: 'AUTH_BEARER: TOK-KERNEL-ESCALATED [OVERPRIVILEGED]',
      hint: 'Aegis Shield capability token table shows service claiming global system administration capability.',
      guide: 'Open Aegis Shield, click Token Registry, and inspect anomalous token assigned to kernel_sync_worker.',
      app: 'shield'
    },
    {
      id: 'clue_op2_03',
      stepIndex: 2,
      title: 'Component Authority Mismatch',
      cat: 'TERMINAL_AUDIT',
      badge: 'AUDIT_FAIL',
      ioc: 'Capability Token TOK-KERNEL-ESCALATED unverified',
      hexSnippet: '[!] ALERT: Capability grant exceeds baseline by 3 tiers',
      hint: 'Running capability-audit in Terminal exposes unverified capability grant issued without root signature.',
      guide: 'Open Terminal and execute \`capability-audit\` to inspect token signature verification.',
      app: 'terminal'
    }
  ],
  op3: [
    {
      id: 'clue_op3_01',
      stepIndex: 0,
      title: 'Queue Depth Degradation on Edge-01',
      cat: 'RESOURCE_AUDIT',
      badge: 'QUEUE 91% FULL',
      ioc: 'Interface: edge-route-01 / Saturation: 91%',
      hexSnippet: 'RX_DROPS: 4,819 pkts/sec | QUEUE_MAX: 1024',
      hint: 'Task Optimizer shows CPU at 42% but edge network queue at 91% saturation due to asymmetric packet flood.',
      guide: 'Open Task Optimizer, inspect Resource Audit telemetry, and observe queue depth on edge-route-01.',
      app: 'optimizer'
    },
    {
      id: 'clue_op3_02',
      stepIndex: 1,
      title: 'Asymmetric UDP Port Flood Signature',
      cat: 'PACKET_INSPECTION',
      badge: 'UDP 53 / DNS-SEC',
      ioc: 'Source: 198.51.100.22 -> UDP Port 53 Flood',
      hexSnippet: 'UDP LEN: 4096 | DNS-SEC ANY query response',
      hint: 'Packet sniffer reveals coordinated DNS amplification flood with 4096-byte responses overwhelming edge.',
      guide: 'Open CyberSecurity Arsenal, inspect packet distribution, and note repetitive source IP 198.51.100.22.',
      app: 'cybersec'
    },
    {
      id: 'clue_op3_03',
      stepIndex: 2,
      title: 'Resilient Edge Route Availability',
      cat: 'ROUTE_HEALTH',
      badge: 'edge-route-02 (28%)',
      ioc: 'edge-route-02: HEALTHY / Queue 28%',
      hexSnippet: 'ROUTE_02: Priority traffic enabled | Latency: 4ms',
      hint: 'Terminal route-health shows edge-route-02 is operating normally with priority traffic capability.',
      guide: 'Open Terminal, run \`route-health\`, and select \`edge-route-02\` using \`route-select edge-route-02\`.',
      app: 'terminal'
    }
  ],
  op4: [
    {
      id: 'clue_op4_01',
      stepIndex: 0,
      title: 'Ransomware Dropper Worker Thread',
      cat: 'PROCESS_ANCESTRY',
      badge: 'DROPPED PID 4910',
      ioc: 'Worker PID 4910 spawned without service manifest',
      hexSnippet: 'SYS_CALL: sys_openat(AT_FDCWD, "/userland/data", O_RDWR)',
      hint: 'Neural Monitor traces rogue thread opening all files in /userland/data/ for in-place encryption.',
      guide: 'Open Neural Monitor, inspect process ancestry, and identify the rogue ransomware worker.',
      app: 'neural'
    },
    {
      id: 'clue_op4_02',
      stepIndex: 3,
      title: 'Cryptographic Header Magic Bytes',
      cat: 'HEX_CARVER',
      badge: 'MAGIC 56 41 55 4c',
      ioc: 'Corrupted: 00 00 00 00 -> Restored: 56 41 55 4c',
      hexSnippet: 'EXPECTED: 56 41 55 4c ("VAUL") | ASCII: VAULT_ENC',
      hint: 'File Recovery Hex Carver needs 56 41 55 4c to reconstruct the wiped user_vault.dat inode enclave.',
      guide: 'Open Files & Storage -> File Recovery, select user_vault.dat.corrupt, and enter magic bytes 56 41 55 4c.',
      app: 'files'
    }
  ],
  op5: [
    {
      id: 'clue_op5_01',
      stepIndex: 1,
      title: 'Covert DNS-SEC Steganography Channel',
      cat: 'DPI_TUNNEL',
      badge: 'IP 198.51.100.22',
      ioc: 'Hostile C2: 198.51.100.22 / TXT Record Tunneling',
      hexSnippet: 'TXT: "AEGIS_QUANTUM_KEY_EXFIL_PAYLOAD_CHUNK_01..."',
      hint: 'Adversary is exfiltrating post-quantum enclave keys disguised as DNS TXT record replies.',
      guide: 'Open CyberSec Arsenal, inspect DNS-SEC packet stream, and identify C2 relay IP 198.51.100.22.',
      app: 'cybersec'
    },
    {
      id: 'clue_op5_02',
      stepIndex: 4,
      title: 'AegisDB Midnight Audit Ingress',
      cat: 'AEGIS_DB_AUDIT',
      badge: 'AUDIT LOG #003',
      ioc: 'Audit action ZERO_TRUST_DEFENSE against 198.51.100.22',
      hexSnippet: 'AegisDB: user_audit_logs -> IP 198.51.100.22 BLOCKED',
      hint: 'AegisDB audit table stores immutable logs proving adversary ingress attempts.',
      guide: 'Open AegisDB Studio, select table user_audit_logs, and inspect recent high-severity entries.',
      app: 'database'
    }
  ],
  op6: [
    {
      id: 'clue_op6_01',
      stepIndex: 0,
      title: 'Ring-0 Shadow Interrupt Vector',
      cat: 'MICROKERNEL_ASLR',
      badge: 'VECTOR 0x2E HOOK',
      ioc: 'IDT Interrupt Descriptor Table Hook at 0x2E',
      hexSnippet: 'IDT[0x2E]: JMP 0xDEADBEEF ; Shadow ISR handler',
      hint: 'Diagnostic Scanner tests verify microkernel page table isolation detected invalid interrupt trampoline.',
      guide: 'Open System Diagnostic Scanner (Troubleshoot) and run the full diagnostic scan.',
      app: 'troubleshoot'
    },
    {
      id: 'clue_op6_02',
      stepIndex: 3,
      title: 'Rogue Ingress Daemon PID 4819',
      cat: 'KERNEL_DAEMON',
      badge: 'PID 4819',
      ioc: 'Rogue listener PID 4819 holding CAP_NET_RAW_SOCKET',
      hexSnippet: 'PID 4819: svc_ingress_worker (UNVERIFIED_SIGNATURE)',
      hint: 'Terminal capability-audit proves PID 4819 is maintaining rogue shadow interrupt backdoor.',
      guide: 'Open Terminal, run \`capability-audit\` and \`kill-rogue-pid\` to terminate rogue worker PID 4819.',
      app: 'terminal'
    }
  ]
};
`;

if (!code.includes('var CLUE_CARDS = {')) {
    code = code.replace(
        'var missions=[',
        clueCardsDefinition + '\r\nvar missions=['
    );
}

// 2. Add Clue Inspector Modal opener
const clueInspectorFunction = `
function openClueModal(clue, isUnlocked) {
  var modal = document.getElementById('aegis-clue-card-modal');
  if (!modal) return;
  var title = document.getElementById('clue-modal-heading');
  var type = document.getElementById('clue-modal-type');
  var status = document.getElementById('clue-modal-status');
  var ioc = document.getElementById('clue-modal-ioc');
  var hex = document.getElementById('clue-modal-hex');
  var guide = document.getElementById('clue-modal-guide');
  var btnNotes = document.getElementById('btn-clue-insert-notes');
  var btnJump = document.getElementById('btn-clue-jump-tool');
  var closeBtn = document.getElementById('clue-modal-close');

  if (title) title.textContent = clue.title;
  if (type) type.textContent = clue.cat;
  if (status) {
    status.textContent = isUnlocked ? 'UNCOVERED EVIDENCE ✓' : 'LOCKED CLUE 🔒';
    status.style.color = isUnlocked ? '#00ff80' : '#ff5577';
  }
  if (ioc) ioc.textContent = isUnlocked ? clue.ioc : '[LOCKED - Execute Milestone ' + (clue.stepIndex + 1) + ' to Uncover]';
  if (hex) hex.textContent = isUnlocked ? clue.hexSnippet : '?? ?? ?? ?? ?? ?? ?? ?? [ENCRYPTED MEMORY RANGE]';
  if (guide) guide.textContent = clue.guide;

  if (btnNotes) {
    btnNotes.onclick = function() {
      var noteText = '\\n[CLUE DISCOVERY: ' + clue.title + ']\\nIOC: ' + clue.ioc + '\\nTelemetry: ' + clue.hexSnippet + '\\n';
      fetch('/api/files/write', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ path: '/userland/notes/case_notes.txt', content: noteText, append: true })
      }).catch(function(){});
      toast('COPIED TO CASE NOTES', 'Evidence IOC saved to /userland/notes/case_notes.txt', 'S');
      playSfx('milestone');
    };
  }

  if (btnJump) {
    btnJump.onclick = function() {
      modal.classList.add('hidden');
      var appBtn = document.querySelector('[data-app="' + clue.app + '"]');
      if (appBtn) appBtn.click();
      toast('TOOL LAUNCHED', 'Focusing ' + clue.app.toUpperCase() + ' for investigative triage.', 'I');
    };
  }

  if (closeBtn) {
    closeBtn.onclick = function() {
      modal.classList.add('hidden');
    };
  }

  modal.classList.remove('hidden');
}
`;

if (!code.includes('function openClueModal(')) {
    code = code.replace(
        'function renderSideQuestsView()',
        clueInspectorFunction + '\r\nfunction renderSideQuestsView()'
    );
}

// 3. Add Clue Cards View function
const clueCardsViewFunction = `
function renderClueCardsView() {
  var m = mission();
  var clues = CLUE_CARDS[m.id] || [];
  var ev = evidence();

  var topBox = el('div', 'aegis-achievement-summary');
  var discoveredCount = clues.filter(c => ev.includes(c.stepIndex)).length;
  topBox.append(
    el('h3', '', '🎴 FORENSIC CLUE CARDS // CASE: ' + m.title),
    el('p', '', 'Review recovered forensic evidence, cryptographic signatures, and tactical clues. Click on any card to inspect full IOC indicators, copy telemetry to Case Notes, or jump directly to the required defensive tool.')
  );
  body.append(topBox);

  var grid = el('div', 'aegis-clue-grid');
  clues.forEach(function(c, i) {
    var isUnlocked = ev.includes(c.stepIndex);
    var card = el('div', 'aegis-clue-card' + (isUnlocked ? ' is-unlocked' : ' is-locked'));
    var header = el('div', 'clue-card-header');
    header.append(
      el('span', 'clue-cat-tag', c.cat),
      el('span', 'clue-status-tag', isUnlocked ? 'EVIDENCE VERIFIED ✓' : 'LOCKED 🔒')
    );
    var title = el('h4', '', c.title);
    var snippet = el('div', 'clue-snippet-preview', isUnlocked ? c.hexSnippet : '?? ?? ?? ?? [ENCRYPTED]');
    var footer = el('div', 'clue-card-footer');
    footer.append(
      el('span', 'clue-badge-ioc', isUnlocked ? c.badge : 'MILESTONE ' + (c.stepIndex + 1)),
      el('button', 'btn-action-small', isUnlocked ? 'INSPECT CLUE 🔍' : 'VIEW OBJECTIVE 💡')
    );

    card.addEventListener('click', function() {
      openClueModal(c, isUnlocked);
    });

    card.append(header, title, snippet, footer);
    grid.append(card);
  });
  body.append(grid);
}
`;

if (!code.includes('function renderClueCardsView()')) {
    code = code.replace(
        'function renderSideQuestsView()',
        clueCardsViewFunction + '\r\nfunction renderSideQuestsView()'
    );
}

// 4. Add Closed Cases Archive View function
const closedCasesViewFunction = `
function renderClosedCasesView() {
  state.closedCases = state.closedCases || [];
  state.sideQuests = state.sideQuests || {};

  var topBox = el('div', 'aegis-achievement-summary');
  var totalClosed = (state.done || []).length + Object.keys(state.sideQuests).filter(k => state.sideQuests[k]).length + state.closedCases.length;
  topBox.append(
    el('h3', '', '📁 CLOSED CASES & INCIDENT ARCHIVES // ' + totalClosed + ' TOTAL CONTAINED'),
    el('p', '', 'Audit historical resolutions of secured operations, completed side bounties, and neutralized real-time threat incidents.')
  );
  body.append(topBox);

  // Category 1: Closed Missions (Operations)
  var secMissions = el('div', 'closed-section');
  secMissions.append(el('h4', 'closed-sec-title', '🛡️ CLOSED OPERATIONS (' + (state.done || []).length + ')'));
  if (!state.done || state.done.length === 0) {
    var empty = el('div', 'coming-soon-banner');
    empty.append(el('span', 'cs-icon', '📁'), el('h4', '', 'NO OPERATIONS CLOSED YET'), el('p', '', 'Complete all investigative milestones and submit the final incident report for Case #0' + (state.index + 1) + ' to archive it here.'));
    secMissions.append(empty);
  } else {
    state.done.forEach(function(mId) {
      var op = missions.find(m => m.id === mId) || { title: mId, brief: 'Operation contained and verified clean.' };
      var card = el('div', 'closed-case-card');
      card.append(
        el('div', 'closed-card-top', el('span', 'closed-tag-badge tag-operation', 'OPERATION SECURED'), el('span', 'closed-grade-badge', 'GRADE S // 100% CONTAINED')),
        el('h4', '', op.title),
        el('p', '', op.brief),
        el('small', 'closed-meta', 'Incident Report submitted to Command. System baseline verified clean.')
      );
      secMissions.append(card);
    });
  }
  body.append(secMissions);

  // Category 2: Closed Side Cases (Bounties)
  var closedSideQuests = SIDE_QUESTS.filter(sq => !!state.sideQuests[sq.id]);
  var secSide = el('div', 'closed-section');
  secSide.append(el('h4', 'closed-sec-title', '🎯 CLOSED SIDE CASES & BOUNTIES (' + closedSideQuests.length + ')'));
  if (closedSideQuests.length === 0) {
    var emptySide = el('div', 'coming-soon-banner');
    emptySide.append(el('span', 'cs-icon', '🎯'), el('h4', '', 'NO SIDE CASES CLOSED YET'), el('p', '', 'Tackle specialized forensic side quests in the Side Quests tab and claim bounties to archive them here.'));
    secSide.append(emptySide);
  } else {
    closedSideQuests.forEach(function(sq) {
      var card = el('div', 'closed-case-card');
      card.append(
        el('div', 'closed-card-top', el('span', 'closed-tag-badge tag-sidequest', sq.cat), el('span', 'closed-reward-badge', '+' + sq.xp + ' XP · +' + sq.credits + ' Credits')),
        el('h4', '', sq.title),
        el('p', '', sq.desc),
        el('small', 'closed-meta', 'Bounty verified and credited to operator profile in AegisDB.')
      );
      secSide.append(card);
    });
  }
  body.append(secSide);

  // Category 3: Resolved Popped Incidents & Real-Time Threats
  var secIncidents = el('div', 'closed-section');
  secIncidents.append(el('h4', 'closed-sec-title', '⚡ RESOLVED POPPED CASES & REAL-TIME INCIDENTS (' + state.closedCases.length + ')'));
  if (state.closedCases.length === 0) {
    var emptyInc = el('div', 'coming-soon-banner');
    emptyInc.append(el('span', 'cs-icon', '⚡'), el('h4', '', 'NO POPPED CASES RESOLVED YET'), el('p', '', 'Live cyber attacks, emergency drills, and random intrusions that are successfully neutralized will be logged and archived here.'));
    secIncidents.append(emptyInc);
  } else {
    state.closedCases.forEach(function(inc) {
      var card = el('div', 'closed-case-card');
      card.append(
        el('div', 'closed-card-top', el('span', 'closed-tag-badge tag-incident', inc.threatType || 'REAL-TIME INCIDENT'), el('span', 'closed-status-badge', 'NEUTRALIZED ✓')),
        el('h4', '', inc.title),
        el('p', '', inc.summary || 'Defensive countermeasures deployed within operational window. Zero data loss.'),
        el('small', 'closed-meta', 'Contained at ' + new Date(inc.resolvedAt || Date.now()).toLocaleTimeString() + ' | Audit signature verified.')
      );
      secIncidents.append(card);
    });
  }
  body.append(secIncidents);
}
`;

if (!code.includes('function renderClosedCasesView()')) {
    code = code.replace(
        'function renderSideQuestsView()',
        closedCasesViewFunction + '\r\nfunction renderSideQuestsView()'
    );
}

// 5. Update Dossier Navigation to 6 cohesive tabs
const newTabNavSetup = `  tabNav=el('div','aegis-dossier-nav');
  var bCase=el('button','aegis-dossier-tab active','📁 ACTIVE CASE');
  var bQuest=el('button','aegis-dossier-tab','🎯 SIDE QUESTS');
  var bClues=el('button','aegis-dossier-tab','🎴 CLUE CARDS');
  var bClosed=el('button','aegis-dossier-tab','📁 CLOSED CASES');
  var bAch=el('button','aegis-dossier-tab','🎖️ MEDALS');
  var bDrill=el('button','aegis-dossier-tab','⚡ LIVE DRILLS');

  bCase.addEventListener('click',function(){currentTab='cases';render();});
  bQuest.addEventListener('click',function(){currentTab='sidequests';render();});
  bClues.addEventListener('click',function(){currentTab='clues';render();});
  bClosed.addEventListener('click',function(){currentTab='closed';render();});
  bAch.addEventListener('click',function(){currentTab='achievements';render();});
  bDrill.addEventListener('click',function(){currentTab='drills';render();});

  tabNav.append(bCase,bQuest,bClues,bClosed,bAch,bDrill);`;

code = code.replace(
    /tabNav=el\('div','aegis-dossier-nav'\);[\s\S]*?tabNav\.append\(bCase,bQuest,bAch,bDrill\);/,
    newTabNavSetup
);

// 6. Update updateTabNav for 6 tabs
const updateTabNavCode = `function updateTabNav(){
  if(!tabNav)return;
  var tabs=tabNav.querySelectorAll('.aegis-dossier-tab');
  if(tabs.length>=6){
    tabs[0].className='aegis-dossier-tab'+(currentTab==='cases'?' active':'');
    tabs[1].className='aegis-dossier-tab'+(currentTab==='sidequests'?' active':'');
    tabs[2].className='aegis-dossier-tab'+(currentTab==='clues'?' active':'');
    tabs[3].className='aegis-dossier-tab'+(currentTab==='closed'?' active':'');
    tabs[4].className='aegis-dossier-tab'+(currentTab==='achievements'?' active':'');
    tabs[4].textContent='🎖️ MEDALS ('+(state.achievements||[]).length+'/'+ACHIEVEMENTS.length+')';
    tabs[5].className='aegis-dossier-tab'+(currentTab==='drills'?' active':'');
  }
}`;

code = code.replace(/function updateTabNav\(\)[\s\S]*?\}\s*\}/, updateTabNavCode);

// 7. Update render() dispatcher
code = code.replace(
    "if(currentTab==='sidequests'){renderSideQuestsView();return;}",
    "if(currentTab==='sidequests'){renderSideQuestsView();return;}\r\n if(currentTab==='clues'){renderClueCardsView();return;}\r\n if(currentTab==='closed'){renderClosedCasesView();return;}"
);

// 8. Add Tactical Step Guides with [🚀 Jump to Tool] button under each objective in render()
const tacticalStepGuideCode = `
    // Tactical Step Guide & Tool Jump
    if (!complete && kind !== 'choice' && kind !== 'report') {
      var guideWrap = el('div', 'step-guide-wrap');
      var guideToggle = el('button', 'btn-step-guide-toggle', '💡 Tactical Guide & Clue ▼');
      guideToggle.type = 'button';
      var guideContent = el('div', 'step-guide-content hidden');
      
      var cluesForStep = (CLUE_CARDS[m.id] || []).filter(c => c.stepIndex === i);
      var toolApp = cluesForStep[0]?.app || 'terminal';
      var guideDesc = cluesForStep[0]?.guide || ('Open ' + toolApp.toUpperCase() + ' and investigate ' + s[0]);
      
      guideContent.append(
        el('p', 'step-guide-text', '🎯 Target: ' + s[1]),
        el('p', 'step-guide-action', '🛠️ Procedure: ' + guideDesc)
      );
      
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
        guideToggle.textContent = guideContent.classList.contains('hidden') ? '💡 Tactical Guide & Clue ▼' : '💡 Hide Tactical Guide ▲';
      };
      guideWrap.append(guideToggle, guideContent);
      li.append(guideWrap);
    }
`;

if (!code.includes('btn-step-guide-toggle')) {
    code = code.replace(
        "if(partial)li.append(el('small','mission-fragment-note','Evidence sources: '+partial+'/'+s[3].length));}list.append(li);",
        "if(partial)li.append(el('small','mission-fragment-note','Evidence sources: '+partial+'/'+s[3].length));}" + tacticalStepGuideCode + "list.append(li);"
    );
}

// 9. When a timed attack or emergency is defused, log to state.closedCases!
if (!code.includes('state.closedCases.push(')) {
    code = code.replace(
        "activeEmergency = null;\r\n  updateEmergencyBanner();",
        `state.closedCases = state.closedCases || [];
  state.closedCases.push({
    id: 'inc_' + Date.now(),
    title: activeEmergency.title,
    threatType: activeEmergency.threatType,
    summary: 'Defensive countermeasures deployed within operational window. Zero data loss.',
    resolvedAt: Date.now()
  });
  save();
  activeEmergency = null;
  updateEmergencyBanner();`
    );
}

// 10. Add CSS for Clue Cards, Step Guides, and Closed Cases
const campaignExtraCss = `
/* Clue Grid & Cards */
.aegis-clue-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(240px, 1fr));
  gap: 12px;
  margin-top: 12px;
}
.aegis-clue-card {
  background: rgba(12, 22, 36, 0.9);
  border: 1px solid rgba(0, 243, 255, 0.25);
  border-radius: 6px;
  padding: 12px;
  cursor: pointer;
  transition: all 0.2s ease;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.aegis-clue-card:hover {
  border-color: #00f3ff;
  transform: translateY(-2px);
  box-shadow: 0 4px 16px rgba(0, 243, 255, 0.2);
}
.aegis-clue-card.is-unlocked {
  border-left: 3px solid #00f3ff;
}
.aegis-clue-card.is-locked {
  opacity: 0.6;
  border-left: 3px solid #ff5577;
}
.clue-card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.clue-cat-tag {
  font-size: 9px;
  font-family: var(--font-mono, monospace);
  color: #00f3ff;
  background: rgba(0, 243, 255, 0.12);
  padding: 2px 6px;
  border-radius: 3px;
  font-weight: 700;
}
.clue-status-tag {
  font-size: 9px;
  font-family: var(--font-mono, monospace);
  color: #00ff80;
}
.aegis-clue-card h4 {
  margin: 0;
  font-size: 13px;
  color: #fff;
}
.clue-snippet-preview {
  font-family: var(--font-mono, monospace);
  font-size: 10px;
  color: #00ff80;
  background: rgba(4, 8, 14, 0.8);
  padding: 6px 8px;
  border-radius: 4px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
.clue-card-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-top: auto;
}
.clue-badge-ioc {
  font-size: 10px;
  font-family: var(--font-mono, monospace);
  color: var(--text-muted);
}

/* Step Guides */
.step-guide-wrap {
  margin-top: 6px;
}
.btn-step-guide-toggle {
  background: transparent;
  border: none;
  color: #00f3ff;
  font-size: 10px;
  font-family: var(--font-mono, monospace);
  cursor: pointer;
  padding: 2px 0;
}
.btn-step-guide-toggle:hover {
  text-decoration: underline;
}
.step-guide-content {
  background: rgba(10, 18, 30, 0.7);
  border: 1px solid rgba(0, 243, 255, 0.2);
  border-radius: 4px;
  padding: 8px 10px;
  margin-top: 4px;
}
.step-guide-text {
  margin: 0 0 4px 0;
  font-size: 11px;
  color: var(--text-main);
}
.step-guide-action {
  margin: 0 0 6px 0;
  font-size: 10px;
  color: #00ff80;
  font-family: var(--font-mono, monospace);
}
.btn-step-jump {
  color: #00f3ff;
  border-color: #00f3ff;
}

/* Closed Cases */
.closed-section {
  margin-top: 14px;
}
.closed-sec-title {
  margin: 0 0 8px 0;
  font-size: 12px;
  font-family: var(--font-mono, monospace);
  color: #00f3ff;
  letter-spacing: 0.5px;
  border-bottom: 1px solid rgba(0, 243, 255, 0.2);
  padding-bottom: 4px;
}
.closed-case-card {
  background: rgba(10, 18, 28, 0.85);
  border: 1px solid rgba(0, 255, 128, 0.25);
  border-left: 3px solid #00ff80;
  border-radius: 6px;
  padding: 10px 12px;
  margin-bottom: 8px;
}
.closed-card-top {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 4px;
}
.closed-tag-badge {
  font-size: 9px;
  font-family: var(--font-mono, monospace);
  padding: 2px 6px;
  border-radius: 3px;
  font-weight: 700;
  text-transform: uppercase;
}
.tag-operation { background: rgba(0, 243, 255, 0.15); color: #00f3ff; }
.tag-sidequest { background: rgba(157, 0, 255, 0.15); color: #bf55ec; }
.tag-incident { background: rgba(255, 165, 0, 0.15); color: #ffa500; }
.closed-grade-badge, .closed-status-badge, .closed-reward-badge {
  font-size: 9px;
  font-family: var(--font-mono, monospace);
  color: #00ff80;
  font-weight: 600;
}
.closed-case-card h4 {
  margin: 0 0 4px 0;
  font-size: 13px;
  color: #fff;
}
.closed-case-card p {
  margin: 0 0 4px 0;
  font-size: 11px;
  color: var(--text-muted);
}
.closed-meta {
  font-size: 9px;
  font-family: var(--font-mono, monospace);
  color: #6a829a;
}
`;

fs.writeFileSync(path.resolve('simulator/web/native-campaign.css'), fs.readFileSync(path.resolve('simulator/web/native-campaign.css'), 'utf8') + '\n' + campaignExtraCss);

fs.writeFileSync(campaignFile, code, 'utf8');
console.log('Successfully updated native-campaign.js with Clue Cards, Tactical Guides, and Closed Cases Archive!');
