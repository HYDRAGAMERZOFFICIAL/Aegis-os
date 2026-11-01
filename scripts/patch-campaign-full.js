const fs = require('fs');
const path = require('path');

const campaignFile = path.join(__dirname, '..', 'simulator', 'web', 'native-campaign.js');
let code = fs.readFileSync(campaignFile, 'utf8');

// 1. Add practice drill scenarios and practice mode variable
const practiceVariables = `
var practiceMode = false;
var randomEventTimer = null;
var breachCountdownInterval = null;
var breachSecondsLeft = 30.0;
var tutCurrentStep = 0;
`;

if (!code.includes('var practiceMode = false;')) {
    code = code.replace('var activeEmergency=null,emergencyInterval=null', `${practiceVariables}\nvar activeEmergency=null,emergencyInterval=null`);
}

// 2. Add Critical 30-Second Breach Countdown function
const criticalBreachCode = `
function triggerCriticalBreachCountdown(reason) {
  var overlay = document.getElementById('aegis-critical-breach-overlay');
  var clock = document.getElementById('breach-timer-clock');
  var input = document.getElementById('breach-override-input');
  var fixBtn = document.getElementById('btn-breach-fix');
  var status = document.getElementById('breach-status');
  if (!overlay || !clock) return;

  document.body.classList.add('system-breached-critical');
  overlay.classList.remove('hidden');
  playSfx('emergency');

  breachSecondsLeft = 30.0;
  clock.textContent = '00:30.00';
  if (status) status.textContent = reason || 'SYSTEM LOCKDOWN IN PROGRESS. DEPLOY DEFENSE OVERRIDE!';
  if (input) input.value = '';

  if (breachCountdownInterval) clearInterval(breachCountdownInterval);
  var started = Date.now();

  breachCountdownInterval = setInterval(function() {
    var elapsed = (Date.now() - started) / 1000;
    var remain = Math.max(0, 30.0 - elapsed);
    breachSecondsLeft = remain;
    var sec = Math.floor(remain);
    var ms = Math.floor((remain - sec) * 100);
    clock.textContent = '00:' + String(sec).padStart(2, '0') + '.' + String(ms).padStart(2, '0');

    if (remain <= 0) {
      clearInterval(breachCountdownInterval);
      breachSecondsLeft = 0;
      executeSystemWipeAndEmergencyRecovery();
    }
  }, 50);

  if (fixBtn && !fixBtn._bound) {
    fixBtn._bound = true;
    fixBtn.addEventListener('click', function() {
      var cipher = String(input ? input.value : '').trim().toUpperCase();
      // Valid override code or any quick-fix entry
      if (cipher === 'AEGIS-OVERRIDE-SEC99' || cipher.length >= 6 || cipher.includes('OVERRIDE') || cipher.includes('FIX')) {
        clearInterval(breachCountdownInterval);
        document.body.classList.remove('system-breached-critical');
        overlay.classList.add('hidden');
        playSfx('victory');
        toast('BREACH DEFUSED // SYSTEM SAVED!', 'Emergency microkernel override accepted with ' + breachSecondsLeft.toFixed(1) + 's remaining!', 'S');
        showAiDialogue('CRITICAL BREACH DEFUSED // GRADE S', 'Outstanding work, Operator! You defused the ring-0 breach in the nick of time. Microkernel state preserved.', [
          { q: 'Resume standard operations →', a: 'Tactical station restored to normal status.' }
        ]);
        unlockAchievement('timed_hero');
        unlockAchievement('rapid_triage');
        state.stats.xp += 200;
        state.stats.credits += 300;
        save();
        render();
      } else {
        playSfx('alert');
        if (status) status.textContent = 'INVALID CIPHER! Try entering code: AEGIS-OVERRIDE-SEC99';
      }
    });
  }
}

function executeSystemWipeAndEmergencyRecovery() {
  playSfx('alert');
  fetch('/api/system/trigger-wiped', { method: 'POST' }).catch(function(){});
  var overlay = document.getElementById('aegis-critical-breach-overlay');
  if (overlay) {
    overlay.innerHTML = '<div class="breach-panel" style="border-color:#ff0000;box-shadow:0 0 80px #ff0000;">' +
      '<div class="breach-header"><span class="breach-skull">💥</span><span class="breach-title">SYSTEM COMPROMISED // INODES WIPED</span></div>' +
      '<div class="breach-subtitle" style="color:#ff6677;">Adversary zero-day purged local userland storage tables!</div>' +
      '<div class="breach-warning">Microkernel severed corrupted storage inodes to protect core firmware. All data files in /userland/data/ were wiped and moved to the corrupted manifest.</div>' +
      '<div style="margin:20px 0;"><button type="button" class="btn-primary" id="btn-goto-recovery" style="padding:12px 24px;font-size:13px;font-weight:700;">PROCEED TO EMERGENCY FILE RECOVERY →</button></div>' +
      '</div>';
    document.getElementById('btn-goto-recovery')?.addEventListener('click', function() {
      overlay.classList.add('hidden');
      document.body.classList.remove('system-breached-critical');
      toast('EMERGENCY RECOVERY ACTIVATED', 'Use File Recovery & Hex Carver or Merkle Rollback to restore your wiped files.', 'D');
      showAiDialogue('EMERGENCY RECOVERY MISSION // ACTION REQUIRED', 'Adversary payload wiped your local storage inodes! Open Files & Storage and switch to [🔬 File Recovery & Hex Carver]. Check the damaged inode offsets, enter the magic bytes, and restore your files!', [
        { q: 'How do I carve the wiped files?', a: 'In Files & Storage, click on user_vault.dat.corrupt, enter the magic bytes (56 41 55 4c) and click Carve & Restore Inode.' },
        { q: 'Can I also use Merkle rollback?', a: 'Yes! Open Self-Healing Merkle Filesystem and execute Rollback Checkpoint to revert to chkpt-0001.' }
      ]);
      if (window.WM) window.WM.open('files', 'Files & Storage Manager', 820, 530);
    });
  }
}
`;

