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
    }
  }catch(e){}
}

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
function fresh(){return {v:1,index:0,started:false,done:[],evidence:{},fragments:{},decision:{},answers:{},stats:{xp:0,credits:0,research:0,score:0,history:[]},log:[]};}
var state=fresh(),authenticated=false,syncing=false,saveTimer=null;
if(!state||state.v!==1)state=fresh();
function save(){if(!authenticated)return;try{localStorage.setItem(KEY,JSON.stringify(state));}catch(e){}if(saveTimer)clearTimeout(saveTimer);saveTimer=setTimeout(function(){fetch('/api/campaign/state',{method:'POST',credentials:'same-origin',headers:{'Content-Type':'application/json'},body:JSON.stringify({state:state})}).catch(function(){});},250);}
function mission(){return missions[Math.min(state.index,missions.length-1)];}
function evidence(){var m=mission();return state.evidence[m.id]||(state.evidence[m.id]=[]);}
function fragments(){var m=mission();if(!state.fragments)state.fragments={};return state.fragments[m.id]||(state.fragments[m.id]={});}
function answersCorrect(m){var values=state.answers&&state.answers[m.id]||{};return !m.quiz||m.quiz.every(function(q){var value=String(values[q.id]||'').toLowerCase().replace(/[^a-z0-9]+/g,'');return q.accept.some(function(a){return value===a.toLowerCase().replace(/[^a-z0-9]+/g,'');});});}
function ensureProfileState(){if(!state.answers)state.answers={};if(!state.stats)state.stats={xp:0,credits:0,research:0,score:0,history:[]};if(!Array.isArray(state.stats.history))state.stats.history=[];if(!state.fragments)state.fragments={};}
function record(s){state.log.push({id:mission().id,t:new Date().toLocaleTimeString(),s:s});if(state.log.length>200)state.log=state.log.slice(-200);save();}
function el(tag,cls,txt){var n=document.createElement(tag);if(cls)n.className=cls;if(txt!==undefined)n.textContent=txt;return n;}
var panel,body,count,sfxBtn;
function toast(title,desc,grade){if(window.AegisSystemNotify){window.AegisSystemNotify(title,desc,'info');return;}var n=el('div','aegis-native-toast');var head=el('b','',title);if(grade){var badge=el('span','mission-grade-badge grade-'+grade.toLowerCase(),'GRADE '+grade);head.append(badge);}n.append(head,el('span','',desc));document.body.append(n);setTimeout(function(){n.remove();},5000);}

