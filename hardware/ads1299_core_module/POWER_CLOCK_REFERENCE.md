# Power, Clock and Reference Design

This file defines the checks that must be completed around the ADS1299 power, clock and reference architecture. Exact electrical limits and sequencing must be verified against the current TI ADS1299 datasheet and the actual board schematic.

## 1. Power domains

Document every ADS1299-related rail explicitly:

- rail name;
- nominal voltage;
- tolerance;
- source/regulator;
- expected current;
- analog or digital load ownership;
- local decoupling population;
- power-up/down relationship to RESET/PWDN.

Do not treat a rail as valid simply because its DC multimeter reading is correct. Check transient noise and switching activity at the device pins when the system is sampling and transmitting data.

## 2. Local decoupling

Place device-local bypass capacitors close to the associated ADS1299 supply/reference pins with short current loops and an uninterrupted return path.

Review:

- smallest high-frequency bypass placement first;
- bulk/local reservoir placement;
- via count and loop area;
- whether digital return current crosses the analog reference/input region;
- whether regulator output-capacitor requirements are satisfied.

Do not copy a capacitor network without checking the selected regulator/reference stability requirements.

## 3. Analog vs digital noise sources

Keep the ADS1299 analog/reference area physically separated from:

- USB PHY/connector return currents;
- Ethernet magnetics/PHY clocks;
- Wi-Fi/BLE radios and antennas;
- switching regulators/inductors;
- high-current LEDs or motors;
- MCU/FPGA clock trees;
- fast level shifters.

The important principle is return-current control and coupling reduction, not merely drawing separate AGND/DGND labels on the schematic.

## 4. Clock strategy

One ADS1299 core module must have one clearly documented clock mode:

- internal oscillator; or
- external clock input; or
- module-generated clock output when intentionally used by the system.

For multi-device systems, clock ownership must be explicit. Do not allow two modules to drive the same clock net.

### What to validate

- clock starts reliably after power/reset;
- frequency is correct for the selected mode;
- logic levels meet the receiving pin requirements;
- routing/fan-out does not create unacceptable ringing or edge degradation;
- multi-device sampling relationship is measured when synchronization matters.

## 5. Reference strategy

The reference voltage directly affects ADC scaling and noise performance. For every board revision, document:

- internal vs external reference choice;
- reference node/component implementation;
- decoupling/filtering;
- expected reference voltage;
- startup behavior;
- which software scaling value corresponds to that configuration.

The host-side conversion from ADC counts to volts must use the same Vref and PGA gain that the actual hardware/configuration used.

## 6. Reset / power-down behavior

RESET and PWDN must have deterministic states during power-up. Avoid relying on floating GPIOs from an MCU that is itself still booting.

Document whether the module uses:

- pull-up/pull-down straps;
- supervisor/reset IC;
- MCU-controlled reset;
- hardware delay;
- carrier-controlled PWDN.

## 7. Bring-up measurements

Before first EEG use, capture/record:

- all ADS1299 rails at idle;
- rails during continuous SPI streaming;
- rails during USB/network/radio activity;
- reference node stability;
- clock waveform;
- RESET/START timing;
- input-short noise with transport disabled and enabled.

This separates power/reference coupling from firmware parsing problems.

## 8. Multi-module power planning

For 8 ADS1299 modules, size regulators and distribution from measured per-module current plus margin. Check:

- connector voltage drop;
- ground/return impedance;
- startup inrush;
- simultaneous radio/USB/FPGA load if shared;
- thermal rise;
- whether one failing module can disturb the shared rail.

Do not assume that a supply adequate for one module will remain quiet when eight are populated.

## Acceptance gate

Power/clock/reference design is ready for the next stage only when the real board has stable rails, deterministic reset/clock behavior and a repeatable input-short noise baseline under the intended digital activity.
