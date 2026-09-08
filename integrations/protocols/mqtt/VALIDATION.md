# MQTT validation checklist

## Topic and payload contract

- [ ] system ID is stable and non-sensitive;
- [ ] device/source identity is explicit;
- [ ] raw EEG payload is exactly 49 canonical bytes when binary mode is used;
- [ ] canonical sync/version/CRC is validated by subscribers;
- [ ] status/event/command JSON schemas are versioned or documented;
- [ ] clients tolerate additive metadata fields where intended.

## Broker/network behavior

Test at minimum:

- [ ] normal publish/subscribe;
- [ ] broker restart;
- [ ] client reconnect;
- [ ] temporary network loss;
- [ ] slow subscriber;
- [ ] publisher queue saturation;
- [ ] duplicate delivery behavior for the chosen QoS;
- [ ] stale retained status/configuration handling if retained messages are used.

## Backpressure

- [ ] acquisition-to-MQTT queue is bounded;
- [ ] overflow policy is explicit;
- [ ] disconnect buffering is bounded;
- [ ] raw EEG is not published from DRDY ISR context;
- [ ] queue/drop counters are observable;
- [ ] reconnect does not silently erase discontinuity information.

## Commands

- [ ] commands and data use separate topics;
- [ ] command payload validation is strict;
- [ ] acknowledgements report actual application of the command;
- [ ] duplicate command delivery is safe/idempotent where required;
- [ ] unauthorized clients cannot publish control commands in production deployments.

## 64-channel qualification

- [ ] exact publish rate is calculated for eight devices;
- [ ] broker/client sustained throughput is measured at intended sample rate;
- [ ] source mapping remains deterministic;
- [ ] missing device messages are detected;
- [ ] aggregation policy, if used, preserves sequence/timestamps/status;
- [ ] long-duration memory/disk/network behavior is measured.

## Qualification record

Record:

- broker implementation/version;
- broker hardware/hosting environment;
- publisher hardware/software revision;
- subscriber implementation;
- network path;
- QoS per topic class;
- retained-message policy;
- sample rate and device count;
- message rate and payload rate;
- soak-test duration;
- reconnect/drop/duplicate counters;
- CPU/memory/storage usage where relevant.

Passing this checklist validates MQTT interoperability only. It does not validate analog EEG quality, electrical safety, EMC or clinical/medical compliance.