if (!code.includes('function triggerCriticalBreachCountdown')) {
    code = code.replace('function emergencyTimeOut(){', `${criticalBreachCode}\nfunction emergencyTimeOut(){`);
}

// 3. Update emergencyTimeOut to trigger the 30s critical countdown
const oldTimeout = `function emergencyTimeOut(){
  playSfx('alert');
  toast('CRISIS BREACH SUSTAINED','The emergency attack timer expired before mitigation was completed. Corrupted block detected.','D');
  showAiDialogue('EMERGENCY BREACH SUSTAINED','The timer expired before countermeasures were deployed! Adversary managed to compromise filesystem state. Open Merkle Filesystem and execute an atomic rollback to restore clean integrity.',[
    { q:'Initiating emergency rollback now.', a:'Click Rollback Checkpoint in Merkle Filesystem or run \`verify-root\` in Terminal.' }
  ]);
  activeEmergency=null;
  removeEmergencyBanner();
  render();
}`;

const newTimeout = `function emergencyTimeOut(){
  playSfx('alert');
  removeEmergencyBanner();
  activeEmergency=null;
  render();
  if (!practiceMode) {
    triggerCriticalBreachCountdown('TIMED EMERGENCY ATTACK FAILED: ZERO-DAY PAYLOAD ACTIVATED!');
  } else {
    toast('PRACTICE DRILL ENDED', 'Practice drill concluded. Review guided countermeasures.', 'info');
    showAiDialogue('PRACTICE DRILL CONCLUDED', 'In live combat, timer expiration triggers a 30s critical lockdown. Review the defensive tools in your arsenal and try again!', [
      { q: 'Got it, Operator.', a: 'Standing by.' }
    ]);
  }
}`;

code = code.replace(oldTimeout, newTimeout);

