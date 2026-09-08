# EEG + eye-tracking synchronization validation checklist

## Clock mapping

- [ ] EEG and eye-tracker clocks are treated as separate time domains;
- [ ] mapping method/evidence is documented;
- [ ] offset and drift/scale are estimated or justified;
- [ ] mapping residual/error is measured;
- [ ] long-duration drift is evaluated;
- [ ] restarts create new mapping segments when needed.

## Alignment helper

- [ ] `python test_multimodal_sync.py` passes;
- [ ] eye samples are sorted by mapped/common time;
- [ ] invalid eye samples remain invalid;
- [ ] maximum pairing tolerance is explicit;
- [ ] samples outside tolerance produce missing gaze rather than forced matches;
- [ ] interpolation/hold-last-value is only used when explicitly documented.

## Eye data metadata

For the target tracker record available fields and semantics:

- [ ] gaze coordinates/direction reference frame;
- [ ] validity/confidence;
- [ ] eye identifier if applicable;
- [ ] pupil metric units if applicable;
- [ ] head pose/reference frame if used;
- [ ] source sample rate;
- [ ] device/SDK/OS version;
- [ ] timestamp origin/units.

Do not invent fields the device does not expose.

## Visual event cross-check

For task experiments:

- [ ] visual event IDs are present in the experiment log;
- [ ] event timing can be located in EEG time;
- [ ] the same event can be located in eye-tracker time where supported;
- [ ] mapped event residuals are inspected throughout the recording;
- [ ] frame/presentation and gaze alignment are not inferred only from packet arrival order.

## Data loss

- [ ] EEG sequence gaps are tracked;
- [ ] eye-tracker invalid/missing samples are counted;
- [ ] transport disconnects are logged;
- [ ] missing gaze does not shift later EEG rows;
- [ ] missing EEG does not shift later gaze samples.

## 64-channel EEG

- [ ] eight ADS1299 sources are synchronized before gaze pairing;
- [ ] global channel mapping is stable;
- [ ] per-device timing diagnostics remain available;
- [ ] one gaze sample is paired against the intended assembled EEG epoch;
- [ ] eye tracking is not used to mask inter-device synchronization errors.

## Qualification record

Record exact glasses/eye-tracker model, SDK/runtime version, ADS1299 hardware/firmware revision, sample rates, clock mapping method, pairing tolerance, test duration, mapping residual statistics and loss/invalid-sample counts.

Passing this checklist validates multimodal synchronization engineering only. It does not establish clinical validity, eye-tracker accuracy beyond its validated configuration, electrical safety or medical-device compliance.
