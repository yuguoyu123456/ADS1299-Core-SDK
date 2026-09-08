# Companion bridge validation checklist

## Adapter contract

- [ ] `python test_adapter_contract.py` passes;
- [ ] vendor/platform adapter implements only documented capabilities;
- [ ] endpoint ID and adapter name are stable and non-sensitive;
- [ ] unsupported operations fail explicitly;
- [ ] software command acceptance is not mislabeled as acoustic onset.

## Connection lifecycle

- [ ] connect/disconnect state is explicit;
- [ ] failed connection does not crash EEG acquisition;
- [ ] reconnect behavior is documented;
- [ ] stale commands are not replayed unintentionally;
- [ ] endpoint replacement/reselection is logged.

## Timing

- [ ] host monotonic dispatch time is recorded;
- [ ] endpoint timing is recorded only when actually exposed by the API;
- [ ] physical acoustic onset is measured independently when timing accuracy requires it;
- [ ] EEG device timestamp remains separate from companion-controller time;
- [ ] repeated trials characterize latency/jitter rather than relying on one measurement.

## EEG independence

- [ ] ADS1299 packet sequence/timestamps continue to be validated independently;
- [ ] audio endpoint disconnect does not silently renumber EEG samples;
- [ ] experiment abort/pause caused by audio failure generates an explicit event marker;
- [ ] vendor adapter cannot block DRDY/acquisition handling.

## Proprietary-device boundary

For each supported endpoint record:

- exact device model;
- public SDK/API/documentation used;
- supported OS/version;
- commands actually tested;
- timing information actually available;
- unsupported capabilities.

Do not claim direct integration with a closed consumer device without this evidence.

## Security/privacy

- [ ] participant/study identifiers are not unnecessarily embedded in endpoint names or network topics;
- [ ] network-facing APIs use appropriate authentication/access controls;
- [ ] logs avoid storing unnecessary device/account identifiers;
- [ ] vendor cloud dependency, if any, is documented separately.

Passing this checklist validates the companion integration layer only. It does not establish audio/hearing safety, ADS1299 electrical safety, EMC or medical-device compliance.
