# Smart-home safety and privacy boundary

This directory defines non-negotiable engineering boundaries for ADS1299/BCI smart-home research integrations.

## Default failure state

When any of these conditions occurs:

- EEG transport disconnect;
- stale EEG/feature state;
- classifier failure;
- automation bridge disconnect;
- hub restart;
- command schema error;
- unknown target state;

the default should be **no new BCI command intent**.

Do not replay the last accepted command automatically after recovery unless the experiment protocol explicitly requires and logs that behavior.

## Manual override

Every research/demo automation should have an ordinary non-BCI control path and an obvious way to disable BCI command generation.

Manual/user override should take precedence over the BCI bridge.

## Low-risk scope

Generic research integrations should use reversible, low-risk outputs such as non-critical scene/UI/media changes.

Do not use this repository's generic BCI bridge as the sole control path for:

- locks/access control;
- fire/security alarms;
- heating/cooking appliances;
- emergency functions;
- medical equipment;
- machinery or other hazardous actuators.

## Data minimization

A smart-home hub usually does not need raw EEG or participant identity to execute an already accepted command intent.

Prefer sending only the minimum required fields:

- intent ID;
- allowlisted command;
- source sequence/time reference if needed for audit;
- confidence/validity summary if needed;
- non-sensitive system ID.

Keep raw EEG and participant/study metadata in the research data system, not broadly distributed to home-automation components.

## Identifier hygiene

Avoid putting participant names, diagnoses, study groups or other sensitive labels into:

- MQTT topic names;
- device names;
- OSC paths;
- hub entity names;
- network hostnames;
- routine debug logs.

Use experiment-local pseudonymous identifiers where identity linkage is necessary under the study's data-governance plan.

## Logging

Keep an audit trail of:

```text
BCI proposal
gate decision
intent ID
dispatch attempt
hub/adapter acknowledgement
result/failure
manual override/recovery
```

But do not log unnecessary raw EEG or sensitive metadata into automation-system logs.

## Security

Network-facing deployments should define authentication, authorization, encrypted transport where appropriate, least-privilege broker/hub permissions, update management and secure secret storage.

A topic name or local-LAN assumption is not an access-control mechanism.
