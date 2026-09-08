# Auxiliary Sensor and IMU Synchronization

This document defines how optional IMU and other auxiliary sensors should be synchronized with ADS1299 acquisition without disturbing the DRDY-critical path.

## 1. Keep acquisition domains separate

The ADS1299 sample clock/DRDY timing is the primary EEG acquisition domain. IMU or auxiliary sensors may have their own sampling clocks and update rates.

Do not assume samples are synchronized just because they are read by the same MCU.

## 2. Recommended timestamp model

For every auxiliary sample, record:

- sensor ID/type;
- sensor-local sample counter if available;
- host/device timestamp captured as close as practical to the sample-ready event;
- relation to the ADS1299 device timestamp basis;
- configured auxiliary sample rate.

For analysis, align data using the explicit timestamps/counters rather than nearest array index.

## 3. Hardware event sync

If the auxiliary sensor exposes an interrupt/data-ready pin, route it to a host/FPGA input when timing matters.

Recommended architecture:

- ADS1299 DRDY -> high-priority EEG acquisition event;
- IMU DRDY -> separate auxiliary event;
- common monotonic timer timestamps both events;
- lower-priority code packages/transports both streams.

Do not perform long I2C/SPI transactions for an IMU inside the ADS1299 DRDY ISR.

## 4. Shared SPI bus caution

If ADS1299 and IMU share one SPI controller:

- ADS1299 acquisition has priority near DRDY;
- IMU transactions must fit in the remaining service window;
- use independent CS lines;
- verify bus mode changes if the devices require different CPOL/CPHA;
- ensure no RTOS task changes SPI configuration while an ADS1299 DMA transfer is active.

For high-rate/64-channel systems, place the IMU on a separate SPI/I2C peripheral when practical.

## 5. IMU placement

If the IMU is used to characterize head motion:

- place it mechanically close to the headstage/electrode assembly whose motion is being measured;
- document axis orientation relative to the cap/head coordinate system;
- mark orientation on PCB/enclosure;
- avoid placing it where cable flex dominates the measured motion instead of head motion.

## 6. Motion artifact studies

Keep raw EEG and raw IMU synchronized. Do not pre-filter away the artifact you later want to model.

Useful metadata:

- accelerometer range;
- gyroscope range;
- sensor ODR;
- filter settings;
- axis orientation;
- sensor timestamp/counter behavior;
- dropped auxiliary samples.

## 7. Other auxiliary sensors

The same architecture can be used for:

- trigger/marker input;
- PPG;
- EDA;
- external event button;
- respiration sensor;
- optical synchronization pulse;
- experiment-stimulus TTL.

Give each stream its own source ID and timestamp basis.

## 8. External trigger input

For ERP/experiment synchronization, an external digital trigger input should be timestamped at the hardware interrupt/FPGA edge whenever possible.

Do not rely only on a PC GUI event time if sub-sample timing matters.

Document:

- voltage level;
- polarity;
- electrical isolation/protection;
- edge timestamp source;
- trigger code width/encoding;
- behavior if triggers arrive faster than software can process them.

## 9. Validation

Test synchronization using a repeatable event visible in more than one domain, for example a hardware pulse routed to a trigger input while also driving a measurable auxiliary channel in a safe bench setup.

Measure:

- constant offset;
- jitter;
- clock drift over long recordings;
- dropped samples/events;
- behavior under USB/network/radio load.

## Acceptance gate

Auxiliary synchronization is acceptable only when timestamp ownership, axis/channel mapping and measured timing behavior are documented and repeatable at the intended 64-channel acquisition load.
