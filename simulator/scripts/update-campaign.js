'use strict';
const fs = require('fs');
const path = require('path');

const campaignPath = path.resolve('simulator/web/native-campaign.js');
let content = fs.readFileSync(campaignPath, 'utf8');

// 1. Elaborated Stage Guides metadata
const ELABORATED_DATA = `
var STAGE_NAMES = [
  'Reconnaissance & Tool Init',
  'Deep Forensic Triage',
  'Verification & Inode Seal',
  'Subsystem Defense Check'
];

var ELABORATED_HINTS = {
  op1: [
    {
      ioc: 'Process PID 1337 | svc_sync_worker | Parent mismatch',
      stages: [
        'Open Neural Monitor or System Diagnostics from dock or top bar.',
        'Click on suspicious worker PID 1337 or execute ps in Terminal to reveal spoofed parent PPID 1.',
        'Run netstat in Terminal or click CyberSec Arsenal to correlate outbound socket descriptors.'
      ],
      insight: 'Adversaries often use process masquerading (MITRE T1036) to blend malicious workers into legitimate daemon trees.'
    },
    {
      ioc: 'NetFlow TCP 185.220.101.5:8443 (TLS 1.3 C2 Relay)',
      stages: [
        'Open Terminal (aegis_shell) from dock or desktop.',
        'Execute ps to list all running threads and inspect anomalous execution arguments.',
        'Execute netstat to correlate process PID 1337 with the active remote connection to 185.220.101.5:8443.'
      ],
      insight: 'Network session correlation (MITRE T1071) connects endpoint process telemetry directly to external C2 communication.'
    },
    {
      ioc: 'Packet Payload Entropy: 7.94 / 8.00 (Encrypted C2 Beacon)',
      stages: [
        'Launch CyberSecurity Arsenal from dock.',
        'Navigate to Deep Packet Sniffer and click suspicious packet stream destined for 185.220.101.5:8443.',
        'Verify continuous beacon cadence (60s jittered intervals) and correlate with active Shield edge filters.'
      ],
      insight: 'High Shannon entropy in network packet data indicates cryptographic framing, typical of modern stealth implants.'
    },
    {
      ioc: 'File Inode /userland/data/svc-sync-cache.bin (Modified: 02:13)',
      stages: [
        'Launch Files & Storage Manager (AegisFS) from dock.',
        'Inspect metadata, SHA-256 hash, and timestamps of svc-sync-cache.bin in the filesystem view.',
        'Cross-reference file modification time with the 02:13 incident alert time.'
      ],
      insight: 'Attackers drop temporary staging artifacts before initiating process injection; preserving file metadata is vital for forensics.'
    },
    {
      ioc: 'Timeline: Modified Artifact -> Process Spawn -> Outbound Beacon',
      stages: [
        'Open Aegis Mind AI Controller from dock or top bar.',
        'Prompt Aegis Mind to correlate the timeline between file modification, process spawn, and outbound beacon.',
        'Record the reconstructed timeline sequence in your Case Notes dossier journal.'
      ],
      insight: 'Multivariate timeline reconstruction links volatile and non-volatile digital evidence to prove causality.'
    },
    {
      ioc: 'Decision Gate: Targeted Egress Deny Policy vs Host Disconnect',
      stages: [
        'Open Operations panel and review containment trade-offs.',
        'Evaluate operational readiness impact: targeted egress preserves research cluster availability.',
        'Commit your strategic command choice: Targeted egress block.'
      ],
      insight: 'Real-world incident response balances threat containment speed against collateral service disruption.'
    },
    {
      ioc: 'Shield Firewall Rule #7: Deny Ingress/Egress on 185.220.101.5',
      stages: [
        'Launch Aegis Shield Firewall from dock.',
        'Toggle firewall Rule #7 (Untrusted Ingress / C2 Deny policy) to block hostile relay 185.220.101.5.',
        'Verify packet drop counter increases and audit edge route health.'
      ],
      insight: 'Boundary filtering (MITRE M1037) stops data exfiltration immediately at the perimeter without terminating legitimate services.'
    },
    {
      ioc: 'Aegis Guard Ring-0 Telemetry | Severed Socket Verification',
      stages: [
        'Click Run Aegis Guard AI Scan in Aegis Shield to initiate behavioral audit.',
        'Switch to Terminal shell.',
        'Execute verify-containment to confirm the remote hostile socket has been terminated.'
      ],
      insight: 'Containment verification ensures that active established connections are closed and no keep-alive beacons persist.'
    },
    {
      ioc: 'Merkle Block #4 | Cryptographic Inode Divergence',
      stages: [
        'Open Merkle Filesystem Engine from dock or desktop.',
        'Click Run Merkle Hash Scan to locate the diverged cryptographic leaf block.',
        'Execute verify-root in Terminal to inspect the divergent Merkle parent hash.'
      ],
      insight: 'Merkle DAG filesystems guarantee tamper-evidence: any unauthorized write diverges parent hashes up to the root.'
    },
    {
      ioc: 'Checkpoint chkpt-0003 (Pre-Incident Alpha Snapshot)',
      stages: [
        'Select clean checkpoint chkpt-0003 in Merkle Filesystem and trigger atomic rollback.',
        'Execute verify-root in Terminal to confirm cryptographic root hash integrity.',
        'Check Task Optimizer or service-health to verify normal system operation.'
      ],
      insight: 'Atomic snapshot rollbacks restore corrupted filesystem blocks in sub-10s without requiring host reboot.'
    }
  ]
};
`;

console.log('Script loaded, ready to patch native-campaign.js');
