'use strict';
(() => {
  const $ = (selector) => document.querySelector(selector);
  let selectedMissionId = 'mission-01';
  let latestState = null;
  let activeChallenge = null;
  const challenges = {
    'mission-01': {
      trace_origin: { title: 'Trace the intrusion source', prompt: 'Packet correlation found three candidate addresses. Which source matches the hostile relay signature?', options: [
        { label: '10.0.0.8', detail: 'Internal Aegis resolver', answer: '10.0.0.8' },
        { label: '185.220.101.5', detail: 'Untrusted external relay / anomalous beacon', answer: '185.220.101.5' },
        { label: '172.16.0.3', detail: 'Backup telemetry node', answer: '172.16.0.3' }
      ] },
      configure_firewall: { title: 'Configure Aegis Shield', prompt: 'Choose the defensive rule that blocks the intrusion without disabling trusted internal services.', options: [
        { label: 'Allow all inbound traffic', detail: 'Leaves the attack surface exposed', answer: 'allow-all' },
        { label: 'Deny untrusted inbound traffic', detail: 'Default-deny policy for suspicious ingress', answer: 'deny-untrusted' },
        { label: 'Disable the firewall', detail: 'Removes the defensive boundary', answer: 'disable-firewall' }
      ] },
      restore_files: { title: 'Select a recovery checkpoint', prompt: 'Choose the verified clean restore point created before the intrusion.', options: [
        { label: 'chkpt-0001', detail: 'Clean boot baseline', answer: 'chkpt-0001' },
        { label: 'chkpt-0002', detail: 'Snapshot captured during pre-update maintenance', answer: 'chkpt-0002' },
        { label: 'chkpt-0003', detail: 'Verified clean alpha restore point', answer: 'chkpt-0003' }
      ] },
      submit_report: { title: 'Classify the incident', prompt: 'The intrusion was contained and files were restored. Submit the matching incident classification.', options: [
        { label: 'Low / no impact', detail: 'Does not reflect the detected ransomware activity', answer: 'low' },
        { label: 'Critical / contained', detail: 'Critical incident, containment and recovery verified', answer: 'critical-contained' },
        { label: 'Uncontained compromise', detail: 'Contradicts the completed recovery actions', answer: 'uncontained' }
      ] }
    },
    'mission-02': {
      revoke_token: { title: 'Identify the compromised capability', prompt: 'Which token belongs to the process attempting unauthorised kernel access?', options: [
        { label: 'TOK-UI-FRAMEBUFFER', detail: 'Expected display capability', answer: 'TOK-UI-FRAMEBUFFER' },
        { label: 'TOK-KERNEL-ESCALATED', detail: 'Unexpected elevated kernel capability', answer: 'TOK-KERNEL-ESCALATED' },
        { label: 'TOK-TELEMETRY-READ', detail: 'Read-only monitoring capability', answer: 'TOK-TELEMETRY-READ' }
      ] },
      apply_patch: { title: 'Verify the kernel patch', prompt: 'Select the signed training patch approved for this Aegis kernel build.', options: [
        { label: 'AEGIS-KERNEL-2.0.4', detail: 'Verified security patch / signature valid', answer: 'AEGIS-KERNEL-2.0.4' },
        { label: 'AEGIS-KERNEL-0.9.1', detail: 'Legacy build / unsupported', answer: 'AEGIS-KERNEL-0.9.1' },
        { label: 'UNKNOWN-BINARY', detail: 'Unverified package / signature missing', answer: 'UNKNOWN-BINARY' }
      ] },
      audit_kernel: { title: 'Validate the trust root', prompt: 'Choose the integrity result that matches the trusted Aegis root record.', options: [
        { label: 'AEGIS-ROOT-VERIFIED', detail: 'Trusted root hash and kernel state match', answer: 'AEGIS-ROOT-VERIFIED' },
        { label: 'ROOT-MISMATCH', detail: 'Integrity divergence detected', answer: 'ROOT-MISMATCH' }
      ] },
      submit_report: { title: 'Submit the kernel incident report', prompt: 'Choose the final status after revoking the token, patching and auditing the kernel.', options: [
        { label: 'Privilege escalation contained', detail: 'All defensive actions verified', answer: 'privilege-contained' },
        { label: 'Kernel trust still compromised', detail: 'Does not match the completed audit', answer: 'trust-compromised' }
      ] }
    },
    'mission-03': {
      rate_limit: { title: 'Tune ingress protection', prompt: 'Choose the adaptive rate profile that protects priority services while containing the flood.', options: [
        { label: 'Unlimited ingress', detail: 'No traffic control', answer: 'unlimited' },
        { label: 'Adaptive 750 requests', detail: 'Adaptive ingress threshold with priority queues', answer: 'adaptive-750' },
        { label: 'Block all traffic', detail: 'Would also deny trusted service traffic', answer: 'block-all' }
      ] },
      reroute_traffic: { title: 'Select the resilient route', prompt: 'Which route remains healthy and has sufficient reserved capacity?', options: [
        { label: 'Edge route 01', detail: 'High packet loss / degraded', answer: 'edge-route-01' },
        { label: 'Edge route 02', detail: 'Healthy / reserved capacity available', answer: 'edge-route-02' },
        { label: 'Direct origin route', detail: 'Currently saturated', answer: 'direct-origin' }
      ] },
      restore_service: { title: 'Restore the affected service', prompt: 'Select the service endpoint whose health check failed during the flood.', options: [
        { label: 'Aegis API gateway', detail: 'Primary affected service', answer: 'api-gateway' },
        { label: 'Telemetry archive', detail: 'Healthy / unaffected', answer: 'telemetry-archive' },
        { label: 'Identity vault', detail: 'Healthy / unaffected', answer: 'identity-vault' }
      ] },
      submit_report: { title: 'Close the blackout incident', prompt: 'Choose the verified outcome after mitigation and service restoration.', options: [
        { label: 'Availability restored', detail: 'Service health checks pass', answer: 'availability-restored' },
        { label: 'Outage ongoing', detail: 'Does not match the completed recovery', answer: 'outage-ongoing' }
      ] }
    }
  };

  const api = async (endpoint, body) => {
    const response = await fetch(endpoint, {
      method: body ? 'POST' : 'GET',
      credentials: 'same-origin',
      headers: body ? { 'Content-Type': 'application/json' } : {},
      body: body ? JSON.stringify(body) : undefined
    });
    const data = await response.json().catch(() => ({}));
    if (!response.ok) throw new Error(data.error || data.reason || 'Request failed');
    return data;
  };
  const isCloudProfile = () => window.AegisAuth && window.AegisAuth.storageMode === 'cloud';
  const savedLabel = () => isCloudProfile() ? 'CLOUD SAVE SYNCED' : 'LOCAL SAVE WRITTEN';
  const setCloudStatus = (message, healthy = true) => {
    const status = $('#cloud-sync');
    if (!status) return;
    status.textContent = message;
    status.classList.toggle('offline', !healthy);
  };
  const configureStorageMode = () => {
    const cloud = isCloudProfile();
    setCloudStatus(cloud ? 'CLOUD SAVE READY' : 'LOCAL SAVE MODE');
    $('#save').textContent = cloud ? 'Save to cloud' : 'Save locally';
    $('#profile-storage-label').textContent = cloud ? 'AEGIS CORE / CLOUD PROFILE' : 'AEGIS CORE / LOCAL PROFILE';
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
    if (state.saveUpdatedAt) setCloudStatus('CLOUD SAVE SYNCED');
    else if (!isCloudProfile()) setCloudStatus('LOCAL SAVE MODE');
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
        const action = activeMission.objectives[index].action;
        const challenge = challenges[activeMission.id] && challenges[activeMission.id][action];
        const button = addText('button', 'button button-primary', challenge ? 'Open console' : 'Execute');
        button.type = 'button';
        button.addEventListener('click', () => {
          if (challenge) { activeChallenge = { ...challenge, action, missionId: activeMission.id }; render(latestState); }
          else execute(action);
        });
        row.append(button);
      }
      objectives.append(row);
    });
    const consolePanel = $('#mission-console');
    if (activeChallenge && (!missionInProgress || activeChallenge.missionId !== activeMission.id)) activeChallenge = null;
    consolePanel.replaceChildren();
    if (activeChallenge && missionInProgress) {
      consolePanel.classList.remove('hidden');
      consolePanel.append(addText('p', 'eyebrow', 'TACTICAL DECISION / ' + activeChallenge.title.toUpperCase()));
      consolePanel.append(addText('p', 'challenge-prompt', activeChallenge.prompt));
      const choices = document.createElement('div'); choices.className = 'challenge-choices';
      activeChallenge.options.forEach(option => {
        const choice = document.createElement('button'); choice.type = 'button'; choice.className = 'challenge-choice';
        choice.append(addText('strong', '', option.label), addText('small', '', option.detail));
        choice.addEventListener('click', () => execute(activeChallenge.action, { answer: option.answer }));
        choices.append(choice);
      });
      consolePanel.append(choices);
      const cancel = addText('button', 'text-button challenge-cancel', 'Close tactical console'); cancel.type = 'button';
      cancel.addEventListener('click', () => { activeChallenge = null; render(latestState); });
      consolePanel.append(cancel);
    } else consolePanel.classList.add('hidden');

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
  const execute = async (action, payload = {}) => {
    try {
      const result = await api('/api/game/action', { action, payload });
      activeChallenge = null;
      setCloudStatus(savedLabel());
      log((result.detail || action) + (isCloudProfile() ? ' · cloud save confirmed' : ' · local save written'), 'success');
      if (result.progress && result.progress.completed) log('OPERATION SECURED — campaign rewards transferred to your profile.', 'success');
      render(result.state);
    } catch (error) { log(error.message, 'warning'); $('#mission-feedback').textContent = error.message; }
  };
  const unlock = async (nodeId) => {
    try {
      const result = await api('/api/game/research', { nodeId });
      setCloudStatus(savedLabel());
      log('Research unlocked: ' + result.node.name + (isCloudProfile() ? ' · cloud save confirmed' : ' · local save written'), 'success');
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
      setCloudStatus(savedLabel());
      log('Operation deployed: ' + result.mission.title + (isCloudProfile() ? ' · cloud save confirmed' : ' · local save written'), 'success');
      render(result.state);
    } catch (error) { $('#mission-feedback').textContent = error.message; log(error.message, 'warning'); }
  });
  $('#save').addEventListener('click', async () => {
    try { const result = await api('/api/game/save', { slot: 'cloud-profile' }); setCloudStatus(savedLabel()); log((isCloudProfile() ? 'Cloud' : 'Local') + ' campaign saved at ' + result.savedAt, 'success'); }
    catch (error) { log(error.message, 'warning'); }
  });
  $('#load').addEventListener('click', async () => {
    try { const result = await api('/api/game/load', { slot: 'cloud-profile' }); setCloudStatus(isCloudProfile() ? 'CLOUD SAVE RESTORED' : 'LOCAL SAVE RESTORED'); render(result.state); log('Your ' + (isCloudProfile() ? 'cloud' : 'local') + ' campaign was restored.', 'success'); }
    catch (error) { log(error.message, 'warning'); }
  });

  window.addEventListener('aegis:authenticated', () => {
    configureStorageMode();
    $('#mission-feedback').textContent = '';
    refresh().then(() => log('Authenticated player profile loaded. Welcome to Aegis Command.', 'success')).catch(error => {
      $('#connection').textContent = 'OFFLINE';
      setCloudStatus(isCloudProfile() ? 'CLOUD OFFLINE' : 'LOCAL SERVICE OFFLINE', false);
      log(error.message, 'warning');
    });
  });
})();