// 4. Update observe() to strictly enforce sequential progression (Step N must be completed before Step N+1)
const oldObserveProgression = `  var m=mission(),changed=false,milestoneIndex=-1;
  m.steps.forEach(function(s,i){
    if(!Array.isArray(s[3])||evidence().includes(i))return;
    var progress=fragments()[i]||(fragments()[i]=[]);
    s[3].forEach(function(group,gi){
      if(progress.includes(gi))return;
      var matched=group.split('|').some(function(token){var p=token.split(':');return p[0]===type&&(p.slice(1).join(':')==='*'||p.slice(1).join(':')===value);});
      if(matched){progress.push(gi);record('Evidence source recorded for: '+s[0]);changed=true;}
    });
    if(progress.length===s[3].length){evidence().push(i);record('Evidence milestone secured: '+s[0]);milestoneIndex=i;}
  });`;

const newObserveProgression = `  var m=mission(),changed=false,milestoneIndex=-1;
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
    if (progress.length === s[3].length) {
      evidence().push(activeStepIndex);
      record('Evidence milestone secured: ' + s[0]);
      milestoneIndex = activeStepIndex;
    }
  }`;

code = code.replace(oldObserveProgression, newObserveProgression);

// 5. Update render() to display locked indicator on future steps
const oldStepRender = `    var li=el('li','mission-step-item'+(done?' is-done':'')+(open?' is-open':''));
    var marker=el('span','step-marker',done?'✓':String(i+1));
    var info=el('div','step-info');
    info.append(el('strong','',s[0]),el('p','',s[1]));`;

const newStepRender = `    var activeStepIdx = m.steps.findIndex(function(st, idx) { return Array.isArray(st[3]) && !evidence().includes(idx); });
    var isLocked = !done && activeStepIdx !== -1 && i > activeStepIdx;
    var li=el('li','mission-step-item'+(done?' is-done':'')+(open?' is-open':'')+(isLocked?' is-locked':''));
    var marker=el('span','step-marker',done?'✓':isLocked?'🔒':String(i+1));
    var info=el('div','step-info');
    var titleRow=el('div','step-title-row');
    titleRow.append(el('strong','',s[0]));
    if(isLocked) titleRow.append(el('span','step-lock-badge','🔒 LOCKED (Complete Step '+(activeStepIdx+1)+' first)'));
    info.append(titleRow,el('p','',s[1]));`;

code = code.replace(oldStepRender, newStepRender);

// 6. Add Tutorial controller functions
const tutorialFunctions = `
function openTutorial() {
  var overlay = document.getElementById('aegis-tutorial-overlay');
  if (!overlay) return;
  overlay.classList.remove('hidden');
  tutCurrentStep = 0;
  showTutStep(0);

  var closeBtn = document.getElementById('btn-close-tutorial');
  var prevBtn = document.getElementById('btn-tut-prev');
  var nextBtn = document.getElementById('btn-tut-next');
  var completeBtn = document.getElementById('btn-tut-complete');
  var tabs = overlay.querySelectorAll('.tutorial-tab');

  tabs.forEach(function(t, idx) {
    t.onclick = function() { showTutStep(idx); };
  });

  if (closeBtn) closeBtn.onclick = function() { overlay.classList.add('hidden'); };
  if (prevBtn) prevBtn.onclick = function() { if (tutCurrentStep > 0) showTutStep(tutCurrentStep - 1); };
  if (nextBtn) nextBtn.onclick = function() { if (tutCurrentStep < 4) showTutStep(tutCurrentStep + 1); };
  if (completeBtn) completeBtn.onclick = function() {
    overlay.classList.add('hidden');
    state.tutorialCompleted = true;
    save();
    try {
      if (window.AegisAuth && window.AegisAuth.user) {
        localStorage.setItem('aegis_tut_' + window.AegisAuth.user.id, 'true');
      }
    } catch (e) {}
    toast('TUTORIAL COMPLETED', 'Tactical briefing complete. You are certified for live operational duty.', 'S');
    playSfx('milestone');
  };
}

function showTutStep(idx) {
  tutCurrentStep = idx;
  var overlay = document.getElementById('aegis-tutorial-overlay');
  if (!overlay) return;
  var tabs = overlay.querySelectorAll('.tutorial-tab');
  var panels = overlay.querySelectorAll('.tutorial-panel');
  var prevBtn = document.getElementById('btn-tut-prev');
  var nextBtn = document.getElementById('btn-tut-next');
  var completeBtn = document.getElementById('btn-tut-complete');
  var indicator = document.getElementById('tut-page-indicator');

  tabs.forEach(function(t, i) { t.classList.toggle('active', i === idx); });
  panels.forEach(function(p, i) { p.classList.toggle('hidden', i !== idx); });

  if (prevBtn) prevBtn.disabled = (idx === 0);
  if (nextBtn) nextBtn.classList.toggle('hidden', idx === 4);
  if (completeBtn) completeBtn.classList.toggle('hidden', idx !== 4);
  if (indicator) indicator.textContent = 'Step ' + (idx + 1) + ' of 5';
}
`;

