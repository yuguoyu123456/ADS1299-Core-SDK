# Hardware Accessories

This folder covers hardware that connects the ADS1299 core/multi-device system to electrodes, power, isolation, host interfaces and auxiliary sensors.

Accessories are part of signal quality and safety. A good ADS1299 core can still perform poorly when connected through a noisy cable, unsafe power path or ambiguous electrode adapter.

## Sections

- `ELECTRODE_ADAPTERS.md` — electrode/cap connector and channel-mapping rules.
- `ISOLATION_AND_POWER.md` — human-connected system isolation and power-boundary design review.
- `CABLES_AND_BACKPLANE.md` — cable, connector and interconnect signal-integrity rules.
- `AUX_SYNC_AND_IMU.md` — auxiliary sensor/IMU synchronization interface strategy.
- `VALIDATION_CHECKLIST.md` — accessory-level acceptance tests.

## General rule

Do not treat an accessory as a passive afterthought. Every added connector/cable/adapter can add:

- leakage;
- capacitance;
- source impedance;
- common-mode coupling;
- digital/RF coupling;
- channel-mapping errors;
- fault-current paths.

Validate the complete chain actually used in the experiment/product.
