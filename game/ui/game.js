/* Aegis: The Last Firewall — desktop-integrated game launcher and UI. */
(function () {
  "use strict";
  if (!window.AegisGameEngine || !window.AegisGameMissions) return;
  const engine=new window.AegisGameEngine();
  const root=document.createElement("section"); root.id="aegis-game-root"; root.setAttribute("aria-label","Aegis The Last Firewall game");
  document.body.appendChild(root);
  const css=document.createElement("link"); css.rel="stylesheet"; css.href="../../game/ui/game.css"; document.head.appendChild(css);
  const esc=value=>String(value??"").replace(/[&<>"']/g,c=>({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#39;"}[c]));
  let tab="campaign",activeMission=null,noticeTimer=null;
  const state=()=>engine.snapshot();
  function toast(message){let el=document.querySelector(".ag-toast");if(el)el.remove();el=document.createElement("div");el.className="ag-toast";el.textContent=message;document.body.appendChild(el);clearTimeout(noticeTimer);noticeTimer=setTimeout(()=>el.remove(),3200);}
  function open(){root.classList.add("is-open");render();}
  function close(){root.classList.remove("is-open");}
  function injectLauncher(){
    const grid=document.getElementById("desktop-grid"); if(!grid||grid.querySelector('[data-aegis-game-launcher]'))return;
    const tile=document.createElement("button");tile.type="button";tile.className="desktop-icon";tile.dataset.aegisGameLauncher="true";
    tile.style.cssText="border:0;background:transparent;color:inherit;font:inherit;cursor:pointer";
    tile.innerHTML='<div class="icon-badge" style="color:#00d9ee;border:1px solid rgba(0,217,238,.45);border-radius:12px"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7"><path d="M12 2 20 5v6c0 5-3.4 8.7-8 11-4.6-2.3-8-6-8-11V5l8-3Z"/><path d="m8.5 12 2.2 2.2 4.8-5"/></svg></div><span class="icon-label">Last Firewall</span>';
    tile.addEventListener("click",open);grid.appendChild(tile);
  }
  function render(){
    const s=state(),p=s.player;
    root.innerHTML='<div class="ag-shell"><header class="ag-top"><div class="ag-brand"><span class="ag-crest">A</span><span>AEGIS <b>///</b> THE LAST FIREWALL</span></div><div class="ag-top-actions"><span class="ag-player">'+esc(p.callsign)+' · LEVEL '+p.level+'</span><button class="ag-btn" data-action="close">RETURN TO OS ✕</button></div></header><main class="ag-layout">'+
      '<div class="ag-eyebrow">Aegis Corporation · Cyber Defence Division</div><h1 class="ag-title">The network is your responsibility.</h1><p class="ag-subtitle">Build your security capability, investigate incidents and keep critical systems operational. Every decision has a consequence.</p>'+
      '<section class="ag-stats">'+stat("EXPERIENCE",p.xp+" XP")+stat("CREDITS","¤ "+p.credits)+stat("RESEARCH",p.research+" RP")+stat("INCIDENTS RESOLVED",s.stats.incidentsResolved)+'</section>'+
      '<nav class="ag-tabs"><button class="ag-tab '+(tab==="campaign"?"active":"")+'" data-tab="campaign">CAMPAIGN</button><button class="ag-tab '+(tab==="research"?"active":"")+'" data-tab="research">RESEARCH & UPGRADES</button><button class="ag-tab '+(tab==="records"?"active":"")+'" data-tab="records">SERVICE RECORD</button></nav>'+
      '<div id="ag-content">'+(tab==="campaign"?campaignView(s):tab==="research"?researchView(s):recordsView(s))+'</div>'+
      '<footer class="ag-footer"><span>AEGIS GAME SYSTEMS · BUILD 0.1.0</span><span>FICTIONAL SIMULATION · NO HOST SYSTEM ACTIONS</span></footer></main></div>';
    root.querySelector('[data-action="close"]').addEventListener("click",close);
    root.querySelectorAll('[data-action="abandon"]').forEach(b=>b.addEventListener("click",()=>{engine.abandonMission();activeMission=null;render();}));
    root.querySelectorAll("[data-tab]").forEach(b=>b.addEventListener("click",()=>{tab=b.dataset.tab;render();}));
    root.querySelectorAll("[data-start]").forEach(b=>b.addEventListener("click",()=>start(b.dataset.start)));
    root.querySelectorAll("[data-upgrade]").forEach(b=>b.addEventListener("click",()=>{const r=engine.buyUpgrade(b.dataset.upgrade);toast(r.message);render();}));
    root.querySelectorAll("[data-step]").forEach(b=>b.addEventListener("click",()=>act(b.dataset.step)));
    root.querySelectorAll("[data-reset]").forEach(b=>b.addEventListener("click",resetConfirm));
  }
  function stat(label,value){return '<div class="ag-stat"><div class="ag-stat-label">'+label+'</div><div class="ag-stat-value">'+value+'</div></div>';}
  function campaignView(s){
    if(activeMission){const session=engine.getSession();if(session)return missionView(session);}
    const missions=engine.getMissions();
    return '<div class="ag-grid"><section class="ag-panel"><div class="ag-eyebrow">ACTIVE OPERATIONS</div><h2>Campaign missions</h2><p class="ag-muted">Complete operations to earn experience, credits and research points. New chapters unlock as your record grows.</p><div class="ag-mission-list">'+missions.map(m=>'<article class="ag-mission"><div><div class="ag-eyebrow">CHAPTER '+m.chapter+' · '+esc(m.codename)+'</div><h3>'+esc(m.title)+'</h3><p>'+esc(m.briefing.slice(0,145))+'…</p><div class="ag-mission-meta"><span class="ag-chip">'+esc(m.difficulty)+'</span><span class="ag-chip">THREAT '+esc(m.threat)+'</span><span class="ag-chip">'+m.reward.xp+' XP</span>'+(m.locked?'<span class="ag-chip locked">LOCKED</span>':"")+'</div></div><button class="ag-btn '+(!m.locked?"primary":"")+'" data-start="'+m.id+'" '+(m.locked?"disabled":"")+'>'+(s.completed.includes(m.id)?"COMPLETED":m.locked?"LOCKED":"BRIEFING →")+'</button></article>').join("")+'</div></section><aside class="ag-panel"><div class="ag-eyebrow">COMMANDER'S NOTE</div><h2>Protect the system. Preserve the evidence.</h2><p class="ag-muted">Operations use a fictional, deterministic simulation. The game never scans your real computer, changes host settings or performs real network actions.</p><div class="ag-divider"></div><h3>Campaign status</h3><div class="ag-terminal">CHAPTER UNLOCKED: '+engine.unlockedChapter()+'\nMISSIONS COMPLETE: '+s.completed.length+'/'+missions.length+'\nREPUTATION: '+s.player.reputation+'\nSYSTEMS RESTORED: '+s.stats.systemsRestored+'</div><div class="ag-divider"></div><p class="ag-muted">Tip: contain a threat before running integrity verification. Recovery is only available after the scan confirms a known-good restore point.</p></aside></div>';
  }
  function missionView(session){
    const m=session.mission,steps=m.objectives,done=session.steps.length,next=steps[done];
    const actions={inspect:"INSPECT PROCESS",isolate:"ISOLATE THREAT",scan:"RUN INTEGRITY SCAN",restore:"RESTORE SYSTEM"};
    return '<div class="ag-mission-hero"><div class="ag-eyebrow">'+esc(m.codename)+' · MISSION '+String(m.chapter).padStart(2,"0")+'</div><h2 style="font-size:30px;margin:8px 0">'+esc(m.title)+'</h2><p class="ag-muted">'+esc(m.briefing)+'</p><div class="ag-mission-meta">'+m.intel.map(x=>'<span class="ag-chip">'+esc(x)+'</span>').join("")+'</div></div><div class="ag-grid" style="margin-top:16px"><section class="ag-panel"><div class="ag-eyebrow">MISSION OBJECTIVES</div><h2>Incident response workflow</h2>'+steps.map((o,i)=>'<div class="ag-objective"><span class="ag-step '+(i<done?"done":"")+'">'+(i<done?"✓":i+1)+'</span><div><h4>'+esc(o.title)+'</h4><p>'+esc(o.detail)+'</p><div class="ag-hint">'+(i===done?"NEXT ACTION · ":"")+esc(o.hint)+'</div></div></div>').join("")+'<div style="margin-top:18px"><button class="ag-btn primary" data-step="'+(next?next.id:"")+'" '+(!next?"disabled":"")+'>'+(next?actions[next.id]:"MISSION COMPLETE")+' →</button> <button class="ag-btn" data-action="abandon">ABANDON MISSION</button></div></section><aside class="ag-panel"><div class="ag-eyebrow">LIVE SYSTEM TELEMETRY</div><h3>Simulated incident feed</h3><div class="ag-terminal">[GUARD] Anomalous process behaviour detected\n[TRACE] Process: sync_update32\n[TRACE] PID: 2841 · PRIVILEGE: ELEVATED\n[NETWORK] Unusual outbound session\n[FILESYSTEM] Baseline comparison pending\n[RESPONSE] '+(session.contained?"THREAT CONTAINED":"AWAITING OPERATOR")+'\n[INTEGRITY] '+(session.integrityVerified?"VERIFIED":"NOT VERIFIED")+'\n[RECOVERY] '+(session.restored?"RESTORED":"NOT STARTED")+'</div><div class="ag-divider"></div><h3>Evidence collected</h3><p class="ag-muted">'+(session.evidence.length?session.evidence.map(esc).join("<br>"):"No evidence collected yet.")+'</p><div class="ag-divider"></div><p class="ag-muted">Operator guidance: '+(next?esc(next.hint):"All objectives completed.")+'</p></aside></div>';
  }
  function researchView(s){
    return '<div class="ag-panel"><div class="ag-eyebrow">TECHNOLOGY DEVELOPMENT</div><h2>Expand Aegis capabilities</h2><p class="ag-muted">Spend research points earned from completed operations. Installed upgrades persist with your campaign save.</p><div class="ag-upgrades" style="margin-top:18px">'+window.AegisGameUpgrades.map(u=>'<article class="ag-upgrade"><div class="ag-eyebrow">'+esc(u.branch)+'</div><h3>'+esc(u.name)+'</h3><p>'+esc(u.description)+'</p><div style="display:flex;justify-content:space-between;align-items:center;gap:8px"><span class="ag-chip">'+u.cost+' RESEARCH</span><button class="ag-btn" data-upgrade="'+u.id+'" '+(s.upgrades.includes(u.id)||s.player.research<u.cost?"disabled":"")+'>'+(s.upgrades.includes(u.id)?"INSTALLED":"INSTALL")+'</button></div></article>').join("")+'</div></div>';
  }
  function recordsView(s){
    return '<div class="ag-grid"><section class="ag-panel"><div class="ag-eyebrow">OPERATOR DOSSIER</div><h2>'+esc(s.player.callsign)+'</h2><p class="ag-muted">Aegis Cyber Defence Division · Level '+s.player.level+'</p><div class="ag-divider"></div>'+stat("MISSIONS COMPLETED",s.completed.length)+stat("INCIDENTS RESOLVED",s.stats.incidentsResolved)+stat("SYSTEMS RESTORED",s.stats.systemsRestored)+stat("REPUTATION",s.player.reputation)+'</section><aside class="ag-panel"><div class="ag-eyebrow">SAVE DATA</div><h3>Local campaign profile</h3><p class="ag-muted">Progress is stored in this browser using local storage. It does not sync to a server.</p><p class="ag-muted">Last saved: '+new Date(s.updatedAt).toLocaleString()+'</p><button class="ag-btn" data-reset="true">RESET CAMPAIGN</button></aside></div>';
  }
  function start(id){const result=engine.startMission(id);if(!result.ok){toast(result.message);return;}activeMission=id;toast("Mission briefing accepted.");render();}
  function act(id){if(!id)return;const result=engine.perform(id);if(!result.ok){toast(result.message);return;}if(result.complete){activeMission=null;tab="campaign";render();showDebrief(result);}else{render();toast(result.message);}}
  function showDebrief(result){
    const backdrop=document.createElement("div");backdrop.className="ag-modal-backdrop";backdrop.innerHTML='<div class="ag-modal"><div class="ag-eyebrow">OPERATION COMPLETE</div><h2>'+esc(result.mission.title)+' — SUCCESS</h2><p>'+esc(result.mission.debrief)+'</p><div class="ag-stats">'+stat("EXPERIENCE","+"+result.reward.xp+" XP")+stat("CREDITS","+"+result.reward.credits)+stat("RESEARCH","+"+result.reward.research+" RP")+stat("REPUTATION","+10")+'</div>'+(result.levelUp?'<p style="color:#78f0c2">LEVEL UP — NEW OPERATIONAL CAPABILITIES AVAILABLE.</p>':"")+'<button class="ag-btn primary" data-dismiss>RETURN TO CAMPAIGN →</button></div>';
    root.appendChild(backdrop);backdrop.querySelector("[data-dismiss]").addEventListener("click",()=>{backdrop.remove();render();});
  }
  function resetConfirm(){if(!window.confirm("Reset the Aegis campaign and erase this browser's local game save?"))return;engine.reset();activeMission=null;tab="campaign";render();toast("Campaign profile reset.");}
  engine.subscribe(()=>{});
  injectLauncher();
  if(document.readyState==="loading")document.addEventListener("DOMContentLoaded",injectLauncher);
  window.addEventListener("keydown",e=>{if(e.key==="Escape"&&root.classList.contains("is-open"))close();});
})();