if (!code.includes('function openTutorial()')) {
    code = code.replace('function renderAchievementsView(){', `${tutorialFunctions}\nfunction renderAchievementsView(){`);
}

// 7. Add Random Cyber Events Scheduler
const randomEventScheduler = `
var RANDOM_CYBER_EVENTS = [
  { id: 'port_probe', title: 'Unauthorized Port Probe', desc: 'Inbound port scan detected from 45.33.32.156 targeting ports 22, 80, 8443.', actionHint: 'Inspect Firewall Rules in Aegis Shield or run netstat.' },
  { id: 'rogue_thread', title: 'Rogue Thread Privilege Spike', desc: 'Unknown worker PID 5188 attempted ring 3 capability escalation.', actionHint: 'Inspect Neural Monitor or run capability-audit.' },
  { id: 'canary_trip', title: 'Canary Tripwire Alert', desc: 'File modification tripwire tripped in /var/cache/canary.log.', actionHint: 'Inspect Files or run verify-root in Terminal.' },
  { id: 'dns_spoof', title: 'Suspicious DNS Spoofing Intercepted', desc: 'Unverified DNS response on UDP 853 intercepted by stateful shield filter.', actionHint: 'Check CyberSec Deep Packet Inspector stream.' },
  { id: 'dpi_payload', title: 'Hostile NOP Sled Intercepted', desc: 'Stateful DPI caught hex payload containing 0x90 shellcode sled.', actionHint: 'Detonate sample in AI Threat Sandbox.' }
];

function triggerRandomCyberEvent() {
  if (activeEmergency) return;
  var ev = RANDOM_CYBER_EVENTS[Math.floor(Math.random() * RANDOM_CYBER_EVENTS.length)];
  playSfx('alert');
  toast('🚨 RANDOM CYBER EVENT // ' + ev.title, ev.desc, 'A');
  showAiDialogue('SECURITY TELEMETRY ALERT // ' + ev.title, '⚠️ AD HOC INCIDENT DETECTED: ' + ev.desc + ' Recommended response: ' + ev.actionHint, [
    { q: 'Investigate incident in tools', a: ev.actionHint },
    { q: 'Log observation to Case Notes', a: 'Opening Operator Case Notes...' },
    { q: 'Dismiss telemetry alert', a: 'Alert acknowledged and logged.' }
  ]);
}

function startRandomEventScheduler() {
  if (randomEventTimer) clearTimeout(randomEventTimer);
  var nextDelayMs = Math.floor(90000 + Math.random() * 120000); // 1.5 - 3.5 minutes
  randomEventTimer = setTimeout(function() {
    triggerRandomCyberEvent();
    startRandomEventScheduler();
  }, nextDelayMs);
}
`;

if (!code.includes('function startRandomEventScheduler()')) {
    code = code.replace('function renderDrillsView(){', `${randomEventScheduler}\nfunction renderDrillsView(){`);
}

// 8. Update renderDrillsView to include Practice Mode toggle
const oldDrillsHeader = `  var topBox=el('div','aegis-achievement-summary');
  topBox.append(
    el('h3','','⚡ LIVE EMERGENCY DEFENSE SIMULATOR'),
    el('p','','Simulate real-time cybersecurity zero-day attacks and test your response speed under pressure.')
  );
  body.append(topBox);`;

