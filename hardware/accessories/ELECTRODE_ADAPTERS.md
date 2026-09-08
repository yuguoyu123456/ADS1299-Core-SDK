# Electrode Adapters and Channel Mapping

Electrode adapters must preserve signal integrity, safety intent and deterministic channel mapping.

## 1. One permanent electrical map

For every adapter, publish a table containing:

- connector pin;
- cable conductor/color when applicable;
- global channel number;
- ADS1299 device index;
- ADS1299 channel number;
- electrode label/montage label;
- reference/BIAS role;
- shield/chassis connection if used.

Do not rely on wire color alone as the channel definition.

## 2. Differential-path symmetry

If an adapter carries CHnP and CHnN as a pair:

- keep pair routing together;
- avoid adding asymmetric protection/filter components;
- avoid routing one side next to digital/RF lines while the other is shielded/quiet;
- keep connector/contact resistance reasonably matched.

## 3. Common reference adapters

If many channels share a reference electrode, the adapter must clearly define:

- one reference contact vs multiple contacts;
- where the electrical fan-out occurs;
- whether SRB1/SRB2 is used;
- whether the core module or adapter contains series/protection elements;
- whether more than one reference path can accidentally be connected in parallel.

## 4. BIAS/common-mode electrode

Keep BIAS/common-mode electrode routing distinct from system ground and the measurement reference.

The adapter documentation must show:

- which connector pin carries BIAS/common-mode drive;
- which hardware owns the active BIAS output;
- protection/current-limiting path;
- what happens when the electrode is disconnected.

## 5. Shielding

Cable shield strategy must be deliberate.

Possible choices include:

- no shield;
- passive shield to a defined chassis/system node;
- driven shield/guard where the full design supports it.

Do not connect shields to random ground points at both ends without checking common-mode current paths, safety boundary and interference behavior.

## 6. Connector selection

Evaluate:

- contact resistance stability;
- number of insertion cycles;
- accidental touch exposure;
- keying/polarization;
- strain relief;
- cable pull-out force;
- cleaning/disinfection compatibility when relevant;
- whether reference/BIAS pins can be confused with measurement channels.

## 7. Active electrode / preamplifier adapters

If active electrodes or headstage electronics are used, document:

- power delivered to the electrode;
- return path;
- input/output signal range;
- gain/filtering;
- connector fault behavior;
- added latency/synchronization if digitization occurs remotely.

Do not assume a passive-electrode ADS1299 input network is automatically compatible with an active electrode output.

## 8. Channel-map verification

Before human EEG, perform a deterministic adapter test:

1. inject/short/test one connector channel at a time using a safe bench setup;
2. record host output;
3. confirm exactly the expected global channel changes;
4. verify reference/BIAS pins separately;
5. save the verified adapter map with its hardware revision.

## 9. Artifact checks

Test the actual cable/adapter for:

- cable movement/microphonics;
- connector touch/motion artifact;
- 50/60-Hz pickup;
- nearby USB/RF coupling;
- channel-to-channel crosstalk;
- reference contact loss.

An adapter that passes continuity can still have poor EEG noise performance.
