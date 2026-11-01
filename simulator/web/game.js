'use strict';
(() => {
  const $ = (selector) => document.querySelector(selector);
  let selectedMissionId = 'mission-01';
  let latestState = null;

  const api = async (endpoint, body) => {
    const response = await fetch(endpoint, {
      method: body ? 'POST' : 'GET',
      credentials: 'same-origin',
      headers: body ? { 'Content-Type': 'application/json' } : {},
      body: body ? JSON.stringify(body) : undefined
    });
    const data = await response.json();
    if (!response.ok) throw new Error(data.error || data.reason || 'Request failed');
    return data;
  };
  const addText = (tag, className, value) => {
    const element = document.createElement(tag);
    if (className) element.className = className;
    element.textContent = String(value ?? '');
    return element;
  };
  const log = (message, kind = '') => {
    const row = document.createElement('p');
    if (kind) row.className = kind;
    const time = addText('time', '', new Date().toLocaleTimeString());
    row.append(time, document.createTextNode(' ' + String(message ?? '')));
    $('#event-log').prepend(row);
  };
  const metric = (label, value, suffix = '') => {
    const item = document.createElement('div'); item.className = 'resource';
    item.append(addText('span', '', label), addText('strong', '', Math.round(value * 10) / 10 + suffix));
    return item;
  };
  const renderCampaign = (missions) => {
    const grid = $('#campaign-missions');
    grid.replaceChildren();
    const completed = missions.filter(item => item.completed).length;
    const percent = Math.round(completed / Math.max(1, missions.length) * 100);
    $('#campaign-status').textContent = completed + ' / ' + missions.length + ' OPERATIONS SECURED';
    $('#campaign-progress-label').textContent = percent + '%';
    $('#campaign-progress-fill').style.width = percent + '%';
    missions.forEach((mission, index) => {
      const card = document.createElement('button');
      card.type = 'button';
      card.className = 'campaign-card' + (mission.completed ? ' completed' : '') + (!mission.unlocked ? ' locked' : '') + (mission.id === selectedMissionId ? ' selected' : '');
      card.disabled = !mission.unlocked;
      const number = addText('span', 'campaign-number', 'OPERATION 0' + (index + 1));
      const status = addText('span', 'campaign-card-status', mission.completed ? 'SECURED' : mission.unlocked ? 'AVAILABLE' : 'LOCKED');
      const title = addText('strong', 'campaign-card-title', mission.title);
      const detail = addText('small', 'campaign-card-detail', mission.difficulty.toUpperCase() + ' / ' + mission.objectiveCount + ' OBJECTIVES');
      card.append(number, status, title, detail);
      card.addEventListener('click', () => {
        if (!mission.unlocked) return;
        selectedMissionId = mission.id;
        render(latestState);
      });
      grid.append(card);
    });
  };

  const render = (state) => {
    if (!state) return;
    latestState = state;
    const r = state.resources;
    $('#resources').replaceChildren(
      metric('Credits', r.credits), metric('Research', r.research), metric('CPU capacity', r.cpu, '%'),
      metric('Memory', r.memory, '%'), metric('Bandwidth', r.bandwidth, '%'), metric('Stability', r.stability, '%'), metric('Reputation', r.reputation)
    );
    $('#posture').textContent = r.stability >= 70 ? 'OPERATIONAL' : r.stability >= 35 ? 'DEGRADED' : 'CRITICAL';
    $('#posture').style.color = r.stability >= 70 ? 'var(--green)' : r.stability >= 35 ? 'var(--amber)' : 'var(--red)';

    if (!state.missions.some(item => item.id === selectedMissionId && item.unlocked)) {
      selectedMissionId = state.currentMission ? state.currentMission.id : (state.missions.find(item => item.unlocked && !item.completed) || state.missions[0]).id;
    }
    if (state.currentMission && state.currentMission.status === 'active') selectedMissionId = state.currentMission.id;
    renderCampaign(state.missions);

    const select = $('#mission-select');
    select.replaceChildren();
    state.missions.forEach(item => {
      const option = document.createElement('option');
      option.value = item.id;
      option.textContent = 'Mission ' + String(item.chapter).padStart(2, '0') + ' — ' + item.title + (item.completed ? ' (SECURED)' : !item.unlocked ? ' (LOCKED)' : '');
      option.disabled = !item.unlocked;
      select.append(option);
    });
    select.value = selectedMissionId;

    const selected = state.missions.find(item => item.id === selectedMissionId);
    const activeMission = state.currentMission && state.currentMission.id === selectedMissionId ? state.currentMission : null;
    const missionInProgress = !!activeMission && activeMission.status === 'active';
    const displayed = activeMission || selected;
    $('#mission-title').textContent = displayed ? displayed.title : 'Awaiting deployment';
    $('#mission-status').textContent = missionInProgress ? 'OPERATION IN PROGRESS' : activeMission && activeMission.status === 'completed' ? 'OPERATION SECURED' : selected && selected.completed ? 'OPERATION SECURED' : selected && selected.unlocked ? 'READY FOR DEPLOYMENT' : 'ACCESS LOCKED';
    $('#mission-copy').textContent = displayed ? displayed.description : 'Select an available operation.';
    $('#briefing').textContent = displayed ? (displayed.briefing || 'Review the objectives and deploy when ready.') : '';
    $('#briefing').classList.toggle('hidden', !displayed);
    const objectives = $('#objectives');
    objectives.replaceChildren();
    const objectiveItems = activeMission ? activeMission.objectives : (selected ? selected.objectives : []);
    const doneCount = objectiveItems.filter(item => item.completed).length;
    $('#mission-progress-label').textContent = doneCount + ' / ' + objectiveItems.length;
    $('#mission-progress-fill').style.width = (objectiveItems.length ? doneCount / objectiveItems.length * 100 : 0) + '%';
    if (!objectiveItems.length) objectives.append(addText('div', 'empty-state', 'No mission objectives are available.'));
    objectiveItems.forEach((objective, index) => {
      const completed = !!objective.completed;
      const isNext = missionInProgress && !completed && !activeMission.objectives.slice(0, index).some(item => !item.completed);
      const row = document.createElement('div');
      row.className = 'objective' + (completed ? ' done' : isNext ? ' active' : '');
      const number = addText('span', 'objective-index', String(index + 1).padStart(2, '0'));
      const copy = document.createElement('div'); copy.className = 'objective-copy';
      copy.append(addText('strong', '', objective.title), addText('small', '', objective.description));
      const status = addText('span', 'objective-check', completed ? 'SECURED' : isNext ? 'NEXT' : 'PENDING');
      row.append(number, copy, status);
      if (isNext) {
        const button = addText('button', 'button button-primary', 'Execute');
        button.type = 'button';
        button.addEventListener('click', () => execute(activeMission.objectives[index].action));
        row.append(button);
      }
      objectives.append(row);
    });
    const selectedMeta = state.missions.find(item => item.id === selectedMissionId);
    $('#start-mission').disabled = missionInProgress || !selectedMeta || selectedMeta.completed || !selectedMeta.unlocked;
    $('#start-mission').textContent = missionInProgress ? 'Operation in progress' : selectedMeta && selectedMeta.completed ? 'Operation secured' : 'Deploy operation →';

    const threats = $('#threats'); threats.replaceChildren();
    const activeThreats = state.threats.filter(item => item.status === 'active');
    $('#threat-count').textContent = String(activeThreats.length).padStart(2, '0');
    if (!state.threats.length) threats.append(addText('div', 'empty-state', 'No threat telemetry.'));
    state.threats.forEach(threat => {
      const row = document.createElement('div'); row.className = 'threat-item';
      const top = document.createElement('div'); top.className = 'threat-row';
      top.append(addText('strong', '', threat.name), addText('span', 'severity ' + threat.severity, threat.severity.toUpperCase()));
      row.append(top, addText('small', '', threat.instanceId + ' / ' + (threat.detected ? 'DETECTED' : 'UNANALYSED') + ' / ' + threat.status.toUpperCase()));
      threats.append(row);
    });

    const research = $('#research'); research.replaceChildren();
    state.research.forEach(node => {
      const row = document.createElement('div'); row.className = 'research-item';
      const top = document.createElement('div'); top.className = 'research-row';
      top.append(addText('strong', '', node.name), addText('span', 'severity', node.cost + ' RP'));
      row.append(top, addText('small', '', node.description));
      const button = addText('button', 'button', node.unlocked ? 'UNLOCKED' : node.available ? 'Research upgrade' : 'Prerequisites locked');
      button.type = 'button'; button.disabled = !node.available;
      button.addEventListener('click', () => unlock(node.id));
      row.append(button); research.append(row);
    });
    $('#connection').textContent = 'CONNECTED';
  };

  const refresh = async () => {
    const state = await api('/api/game/state');
    render(state);
  };
  const execute = async (action) => {
    try {
      const result = await api('/api/game/action', { action });
      log(result.detail || action, 'success');
      if (result.progress && result.progress.completed) log('OPERATION SECURED — campaign rewards transferred to your profile.', 'success');
      render(result.state);
    } catch (error) { log(error.message, 'warning'); $('#mission-feedback').textContent = error.message; }
  };
  const unlock = async (nodeId) => {
    try {
      const result = await api('/api/game/research', { nodeId });
      log('Research unlocked: ' + result.node.name, 'success');
      render(result.state);
    } catch (error) { log(error.message, 'warning'); }
  };

  $('#mission-select').addEventListener('change', (event) => {
    selectedMissionId = event.target.value;
    if (latestState) render(latestState);
  });
  $('#start-mission').addEventListener('click', async () => {
    try {
      const result = await api('/api/game/start', { missionId: selectedMissionId });
      log('Operation deployed: ' + result.mission.title, 'success');
      render(result.state);
    } catch (error) { $('#mission-feedback').textContent = error.message; log(error.message, 'warning'); }
  });
  $('#save').addEventListener('click', async () => {
    try { const result = await api('/api/game/save', { slot: 'profile' }); log('Player campaign saved at ' + result.savedAt, 'success'); }
    catch (error) { log(error.message, 'warning'); }
  });
  $('#load').addEventListener('click', async () => {
    try { const result = await api('/api/game/load', { slot: 'profile' }); render(result.state); log('Your player campaign was restored.', 'success'); }
    catch (error) { log(error.message, 'warning'); }
  });

  window.addEventListener('aegis:authenticated', () => {
    $('#mission-feedback').textContent = '';
    refresh().then(() => log('Authenticated player profile loaded. Welcome to Aegis Command.', 'success')).catch(error => {
      $('#connection').textContent = 'OFFLINE';
      log(error.message, 'warning');
    });
  });
})();
