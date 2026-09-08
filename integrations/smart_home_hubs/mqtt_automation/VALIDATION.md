# MQTT smart-home automation validation checklist

## Message schema

- [ ] `python test_automation_messages.py` passes;
- [ ] schema version is validated;
- [ ] intent ID is non-empty and unique enough for the experiment/session;
- [ ] command is validated against an allowlist;
- [ ] source EEG sequence is preserved;
- [ ] confidence is finite and bounded;
- [ ] acknowledgement references the same intent ID.

## Broker/network

- [ ] broker reconnect is tested;
- [ ] duplicate message behavior is tested for the selected MQTT policy;
- [ ] stale retained command messages cannot trigger actions unexpectedly;
- [ ] disconnect buffering is bounded;
- [ ] authentication/ACLs prevent unauthorized command publication in non-demo deployments;
- [ ] participant identifiers are not embedded in topic names.

## Automation adapter

- [ ] downstream adapter has its own allowlist;
- [ ] command mapping is documented;
- [ ] target/hub acknowledgement is distinct from MQTT publish success;
- [ ] timeout/retry behavior is explicit;
- [ ] restart does not replay a stale intent automatically;
- [ ] user/manual override exists.

## Ecosystem qualification

For every claimed adapter (Home Assistant, openHAB, Node-RED or other), record:

- platform/version;
- adapter/configuration revision;
- broker/version;
- supported command names;
- target low-risk automation used for testing;
- acknowledgement/result behavior;
- reconnect/duplicate tests;
- known limitations.

Do not claim Matter-facing or vendor-specific support unless the exact path has been implemented/tested.

## End-to-end BCI path

- [ ] raw EEG is not directly mapped to actuator state;
- [ ] BCI classifier output passes through the command gate;
- [ ] invalid/stale EEG blocks new intents;
- [ ] intent/dispatch/ack/result are separately logged;
- [ ] false-positive tests are performed;
- [ ] target actions are reversible and manually overrideable for research/demo use.

Passing this checklist validates a research smart-home bridge only. It does not establish home-control safety certification, assistive-device certification, clinical validity or medical-device compliance.
