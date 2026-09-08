# 03 - Input Short and Noise Validation

Run this after the internal test passes and before connecting electrodes.

## Goal

Measure the acquisition system with the ADS1299 inputs routed to the device's internal short/test condition so that external electrode/contact noise is removed from the problem.

Use the ready profile:

- `ads1299_profile_input_short(...)`

## Why this step matters

A clean internal test proves the digital path, but it does not prove that the selected PGA/data-rate configuration has an acceptable noise floor. Input-short mode gives a repeatable baseline for comparing firmware settings, power configurations and PCB revisions.

## Recommended procedure

1. Stop continuous streaming cleanly.
2. Apply the input-short profile.
3. Start with one ADS1299 at 250 SPS.
4. Record a continuous block long enough to inspect both time-domain behavior and a spectrum.
5. Save the raw signed 24-bit counts before any digital filter.
6. Record the exact data rate, PGA gain, reference voltage/configuration, clock source and firmware commit.

Do not use software filtering to make a noisy board appear quiet during this test.

## Convert counts consistently

The repository contains packet/physical conversion helpers. For an ideal signed 24-bit ADC representation, use the same Vref and PGA gain assumptions on every comparison. Do not mix raw counts, ADC-input volts and electrode-referred microvolts in one plot without labels.

## What to calculate

For each channel, record at least:

- mean raw count;
- peak-to-peak range;
- RMS/noise standard deviation over the chosen bandwidth;
- obvious periodic tones;
- channel-to-channel differences;
- dropped packet count / sequence gaps.

When comparing hardware versions, use the same sample rate, gain, clock, reference, power source, capture duration and processing bandwidth.

## Common failure patterns

### Strong 50/60 Hz or harmonics even in input-short mode

Investigate board power, grounding, reference/analog supply routing, digital coupling, USB/common-ground paths and measurement setup. Do not assume the source is the electrode because no electrode is involved here.

### One channel much noisier than the others

Check local soldering, input protection/filter components, routing symmetry, channel register configuration and contamination/leakage around that input network.

### Noise increases with SPI/USB/Wi-Fi activity

Look for digital return-current coupling, poor power-domain isolation, insufficient decoupling or acquisition/transport scheduling that disturbs timing. Wireless transmission should be isolated from the acquisition critical path.

## Pass criteria

This step passes when:

- all eight channels produce valid signed data;
- there are no unexplained packet/frame losses;
- the noise floor is stable and repeatable under the same setup;
- no single channel has an unexplained abnormal baseline/noise signature;
- enabling lower-priority transport does not corrupt acquisition.

Record the measured values as engineering data. This guide intentionally does not invent a universal production-noise limit because acceptable limits depend on the complete analog design, gain, bandwidth and product requirement.
