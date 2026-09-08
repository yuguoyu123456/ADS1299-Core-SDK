# Deterministic 64-Channel Mapping

Channel order must be fixed before firmware, host recording and EEG analysis are built around the system.

## Project-default mapping

Use device index first, then ADS1299 channel index:

| Global channel | Device | ADS1299 channel |
|---:|---:|---:|
| 1 | 0 | 1 |
| 2 | 0 | 2 |
| 3 | 0 | 3 |
| 4 | 0 | 4 |
| 5 | 0 | 5 |
| 6 | 0 | 6 |
| 7 | 0 | 7 |
| 8 | 0 | 8 |
| 9 | 1 | 1 |
| 10 | 1 | 2 |
| 11 | 1 | 3 |
| 12 | 1 | 4 |
| 13 | 1 | 5 |
| 14 | 1 | 6 |
| 15 | 1 | 7 |
| 16 | 1 | 8 |
| 17 | 2 | 1 |
| 18 | 2 | 2 |
| 19 | 2 | 3 |
| 20 | 2 | 4 |
| 21 | 2 | 5 |
| 22 | 2 | 6 |
| 23 | 2 | 7 |
| 24 | 2 | 8 |
| 25 | 3 | 1 |
| 26 | 3 | 2 |
| 27 | 3 | 3 |
| 28 | 3 | 4 |
| 29 | 3 | 5 |
| 30 | 3 | 6 |
| 31 | 3 | 7 |
| 32 | 3 | 8 |
| 33 | 4 | 1 |
| 34 | 4 | 2 |
| 35 | 4 | 3 |
| 36 | 4 | 4 |
| 37 | 4 | 5 |
| 38 | 4 | 6 |
| 39 | 4 | 7 |
| 40 | 4 | 8 |
| 41 | 5 | 1 |
| 42 | 5 | 2 |
| 43 | 5 | 3 |
| 44 | 5 | 4 |
| 45 | 5 | 5 |
| 46 | 5 | 6 |
| 47 | 5 | 7 |
| 48 | 5 | 8 |
| 49 | 6 | 1 |
| 50 | 6 | 2 |
| 51 | 6 | 3 |
| 52 | 6 | 4 |
| 53 | 6 | 5 |
| 54 | 6 | 6 |
| 55 | 6 | 7 |
| 56 | 6 | 8 |
| 57 | 7 | 1 |
| 58 | 7 | 2 |
| 59 | 7 | 3 |
| 60 | 7 | 4 |
| 61 | 7 | 5 |
| 62 | 7 | 6 |
| 63 | 7 | 7 |
| 64 | 7 | 8 |

## Why this mapping is useful

It is deterministic and independent of:

- which SPI lane a module uses;
- which device is read first;
- transport packet grouping;
- physical electrode montage.

The physical electrode label should be stored as metadata mapped onto this stable global channel index.

## Canonical packet grouping

Each ADS1299 device produces one 49-byte canonical packet for its eight channels.

A 64-channel sample group therefore contains eight device packets:

- group member 0 → global channels 1-8
- group member 1 → global channels 9-16
- ...
- group member 7 → global channels 57-64

Do not reorder packets based on arrival time. Use the physical/module `device_index`.

## Physical module identity

The backplane should provide a deterministic slot-to-device mapping, for example SLOT0..SLOT7.

If identical removable modules have no unique hardware ID, the slot itself can define the device index. If a module EEPROM/UID is added later, treat it as identity metadata rather than allowing channel order to change unpredictably.

## Electrode montage metadata

Keep montage labels separate from electrical channel indexing.

Example metadata fields:

- global channel number;
- device index;
- ADS1299 channel;
- module/slot serial;
- electrode label;
- reference scheme;
- enabled/disabled status;
- gain/sample rate.

This allows a cap/montage to change without rewriting firmware packet order.

## Validation

Use internal test to prove the mapping:

1. configure a distinctive test/configuration pattern one device or channel at a time;
2. record on the host;
3. confirm that the expected global channel changes;
4. repeat across all 64 channels.

Do this before trusting human EEG channel labels.
