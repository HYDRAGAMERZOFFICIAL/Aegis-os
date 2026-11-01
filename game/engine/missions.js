/* Aegis: The Last Firewall — fictional campaign content. */
(function (root) {
  "use strict";
  root.AegisGameMissions = [
    {
      id:"silent-intrusion", chapter:1, title:"The Silent Intrusion", codename:"OPERATION QUIET ENTRY",
      difficulty:"INTRODUCTORY", threat:"CRITICAL", duration:"8–12 MIN",
      briefing:"A routine integrity check has surfaced an unknown process consuming abnormal memory and opening an unexpected outbound connection. The system is still stable. Find the source, contain it, and verify the installation before the intrusion spreads.",
      intel:["Unknown process: sync_update32","Unexpected outbound traffic detected","One system library may have been modified"],
      objectives:[
        {id:"inspect",title:"Inspect the suspicious process",detail:"Open the process telemetry and review its behaviour.",hint:"Compare its memory and network activity with normal system services."},
        {id:"isolate",title:"Contain the process",detail:"Revoke its simulated network capability and isolate it.",hint:"Containment preserves evidence while preventing further activity."},
        {id:"scan",title:"Run an integrity scan",detail:"Verify critical system files and identify altered components.",hint:"A process can be contained while persistence remains on disk."},
        {id:"restore",title:"Restore the affected component",detail:"Use the verified recovery point to restore the modified library.",hint:"Only restore after the integrity scan has identified the affected component."}
      ],
      decision:{"prompt":"The process is anomalous, but the evidence is incomplete. What is your first response?","bestChoice":"preserve","options":[{"id":"preserve","label":"Preserve telemetry, then isolate the process","reason":"Correct. Capturing volatile evidence first protects the investigation while containment limits further activity."},{"id":"terminate","label":"Immediately terminate the process","reason":"This may stop activity, but it destroys volatile evidence and can hide the intrusion path."},{"id":"block-all","label":"Disable all outbound connectivity","reason":"This is disproportionate and may disrupt unrelated services before the source is understood."}]},
      reward:{xp:250,credits:180,research:2},
      debrief:"The suspicious process was contained and the affected library restored from a verified checkpoint. The incident is closed, but its origin remains unknown."
    },
    {
      id:"credential-shadow",chapter:1,title:"Credential Shadow",codename:"OPERATION PAPER KEY",
      difficulty:"STANDARD",threat:"HIGH",duration:"10–15 MIN",
      briefing:"Several failed authentication attempts are followed by a successful session from an unfamiliar workstation. Audit the session, revoke the exposed access token, and rotate the affected credentials.",
      intel:["Repeated authentication failures","New session from an untrusted endpoint","One temporary access token is still active"],
      objectives:[
        {id:"inspect",title:"Review authentication logs",detail:"Identify the unusual session and its access scope.",hint:"Look for a change in source and privilege level."},
        {id:"isolate",title:"Revoke the suspicious session",detail:"Invalidate the temporary token and quarantine the session.",hint:"Revoke the specific session rather than disabling all users."},
        {id:"scan",title:"Audit account permissions",detail:"Check for unexpected privilege changes.",hint:"Compare current permissions with the baseline."},
        {id:"restore",title:"Apply a secure access baseline",detail:"Rotate credentials and restore least-privilege access.",hint:"Finish by returning the account to a known-good state."}
      ],
      decision:{"prompt":"A valid account has an unfamiliar active session. Which action best limits impact?","bestChoice":"revoke","options":[{"id":"revoke","label":"Revoke the suspicious session and preserve the audit trail","reason":"Correct. Targeted revocation limits exposure while retaining evidence for attribution."},{"id":"disable-all","label":"Disable every user account","reason":"This creates avoidable service disruption and does not distinguish the compromised session."},{"id":"ignore","label":"Wait for another authentication alert","reason":"The active session remains a live risk while the response is delayed."}]},
      reward:{xp:350,credits:240,research:3},
      debrief:"The unauthorised session was revoked and the account returned to least-privilege access."
    },
    {
      id:"broken-trust",chapter:2,title:"Broken Trust",codename:"OPERATION GLASS GATE",
      difficulty:"ADVANCED",threat:"CRITICAL",duration:"15–20 MIN",
      briefing:"A trusted service is requesting capabilities outside its normal operating profile. The service has not yet caused visible damage. Determine whether the trust relationship is still valid and constrain its access.",
      intel:["Service capability drift detected","Unexpected storage access request","Integrity baseline remains partially healthy"],
      objectives:[
        {id:"inspect",title:"Inspect service behaviour",detail:"Compare requested capabilities with the service baseline.",hint:"An unusual request is a signal, not proof of compromise."},
        {id:"isolate",title:"Restrict excess capabilities",detail:"Apply a temporary least-privilege policy.",hint:"Preserve essential service access while denying the anomalous request."},
        {id:"scan",title:"Validate service integrity",detail:"Check service files and signed configuration.",hint:"Confirm that the service has not been modified."},
        {id:"restore",title:"Re-establish trust",detail:"Restore the approved capability profile.",hint:"Use the known-good service policy."}
      ],
      decision:{"prompt":"A trusted service requests a new storage capability. How should Aegis respond?","bestChoice":"constrain","options":[{"id":"constrain","label":"Deny the new capability and validate the service baseline","reason":"Correct. Least privilege contains the request without interrupting approved service functions."},{"id":"grant","label":"Grant the capability temporarily","reason":"Temporary privilege expansion still creates an unverified trust path."},{"id":"remove","label":"Delete the service immediately","reason":"This may cause an outage before the trust relationship has been properly assessed."}]},
      reward:{xp:500,credits:360,research:4},
      debrief:"The service was returned to its approved capability profile. Aegis has identified a wider pattern of trust-boundary probing."
    },
    {
      id:"cold-storage",chapter:2,title:"Cold Storage",codename:"OPERATION DEEP FREEZE",
      difficulty:"ADVANCED",threat:"HIGH",duration:"15–20 MIN",
      briefing:"A storage integrity alert reports inconsistent metadata across a cold archive. Some records remain readable, but the recovery window is closing. Verify the snapshot chain and restore the damaged index.",
      intel:["Archive metadata mismatch","Two snapshots available","Recovery window is limited"],
      objectives:[
        {id:"inspect",title:"Inspect the integrity report",detail:"Identify the damaged archive index.",hint:"Separate metadata errors from content corruption."},
        {id:"isolate",title:"Protect the affected volume",detail:"Place the volume in read-only quarantine.",hint:"Prevent additional writes while investigating."},
        {id:"scan",title:"Verify snapshot chain",detail:"Validate the available recovery points.",hint:"Choose a snapshot with a verified integrity status."},
        {id:"restore",title:"Restore the archive index",detail:"Recover the index from the last valid snapshot.",hint:"Keep the original evidence in quarantine."}
      ],
      decision:{"prompt":"The archive metadata is inconsistent. Which recovery approach preserves the strongest evidence?","bestChoice":"readonly","options":[{"id":"readonly","label":"Quarantine read-only, then verify snapshots","reason":"Correct. Preventing writes preserves the original state while the recovery chain is validated."},{"id":"overwrite","label":"Overwrite the archive with the newest snapshot","reason":"The newest snapshot may also be damaged and overwriting destroys useful evidence."},{"id":"rebuild","label":"Rebuild the index before checking snapshots","reason":"Rebuilding before validation risks embedding corruption into the recovered archive."}]},
      reward:{xp:550,credits:400,research:4},
      debrief:"The archive index was restored from a verified snapshot. The original damaged metadata has been preserved for analysis."
    },
    {
      id:"blackout-signal",chapter:3,title:"Blackout Signal",codename:"OPERATION NIGHT SIGNAL",
      difficulty:"EXPERT",threat:"CRITICAL",duration:"20–25 MIN",
      briefing:"Multiple infrastructure nodes report coordinated anomalies. You have enough resources to protect only two nodes immediately. Prioritise, investigate the common signal, and stabilise the Aegis network.",
      intel:["Three infrastructure nodes affected","Shared anomaly signature","Limited response capacity"],
      objectives:[
        {id:"inspect",title:"Correlate incident telemetry",detail:"Identify the shared anomaly across affected nodes.",hint:"Look for a common pattern rather than treating alerts independently."},
        {id:"isolate",title:"Contain the highest-risk node",detail:"Quarantine the node showing the greatest instability.",hint:"Prioritise impact and spread potential."},
        {id:"scan",title:"Run a network-wide integrity check",detail:"Verify the remaining connected nodes.",hint:"Use the correlated signal to guide the scan."},
        {id:"restore",title:"Stabilise the Aegis network",detail:"Restore trusted routing and recover the affected node.",hint:"Restore connectivity only after validation."}
      ],
      decision:{"prompt":"Only two of three nodes can be protected immediately. What should guide priority?","bestChoice":"risk","options":[{"id":"risk","label":"Prioritise impact, spread potential and node instability","reason":"Correct. Risk-based triage protects the wider network rather than simply choosing the loudest alert."},{"id":"oldest","label":"Protect the node with the oldest alert","reason":"Alert age alone does not measure operational impact or propagation risk."},{"id":"random","label":"Choose a node at random to avoid bias","reason":"Random selection ignores available telemetry and wastes limited response capacity."}]},
      reward:{xp:800,credits:650,research:6},
      debrief:"The network has stabilised. The signal points to a coordinated actor with knowledge of Aegis trust boundaries."
    }
  ];
})(window);