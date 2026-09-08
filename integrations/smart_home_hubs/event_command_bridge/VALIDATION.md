# BCI event command bridge validation checklist

## Gate logic

- [ ] `python test_command_gate.py` passes;
- [ ] command allowlist is explicit;
- [ ] confidence threshold is documented;
- [ ] repeat-confirmation count is documented;
- [ ] cooldown/debounce is documented;
- [ ] invalid EEG suppresses new command intents;
- [ ] user/system disable suppresses new command intents;
- [ ] non-finite confidence values are rejected.

## False-positive testing

Test deliberately with:

- [ ] low-confidence proposals;
- [ ] alternating inconsistent commands;
- [ ] repeated identical proposals;
- [ ] stale/invalid signal intervals;
- [ ] classifier reconnect/restart;
- [ ] rapid repeated decisions during cooldown;
- [ ] command names outside the allowlist.

Record accepted/rejected counts and reasons.

## Dispatch/acknowledgement

- [ ] gate acceptance is logged separately from automation dispatch;
- [ ] automation acknowledgement/result is logged separately;
- [ ] target-device failure does not rewrite the gate decision history;
- [ ] repeated/retried dispatch has an explicit policy;
- [ ] bridge restart does not automatically replay the last command.

## User override / failsafe

- [ ] a visible/manual disable path exists in the prototype;
- [ ] disconnect defaults to no new command;
- [ ] stale classifier output cannot remain active indefinitely;
- [ ] target automation has its own normal/manual control path independent of BCI;
- [ ] demonstration-only automations are labeled as such.

## Scope restriction

Do not qualify this research bridge as a sole control mechanism for safety-critical/hazardous equipment. Validation should use low-risk, reversible demonstration actions with easy manual override.

## Qualification record

Record classifier/version, command set, thresholds, repeat/cooldown settings, EEG sample rate/channel count, automation adapter/hub, test duration, proposal count, false positives, accepted commands, rejected reasons, network failures and manual override tests.

Passing this checklist validates a research command-gating path only. It does not establish assistive-device certification, home-automation safety certification, clinical validity or medical-device compliance.