function render(){
 if(!body)return;var m=mission(),list=el('ol','mission-objectives');body.replaceChildren();
 var head=el('div','mission-operation-top');
 head.append(el('span','mission-kicker','AEGIS COMMAND // OPERATION '+(state.index+1)+' OF 3'),el('h2','',m.title));
 var comm=el('div','mission-comm-dispatch');
 comm.append(el('small','','TACTICAL COMMS // DIRECTOR VANCE'),el('span','','"'+m.brief+'"'));
 head.append(comm,el('p','mission-pressure',m.pressure));
 var done=0;m.steps.forEach(function(s,i){var kind=s[3],complete=kind==='choice'?!!state.decision[m.id]:kind==='report'?state.done.includes(m.id):evidence().includes(i);if(complete)done++;});
 head.append(el('div','mission-progress-label',done+' / '+m.steps.length+' investigative milestones'));
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
  if(!authenticated||!state.started||state.done.includes(mission().id))return;
  var m=mission(),changed=false,milestoneHit=false;
  m.steps.forEach(function(s,i){
    if(!Array.isArray(s[3])||evidence().includes(i))return;
    var progress=fragments()[i]||(fragments()[i]=[]);
    s[3].forEach(function(group,gi){
      if(progress.includes(gi))return;
      var matched=group.split('|').some(function(token){var p=token.split(':');return p[0]===type&&(p.slice(1).join(':')==='*'||p.slice(1).join(':')===value);});
      if(matched){progress.push(gi);record('Evidence source recorded for: '+s[0]);changed=true;}
    });
    if(progress.length===s[3].length){evidence().push(i);record('Evidence milestone secured: '+s[0]);milestoneHit=true;}
  });
  if(changed){
    var toggle=document.querySelector('.aegis-mission-toggle');
    if(toggle){toggle.classList.add('has-pulse');setTimeout(function(){toggle.classList.remove('has-pulse');},1600);}
    if(milestoneHit)playSfx('milestone');else playSfx('clue');
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
  playSfx('victory');
  var outcome=m.outcomes[decision];
  toast('OPERATION SECURED // GRADE '+grade,outcome,grade);
  if(state.done.length===3){render();toast('CAMPAIGN COMPLETE','All three operations secured. XP '+state.stats.xp+' · Credits '+state.stats.credits+' · Research '+state.stats.research+'.','S');return;}
  state.index++;state.started=false;save();render();setTimeout(start,1200);
}

function start(){
  if(!authenticated||state.started||state.done.includes(mission().id))return;
  state.started=true;save();playSfx('alert');render();
  toast('NEW INCIDENT // '+mission().title,'Review the alert. Investigate through the native AegisOS applications.');
}

function build(){
  var link=document.createElement('link');link.rel='stylesheet';link.href='/native-campaign.css';document.head.append(link);
  var host=el('aside','aegis-mission-hud');host.id='aegis-mission-hud';
  var toggle=el('button','aegis-mission-toggle');toggle.type='button';toggle.title='Toggle Aegis Tactical Operations (Hotkey: F4)';
  toggle.innerHTML='<span class="mission-pulse"></span> OPERATIONS <b id="aegis-mission-count"></b>';
  count=toggle.querySelector('b');
  panel=el('section','aegis-mission-panel');panel.hidden=true;
  var top=el('div','aegis-mission-head'),brand=el('div','');
  brand.append(el('small','','AEGIS COMMAND // LIVE INCIDENT'),el('strong','','FIELD OPERATIONS'));
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
  body=el('div','aegis-mission-body');panel.append(top,body);
  host.append(toggle,panel);document.body.append(host);
  toggle.addEventListener('click',function(){panel.hidden=!panel.hidden;render();});
  close.addEventListener('click',function(){panel.hidden=true;});
  window.addEventListener('keydown',function(e){
    if(e.key==='F4'){e.preventDefault();panel.hidden=!panel.hidden;render();}
    if(e.key==='m'||e.key==='M'){
      var tag=document.activeElement&&document.activeElement.tagName;
      if(tag!=='INPUT'&&tag!=='TEXTAREA'){sfxBtn.click();}
    }
  });
  render();
}

document.addEventListener('aegis:desktop-event',function(e){var d=e.detail||{};observe(d.type||'',String(d.value||''));});
document.addEventListener('aegis:ai-prompt',function(e){var p=String(e.detail&&e.detail.prompt||'').toLowerCase();if(/correlat|timeline|root cause/.test(p)){observe('click','ai-correlate-timeline');observe('click','ai-correlate-kernel');observe('click','ai-correlate-traffic');}});
document.addEventListener('aegis:terminal-command',function(e){var c=String(e.detail&&e.detail.command||'').toLowerCase();['ps','netstat','ss','journalctl','capability-audit','verify-containment','verify-root','quarantine-component','service-health','route-health','route-select'].forEach(function(k){if(c.indexOf(k)>=0)observe('terminal',k);});});
document.addEventListener('click',function(e){var b=e.target.closest('button,[role="button"],tr,.packet-row,.process-row,.process-card,.event-row,.merkle-block-card,.metric-gauge');if(!b)return;var id=b.id||'',txt=(b.textContent||'').toLowerCase(),app=b.closest('.aegis-window')&&b.closest('.aegis-window').dataset.appId;var map={'btn-run-vuln-scan':'cybersec-vulnerability-scan','btn-scan-guard':'shield-guard-scan','btn-rollback-merkle':'merkle-rollback','btn-corrupt-merkle':'merkle-corrupt','btn-refresh-files':'files-refresh','btn-run-trim-optimize':'optimizer-resource-audit'};if(map[id])observe('click',map[id]);if(app==='neural'){if(b.closest('.process-row,.process-card,.event-row')||/process detail|inspect process/.test(txt))observe('click','neural-process-detail');if(b.closest('.event-row')||/memory|privilege|address space/.test(txt))observe('click','neural-memory-map');}if(app==='cybersec'){if(b.closest('.packet-row')||/packet|sniffer|traffic feed/.test(txt)||id==='tab-packets')observe('click','cybersec-packet-inspect');if(/kernel patch|apply patch|patch signature/.test(txt))observe('click','cybersec-apply-kernel-patch');}if(app==='shield'){if(/token|capability|zero.trust/.test(txt)||id==='tab-zt-tokens')observe('click','shield-token-audit');if(/firewall|edge|rule/.test(txt)||id==='tab-firewall')observe('click','shield-edge-audit');if(/reroute|route 02|resilient route/.test(txt))observe('click','shield-reroute');}if(app==='merkle'&&(/integrity|merkle root|verify|block hash/.test(txt)||b.classList.contains('merkle-block-card')))observe('click','merkle-integrity-scan');if(app==='files'&&b.closest('tr')&&/suspicious|service|stager|sync-cache/.test(b.closest('tr').textContent.toLowerCase()))observe('click','files-inspect-suspicious');if(app==='files'&&b.closest('tr')&&/libaegis_kernel/.test(b.closest('tr').textContent))observe('click','files-inspect-library');if(app==='ai'&&/correlate|analyse|analyze|timeline|root cause/.test(txt)){observe('click','ai-correlate-timeline');observe('click','ai-correlate-kernel');observe('click','ai-correlate-traffic');}if(app==='optimizer'&&(b.closest('.metric-gauge')||/resource|performance|telemetry|refresh/.test(txt)))observe('click','optimizer-resource-audit');},true);
document.addEventListener('change',function(e){if(e.target.matches('.rule-toggle')){if(e.target.dataset.id==='7')observe('change','shield-rule');if(e.target.dataset.id==='8')observe('change','shield-rate-policy');}});
window.addEventListener('aegis:wm-open',function(e){observe('open',e.detail&&e.detail.app||'');});
build();
document.addEventListener('aegis:authenticated',async function(){authenticated=true;syncing=true;try{var response=await fetch('/api/campaign/state',{credentials:'same-origin'});if(response.ok){var payload=await response.json();state=payload.state&&payload.state.v===1?payload.state:fresh();ensureProfileState();}else state=fresh();}catch(e){state=fresh();}ensureProfileState();syncing=false;save();panel.hidden=true;render();if(!state.started&&!state.done.includes(mission().id))start();});
window.AegisCampaign={open:function(){panel.hidden=false;render();},get state(){return JSON.parse(JSON.stringify(state));},reset:function(){if(confirm('Reset local campaign progress?')){state=fresh();save();render();start();}}};
})();
