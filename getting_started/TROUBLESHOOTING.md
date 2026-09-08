# Troubleshooting Guide

Use this guide by symptom. Change one variable at a time and keep captures/measurements with the firmware commit used.

## 1. Device ID is always 0x00

Check:

1. ADS1299 power rails and ground.
2. RESET/PWDN state.
3. Whether MISO/DOUT is actually connected to the selected host pin.
4. CS activity during the command.
5. SPI Mode 1 configuration.
6. Whether the device clock is present/configured.
7. Whether the host is reading the correct SPI peripheral.

Use a logic analyzer before adding arbitrary delays.

## 2. Device ID is always 0xFF

Likely causes include:

- MISO floating/high;
- CS never becoming active;
- ADS1299 unpowered/in reset;
- incorrect pin mux;
- open solder joint or connector problem.

## 3. ID works once, then register reads fail

Possible causes:

- continuous-data mode was entered and register access is attempted without `SDATAC`;
- another task/peripheral is sharing or reconfiguring the SPI bus;
- CS timing is incorrect between commands;
- reset/start sequencing is not deterministic.

## 4. DRDY never toggles

Check:

- acquisition has actually been started;
- sample-rate/config registers were written successfully;
- START pin/command state;
- clock source/configuration;
- DRDY pin mux and interrupt polarity;
- whether the signal is visible electrically at the ADS1299 pin before blaming firmware.

## 5. DRDY toggles but all channels are zero or saturated

Check:

- exact 27-byte read length;
- SPI Mode 1;
- channel MUX/profile configuration;
- 24-bit byte order;
- sign extension;
- whether the device is still in test/shorted-input mode;
- analog/reference configuration on the real board.

## 6. Internal test works but live EEG does not

This usually moves the fault boundary toward the analog front end. Check:

- channel input network;
- electrode/reference/bias wiring;
- protection/filter components;
- connector pinout;
- reference and BIAS configuration;
- contact quality;
- board contamination/leakage;
- common-mode interference.

Do not change digital packet parsing if internal-test packets are already valid.

## 7. Input-short noise is unexpectedly high

Investigate:

- analog/reference supply noise;
- ground/return-current coupling;
- SPI/USB clock coupling;
- insufficient local decoupling;
- digital activity placed too close to analog routing;
- USB-ground/common-ground effects;
- radio activity on wireless targets;
- board assembly/cleanliness.

Compare raw data with the same gain, sample rate and bandwidth before and after each change.

## 8. One channel is bad and seven are good

Suspect a local channel issue first:

- input network component error;
- connector/solder defect;
- wrong CHnSET write;
- PCB contamination/leakage;
- channel-order mismatch on the host.

Use internal-test and input-short modes to separate configuration from external-electrode problems.

## 9. Negative values appear as huge positive numbers

The 24-bit ADS1299 sample was not sign-extended correctly. Use the repository's canonical frame/packet decode helpers rather than casting three bytes directly to an unsigned integer.

## 10. Correct samples but periodic packet loss

Measure where the loss occurs:

- ADS1299 frame acquisition;
- MCU ring buffer;
- USB/UART/BLE/Wi-Fi/Ethernet queue;
- host read loop;
- disk writer.

Track sequence numbers and per-stage overflow counters. Do not assume every sequence gap is an SPI failure.

## 11. Data becomes unstable when Wi-Fi/BLE starts

Separate RF/network work from the acquisition path:

- short DRDY ISR;
- dedicated high-priority acquisition context;
- bounded queue/ring between acquisition and networking;
- no socket/BLE notification waits in the ISR;
- explicit overflow policy/counters.

Also investigate board-level RF/power coupling if the raw analog noise itself increases.

## 12. USB host reads odd lengths

That is normal for stream-oriented transfers. A host read boundary is not a packet boundary. Feed the bytes into the USB stream parser and recover complete 49-byte packets using sync + CRC validation.

## 13. UART loses synchronization after one corrupt byte

Use the repository's COBS + `0x00` delimiter UART framing. Discard the damaged frame at the next delimiter, decode the following COBS frame and then require a valid canonical CRC.

## 14. BLE receiver cannot reconstruct packets

Check:

- the actual application notification payload size passed to the fragmenter;
- 8-byte BLE fragment header;
- packet sequence;
- fragment index/count;
- in-order delivery expected by the simple reassembler;
- final canonical CRC.

Do not confuse negotiated ATT MTU with the application payload bytes actually available to the notification function.

## 15. Two or more ADS1299 devices drift/misalign

Do not fix this by reordering channels in software. Measure:

- device clock strategy;
- START/RESET timing;
- DRDY relationship;
- SPI lane/read scheduling;
- timestamp assignment;
- actual inter-device skew.

For strict synchronization, use a deterministic multi-lane/DMA or FPGA capture architecture.

## 16. The plot looks good but raw validation fails

Trust the raw validation. A filtered/interpolated plot can hide:

- sequence gaps;
- CRC failures;
- duplicated samples;
- clipping;
- wrong scaling;
- incorrect channel order.

Keep raw packets or a lossless raw-count recording for every engineering test.

## 17. The firmware compiles but hardware does nothing

Compilation proves only software syntax/linkage for that target. It does not prove:

- correct board pin mapping;
- correct power/reference design;
- correct SPI timing at the pins;
- correct analog behavior;
- electrical safety;
- EMC;
- long-duration reliability.

Return to the bring-up sequence and verify each observable hardware gate.
