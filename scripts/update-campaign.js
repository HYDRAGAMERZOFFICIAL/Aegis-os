const fs = require('fs');
const path = require('path');

const srcPath = path.join(__dirname, '..', 'simulator', 'web', 'native-campaign.js');
let src = fs.readFileSync(srcPath, 'utf8');

// 1. Add emergency and achievement audio synthesizers to playSfx
const oldSfxEnd = `    }else if(type==='victory'){
      osc.type='sine';
      osc.frequency.setValueAtTime(523.25,t);
      osc.frequency.setValueAtTime(659.25,t+0.09);
      osc.frequency.setValueAtTime(783.99,t+0.18);
      osc.frequency.setValueAtTime(1046.50,t+0.28);
      gain.gain.setValueAtTime(0.2,t);
      gain.gain.exponentialRampToValueAtTime(0.001,t+0.55);
      osc.start(t);osc.stop(t+0.55);
    }
  }catch(e){}
}`;

const newSfxEnd = `    }else if(type==='victory'){
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
}`;

if (src.includes(oldSfxEnd)) {
  src = src.replace(oldSfxEnd, newSfxEnd);
  console.log('Added emergency and achievement audio synthesizers.');
} else {
  console.log('Notice: SFX block already updated or not matched.');
}

// 2. Add ACHIEVEMENTS and TIMED_SCENARIOS right before missions
const targetBeforeMissions = 'var missions=[';
const achievementsAndScenariosCode = `var ACHIEVEMENTS = [
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
    promptClue: 'Enforce Shield firewall deny rule, run Guard Heuristic scan, or issue \`quarantine-component\` in Terminal!'
  },
  {
    id: 'emg-ddos',
    title: 'VOLUMETRIC SYN FLOOD SATURATION',
    threatType: 'Asymmetric Edge Flood',
    seconds: 60,
    brief: 'Volumetric packet flood hitting edge-route-01! Public gateway is approaching complete queue lockup. Deploy adaptive rate shaping in Shield or issue \`route-select edge-route-02\` before gateway drops!',
    defusalActions: ['change:shield-rate-policy', 'terminal:route-select'],
    promptClue: 'Toggle adaptive rate shaping in Shield or issue \`route-select edge-route-02\` in Terminal!'
  },
  {
    id: 'emg-merkle',
    title: 'STEALTH ROOTKIT CORRUPTION ATTACK',
    threatType: 'Kernel Inode Tampering',
    seconds: 80,
    brief: 'Adversary injected corrupted inode into root filesystem block 4! Execute atomic Merkle rollback to clean restore point before system integrity collapses!',
    defusalActions: ['click:merkle-rollback', 'terminal:verify-root'],
    promptClue: 'Open Merkle Filesystem and click Rollback Checkpoint, or run \`verify-root\` in Terminal!'
  }
];

`;

if (!src.includes('var ACHIEVEMENTS = [')) {
  src = src.replace(targetBeforeMissions, achievementsAndScenariosCode + targetBeforeMissions);
  console.log('Inserted ACHIEVEMENTS and TIMED_SCENARIOS.');
}

// 3. Update fresh() and ensureProfileState()
const oldFresh = "function fresh(){return {v:1,index:0,started:false,done:[],evidence:{},fragments:{},decision:{},answers:{},stats:{xp:0,credits:0,research:0,score:0,history:[]},log:[]};}";
const newFresh = "function fresh(){return {v:1,index:0,started:false,done:[],evidence:{},fragments:{},decision:{},answers:{},stats:{xp:0,credits:0,research:0,score:0,history:[]},achievements:[],log:[]};}";
if (src.includes(oldFresh)) {
  src = src.replace(oldFresh, newFresh);
  console.log('Updated fresh() with achievements.');
}

const oldEnsure = "function ensureProfileState(){if(!state.answers)state.answers={};if(!state.stats)state.stats={xp:0,credits:0,research:0,score:0,history:[]};if(!Array.isArray(state.stats.history))state.stats.history=[];if(!state.fragments)state.fragments={};}";
const newEnsure = "function ensureProfileState(){if(!state.answers)state.answers={};if(!state.stats)state.stats={xp:0,credits:0,research:0,score:0,history:[]};if(!Array.isArray(state.stats.history))state.stats.history=[];if(!state.fragments)state.fragments={};if(!Array.isArray(state.achievements))state.achievements=[];}";
if (src.includes(oldEnsure)) {
  src = src.replace(oldEnsure, newEnsure);
  console.log('Updated ensureProfileState() with achievements.');
}

// 4. Inject unlockAchievement, achievementToast, and emergency engine right after toast definition
const targetToast = "function toast(title,desc,grade){if(window.AegisSystemNotify){window.AegisSystemNotify(title,desc,'info');return;}var n=el('div','aegis-native-toast');var head=el('b','',title);if(grade){var badge=el('span','mission-grade-badge grade-'+grade.toLowerCase(),'GRADE '+grade);head.append(badge);}n.append(head,el('span','',desc));document.body.append(n);setTimeout(function(){n.remove();},5000);}";
const achievementAndEmergencyMethods = `

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
    { q:'Can I use terminal commands?', a:'Yes! Running \`quarantine-component\`, \`verify-containment\`, or \`route-select edge-route-02\` directly counters the attack.' },
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
    { q:'Initiating emergency rollback now.', a:'Click Rollback Checkpoint in Merkle Filesystem or run \`verify-root\` in Terminal.' }
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
`;

if (!src.includes('function unlockAchievement(id)')) {
  src = src.replace(targetToast, targetToast + achievementAndEmergencyMethods);
  console.log('Injected unlockAchievement, emergency engine, and view helpers.');
}

