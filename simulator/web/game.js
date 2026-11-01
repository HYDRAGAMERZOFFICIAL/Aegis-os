'use strict';
(() => {
  const $ = (selector) => document.querySelector(selector);
  const api = async (endpoint, body) => {
    const response = await fetch(endpoint, { method: body ? 'POST' : 'GET', headers: body ? { 'Content-Type': 'application/json' } : {}, body: body ? JSON.stringify(body) : undefined });
    const data = await response.json();
    if (!response.ok) throw new Error(data.error || data.reason || 'Request failed');
    return data;
  };
  const escapeText = (value) => String(value ?? '');
  const log = (message, kind = '') => {
    const row = document.createElement('p'); if (kind) row.className = kind;
    const time = document.createElement('time'); time.textContent = new Date().toLocaleTimeString();
    row.append(time, document.createTextNode(escapeText(message))); $('#event-log').prepend(row);
  };
  const metric = (label, value, suffix = '') => {
    const item = document.createElement('div'); item.className = 'resource';
    const name = document.createElement('span'); name.textContent = label;
    const amount = document.createElement('strong'); amount.textContent = Math.round(value * 10) / 10 + suffix;
    item.append(name, amount); return item;
  };
  const render = (state) => {
    const r = state.resources;
    const resources = $('#resources'); resources.replaceChildren(
      metric('Credits', r.credits), metric('Research', r.research), metric('CPU capacity', r.cpu, '%'),
      metric('Memory', r.memory, '%'), metric('Bandwidth', r.bandwidth, '%'), metric('Stability', r.stability, '%'), metric('Reputation', r.reputation)
    );
    $('#posture').textContent = r.stability >= 70 ? 'OPERATIONAL' : r.stability >= 35 ? 'DEGRADED' : 'CRITICAL';
    $('#posture').style.color = r.stability >= 70 ? 'var(--green)' : r.stability >= 35 ? 'var(--amber)' : 'var(--red)';
    const mission = state.currentMission;
    $('#mission-title').textContent = mission ? 'The Silent Intrusion' : 'Awaiting deployment';
    $('#mission-status').textContent = mission ? mission.status.toUpperCase() : 'NO ACTIVE MISSION';
    $('#mission-copy').textContent = mission ? 'Work through the incident response sequence. Complete each objective to secure the operation.' : 'Aegis Command is standing by. Begin the first operation to receive your briefing and objectives.';
    $('#start-mission').disabled = !!mission && mission.status === 'active';
    $('#start-mission').textContent = mission ? (mission.status === 'completed' ? 'Mission completed' : 'Mission deployed') : 'Deploy Mission 01 →';
    const objectives = $('#objectives'); objectives.replaceChildren();
    if (!mission) { const empty = document.createElement('div'); empty.className = 'empty-state'; empty.textContent = 'No active objectives. Deploy Mission 01 to begin.'; objectives.append(empty); }
    else mission.objectives.forEach((objective, index) => {
      const row = document.createElement('div'); row.className = 'objective' + (objective.completed ? ' done' : (!mission.objectives.slice(0,index).some(item => !item.completed) && mission.status === 'active' ? ' active' : ''));
      const number = document.createElement('span'); number.className = 'objective-index'; number.textContent = String(index + 1).padStart(2,'0');
      const copy = document.createElement('div'); copy.className = 'objective-copy';
      const title = document.createElement('strong'); title.textContent = objective.title;
      const desc = document.createElement('small'); desc.textContent = objective.description; copy.append(title,desc);
      const status = document.createElement('span'); status.className = 'objective-check'; status.textContent = objective.completed ? 'COMPLETE' : 'PENDING';
      row.append(number,copy,status);
      if (!objective.completed && mission.status === 'active' && !mission.objectives.slice(0,index).some(item => !item.completed)) {
        const button = document.createElement('button'); button.className = 'button button-primary'; button.textContent = 'Execute';
        button.addEventListener('click', () => execute(objective.action)); row.append(button);
      }
      objectives.append(row);
    });
    const threats = $('#threats'); threats.replaceChildren();
    const active = state.threats.filter(item => item.status === 'active'); $('#threat-count').textContent = String(active.length).padStart(2,'0');
    if (!state.threats.length) { const empty = document.createElement('div'); empty.className = 'empty-state'; empty.textContent = 'No threat telemetry.'; threats.append(empty); }
    state.threats.forEach(threat => {
      const row = document.createElement('div'); row.className = 'threat-item';
      const top = document.createElement('div'); top.className = 'threat-row';
      const name = document.createElement('strong'); name.textContent = threat.name;
      const severity = document.createElement('span'); severity.className = 'severity ' + threat.severity; severity.textContent = threat.severity.toUpperCase();
      top.append(name,severity);
      const detail = document.createElement('small'); detail.textContent = threat.instanceId + ' / ' + (threat.detected ? 'DETECTED' : 'UNANALYSED') + ' / ' + threat.status.toUpperCase();
      row.append(top,detail); threats.append(row);
    });
    const research = $('#research'); research.replaceChildren();
    state.research.forEach(node => {
      const row = document.createElement('div'); row.className = 'research-item';
      const top = document.createElement('div'); top.className = 'research-row';
      const name = document.createElement('strong'); name.textContent = node.name;
      const cost = document.createElement('span'); cost.className = 'severity'; cost.textContent = node.cost + ' RP'; top.append(name,cost);
      const desc = document.createElement('small'); desc.textContent = node.description;
      row.append(top,desc);
      const button = document.createElement('button'); button.className = 'button'; button.textContent = node.unlocked ? 'UNLOCKED' : node.available ? 'Research upgrade' : 'Prerequisites locked';
      button.disabled = !node.available;
      button.addEventListener('click', () => unlock(node.id)); row.append(button); research.append(row);
    });
    $('#connection').textContent = 'CONNECTED';
  };
  const refresh = async () => { const state = await api('/api/game/state'); render(state); };
  const execute = async (action) => {
    try { const result = await api('/api/game/action',{action}); log(result.detail || action, 'success'); if (result.progress && result.progress.completed) log('MISSION COMPLETE — rewards transferred to command reserves.', 'success'); render(result.state); }
    catch (error) { log(error.message,'warning'); $('#mission-feedback').textContent = error.message; }
  };
  const unlock = async (nodeId) => {
    try { const result = await api('/api/game/research',{nodeId}); log('Research unlocked: ' + result.node.name,'success'); render(result.state); }
    catch (error) { log(error.message,'warning'); }
  };
  $('#start-mission').addEventListener('click', async () => {
    try { const result = await api('/api/game/start',{missionId:'mission-01'}); log('Mission deployed: The Silent Intrusion','success'); render(result.state); }
    catch (error) { $('#mission-feedback').textContent = error.message; log(error.message,'warning'); }
  });
  $('#save').addEventListener('click', async () => { try { const result = await api('/api/game/save',{slot:'profile'}); log('Local campaign saved at ' + result.savedAt,'success'); } catch(error) { log(error.message,'warning'); } });
  $('#load').addEventListener('click', async () => { try { const result = await api('/api/game/load',{slot:'profile'}); render(result.state); log('Campaign restored from local save.','success'); } catch(error) { log(error.message,'warning'); } });
  refresh().catch(error => { $('#connection').textContent = 'OFFLINE'; log(error.message,'warning'); });
})();
