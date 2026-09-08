# Pinout and Connector Contract

This document defines what the ADS1299 core-module connector must make unambiguous. It intentionally does not assign fixed physical pin numbers because those belong to the actual schematic/PCB revision.

## Mandatory signal table

Document every production module with a table containing at least:

| Group | Signal | Direction at module | Required | Notes |
|---|---|---:|---:|---|
| SPI | SCLK | input | yes | ADS1299 serial clock |
| SPI | MOSI / DIN | input | yes | host-to-ADS1299 data |
| SPI | MISO / DOUT | output | yes | ADS1299-to-host data |
| SPI | CS | input | yes | one independent CS per module/device |
| Control | DRDY | output | yes | sample-ready timing source |
| Control | RESET | input | yes | deterministic reset path |
| Control | START | input | recommended | explicit synchronized conversion control |
| Control | PWDN | input or local strap | optional | expose only when carrier control is needed |
| Clock | CLK | input/output/local | architecture-dependent | document one direction only for a given board revision |
| Analog | CH1P..CH8P | input | yes | positive differential inputs |
| Analog | CH1N..CH8N | input | yes | negative differential inputs |
| Reference | SRB1/SRB2 | local or exposed | architecture-dependent | document routing/ownership |
| BIAS | BIAS-related nodes | local or exposed | architecture-dependent | do not leave ownership ambiguous |
| GPIO | GPIO1..GPIO4 | optional | no | expose only if used by the product |
| Power | analog rail(s) | input | yes if not locally generated | name voltage/domain clearly |
| Power | digital rail(s) | input | yes if not locally generated | name voltage/domain clearly |
| Ground | AGND/DGND/system return | board-specific | yes | document intended connection strategy |

## Connector rules

- Do not place high-edge-rate clock pins between sensitive analog-input pins when the connector can be arranged otherwise.
- Keep the eight differential channel pairs adjacent and consistently ordered.
- Reserve ground/return pins near groups of high-speed digital signals when the connector architecture permits.
- Use the same channel numbering in schematic, PCB, silkscreen, firmware, host software and electrode map.
- Clearly mark module orientation and pin-1 reference.
- Avoid a connector mapping that can be plugged in reversed without mechanical/keying protection when reverse insertion could damage the board.

## Multi-module rules

For 2/4/8-module systems:

- each ADS1299 must have a uniquely controlled CS unless a different validated architecture is intentionally used;
- shared SCLK/MOSI must be evaluated for fan-out, edge quality and return path;
- MISO/DOUT sharing must guarantee that only the selected device drives the bus;
- START/RESET/clock fan-out must be deterministic;
- DRDY ownership must be documented per module.

## Optional pins

Do not add optional signals merely because the IC exposes them. Every connector signal increases routing, fan-out and integration burden. If a function is fully local to the core module, keep it local unless the carrier needs explicit control or observability.

## Revision control

Each module revision should publish:

- connector name and mating part;
- pin number;
- signal name;
- direction;
- voltage domain;
- default state/strap;
- whether 5-V tolerant (if applicable and verified);
- whether safe to leave unconnected;
- schematic net name;
- firmware logical name.

Never infer these electrical properties from an MCU development board or another ADS1299 module. Verify them from the actual design and datasheets.