// 5. Update render() to support tabs
const oldRenderHead = "function render(){\n if(!body)return;var m=mission(),list=el('ol','mission-objectives');body.replaceChildren();";
const newRenderHead = `function render(){\n if(!body)return;updateTabNav();body.replaceChildren();\n if(currentTab==='achievements'){renderAchievementsView();return;}\n if(currentTab==='drills'){renderDrillsView();return;}\n var m=mission(),list=el('ol','mission-objectives');`;

if (src.includes(oldRenderHead)) {
  src = src.replace(oldRenderHead, newRenderHead);
  console.log('Updated render() to support currentTab routing.');
}

// 6. Update observe() to check emergency defusal and auto-achievements
const oldObserveHead = `function observe(type,value){
  if(!authenticated||!state.started||state.done.includes(mission().id))return;`;

const newObserveHead = `function observe(type,value){
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

  if(!authenticated||!state.started||state.done.includes(mission().id))return;`;

if (src.includes(oldObserveHead)) {
  src = src.replace(oldObserveHead, newObserveHead);
  console.log('Updated observe() with emergency defusal & auto achievements.');
}

// Check first breach unlock on milestone
const oldMilestoneTrigger = `    if(milestoneIndex>=0){
      playSfx('milestone');`;
const newMilestoneTrigger = `    if(milestoneIndex>=0){
      playSfx('milestone');
      if(evidence().length===1)unlockAchievement('first_breach');`;
if (src.includes(oldMilestoneTrigger)) {
  src = src.replace(oldMilestoneTrigger, newMilestoneTrigger);
  console.log('Added first_breach unlock on milestone.');
}

// 7. Update complete() to check forensic_genius, director_commendation, and citadel_sealed
const oldCompleteSeals = `  record('Evidence-backed incident report sealed. Grade '+grade+' / '+points+' command points.');`;
const newCompleteSeals = `  record('Evidence-backed incident report sealed. Grade '+grade+' / '+points+' command points.');
  if(answersCorrect(m))unlockAchievement('forensic_genius');
  if(grade==='S')unlockAchievement('director_commendation');
  if(state.done.length===3)unlockAchievement('citadel_sealed');`;
if (src.includes(oldCompleteSeals)) {
  src = src.replace(oldCompleteSeals, newCompleteSeals);
  console.log('Updated complete() with case completion achievements.');
}

// 8. Update build() to add tab navigation
const oldBuildPanel = `  top.append(brand,actions);
  body=el('div','aegis-mission-body');panel.append(top,body);`;

const newBuildPanel = `  top.append(brand,actions);
  tabNav=el('div','aegis-dossier-nav');
  var bCase=el('button','aegis-dossier-tab active','📁 CASES');
  var bAch=el('button','aegis-dossier-tab','🏅 MEDALS');
  var bDrill=el('button','aegis-dossier-tab','⚡ LIVE DRILLS');
  bCase.addEventListener('click',function(){currentTab='cases';render();});
  bAch.addEventListener('click',function(){currentTab='achievements';render();});
  bDrill.addEventListener('click',function(){currentTab='drills';render();});
  tabNav.append(bCase,bAch,bDrill);
  body=el('div','aegis-mission-body');panel.append(top,tabNav,body);`;

if (src.includes(oldBuildPanel)) {
  src = src.replace(oldBuildPanel, newBuildPanel);
  console.log('Added dossier tab navigation to build().');
}

// 9. Update aegis:terminal-command listener
const oldTermListener = `document.addEventListener('aegis:terminal-command',function(e){var c=String(e.detail&&e.detail.command||'').toLowerCase();['ps','netstat','ss','journalctl','capability-audit','verify-containment','verify-root','quarantine-component','service-health','route-health','route-select'].forEach(function(k){if(c.indexOf(k)>=0)observe('terminal',k);});});`;
const newTermListener = `document.addEventListener('aegis:terminal-command',function(e){
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
  ['ps','netstat','ss','journalctl','capability-audit','verify-containment','verify-root','quarantine-component','service-health','route-health','route-select'].forEach(function(k){if(c.indexOf(k)>=0)observe('terminal',k);});
});`;

if (src.includes(oldTermListener)) {
  src = src.replace(oldTermListener, newTermListener);
  console.log('Updated terminal command listener for achievements and drills.');
}

// 10. Update window.AegisCampaign export
const oldExport = `window.AegisCampaign={open:function(){panel.hidden=false;render();},openCopilot:function(){if(copilotPopup){copilotPopup.hidden=false;copilotPopup.classList.remove('is-minimized');if(copilotOrb)copilotOrb.hidden=true;}},get state(){return JSON.parse(JSON.stringify(state));},reset:function(){if(confirm('Reset local campaign progress?')){state=fresh();save();render();start();}}};`;
const newExport = `window.AegisCampaign={
  open:function(){panel.hidden=false;render();},
  openAchievements:function(){currentTab='achievements';panel.hidden=false;render();},
  openDrills:function(){currentTab='drills';panel.hidden=false;render();},
  triggerTimedAttack:triggerTimedAttack,
  unlockAchievement:unlockAchievement,
  openCopilot:function(){if(copilotPopup){copilotPopup.hidden=false;copilotPopup.classList.remove('is-minimized');if(copilotOrb)copilotOrb.hidden=true;}},
  get state(){return JSON.parse(JSON.stringify(state));},
  reset:function(){if(confirm('Reset local campaign progress?')){state=fresh();save();render();start();}}
};`;

if (src.includes(oldExport)) {
  src = src.replace(oldExport, newExport);
  console.log('Updated window.AegisCampaign export.');
}

fs.writeFileSync(srcPath, src, 'utf8');
const destPublic = path.join(__dirname, '..', 'public', 'native-campaign.js');
fs.writeFileSync(destPublic, src, 'utf8');
console.log('Saved native-campaign.js to both simulator/web and public.');