const newDrillsHeader = `  var topBox=el('div','aegis-achievement-summary');
  var modeBtn = el('button', 'btn-action-small', practiceMode ? '🛡️ MODE: PRACTICE (GUIDED)' : '⚡ MODE: LIVE COMBAT (TIMED)');
  modeBtn.style.float = 'right';
  modeBtn.style.borderColor = practiceMode ? '#22dd66' : '#ff5577';
  modeBtn.style.color = practiceMode ? '#22dd66' : '#ff5577';
  modeBtn.onclick = function() {
    practiceMode = !practiceMode;
    toast(practiceMode ? 'PRACTICE MODE ACTIVE' : 'LIVE COMBAT MODE ACTIVE', practiceMode ? 'Unlimited time & guided hints enabled.' : 'Live 30s lockdown penalties enabled.');
    render();
  };
  var headerRow = el('div', '', '');
  headerRow.style.display = 'flex';
  headerRow.style.justifyContent = 'space-between';
  headerRow.style.alignItems = 'center';
  headerRow.append(el('h3','','⚡ LIVE EMERGENCY DEFENSE SIMULATOR'), modeBtn);

  topBox.append(
    headerRow,
    el('p','', practiceMode ? 'PRACTICE MODE: Train your response with guided step-by-step hints and zero failure penalty.' : 'LIVE COMBAT MODE: Real-time countdowns. Failure triggers a 30s critical system breach!')
  );
  body.append(topBox);`;

code = code.replace(oldDrillsHeader, newDrillsHeader);

// 9. Update on authenticated to show Welcome/Welcome-Back in AI Copilot and launch tutorial once
const oldAuthListener = `document.addEventListener('aegis:authenticated',async function(){authenticated=true;syncing=true;try{var response=await fetch('/api/campaign/state',{credentials:'same-origin'});if(response.ok){var payload=await response.json();state=payload.state&&payload.state.v===1?payload.state:fresh();ensureProfileState();}else state=fresh();}catch(e){state=fresh();}ensureProfileState();syncing=false;save();panel.hidden=true;render();if(!state.started&&!state.done.includes(mission().id))start();});`;

const newAuthListener = `document.addEventListener('aegis:authenticated',async function(e){
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

  startRandomEventScheduler();

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
    if (!state.tutorialCompleted) {
      setTimeout(openTutorial, 600);
    }
  } else {
    showAiDialogue('WELCOME BACK, ' + (user.displayName || user.username).toUpperCase(), '👋 Welcome back, Operator! Let\\'s continue where you had left off. Active theater: Case #' + (state.index + 1) + ' (' + mission().title + '). Current priority objective: ' + activeStepName + '. ⚠️ URGENT: System telemetry is live—solve pending diagnostics swiftly!', [
      { q: 'Show active objective details', a: activeStepClue },
      { q: 'Open Operator Case Notes', a: 'Opening Case Notes...' },
      { q: 'Review Defense Tutorial', a: 'Opening tutorial...' }
    ]);
  }
});`;

code = code.replace(oldAuthListener, newAuthListener);

// 10. Expose openTutorial and triggerCriticalBreach on window.AegisCampaign
if (!code.includes('openTutorial: openTutorial')) {
    code = code.replace(
        'openCopilot:function(){if(copilotPopup){copilotPopup.hidden=false;copilotPopup.classList.remove(\'is-minimized\');if(copilotOrb)copilotOrb.hidden=true;}},',
        'openCopilot:function(){if(copilotPopup){copilotPopup.hidden=false;copilotPopup.classList.remove(\'is-minimized\');if(copilotOrb)copilotOrb.hidden=true;}},\n  openTutorial: openTutorial,\n  triggerCriticalBreach: triggerCriticalBreachCountdown,'
    );
}

fs.writeFileSync(campaignFile, code, 'utf8');
console.log('Successfully updated simulator/web/native-campaign.js with breach countdown, sequential step lock, tutorial, and random events.');

