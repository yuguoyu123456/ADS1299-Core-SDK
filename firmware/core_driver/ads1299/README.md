# ADS1299 portable core driver

This directory contains the controller-independent C core for the Texas Instruments ADS1299, ADS1299-6 and ADS1299-4 family. The normative specification is the TI ADS1299-x datasheet SBAS499C (Rev. C). OpenBCI, HackEEG and other public implementations are implementation cross-checks only; they are not specification authorities.

## Register layer

`ads1299_regs.h` defines the complete user-visible address map from `ID` (0x00) through `CONFIG4` (0x17), every SPI command opcode, the semantic field masks/codes used by the API, and the fixed datasheet reset values. `ads1299_register_model.[ch]` is the machine-readable safety model for all 24 addresses: reset value/known state, writable mask, prescribed reserved-one/reserved-zero bits, read-only classification, and ADS1299-4/-6/8 availability.

`ads1299_runtime.[ch]` exposes safe single, consecutive and masked register mutation. Safe consecutive writes validate every target before any SPI write occurs; safe masked updates reject masks containing reserved/read-only bits. Raw RREG/WREG remain available for expert diagnostics and compatibility.

## Functional API layer

The core covers the meaningful programmable functions in the register map: CONFIG1 data rate, clock output and daisy/multiple-readback mode; CONFIG2 external/internal calibration source, amplitude and frequency; CONFIG3 reference/BIAS controls and BIAS sense masks/status; LOFF threshold/current/frequency, sense masks, current flip and comparator enable; every CHnSET field (power, PGA gain, SRB2 and all eight mux selections); SRB1; GPIO data/direction; CONFIG4 continuous/single-shot and lead-off comparator power; plus the full SPI command set.

Runtime APIs separate START-pin control from START/STOP opcodes, support PWDN and DRDY polling, enforce TI's STANDBY rule that only WAKEUP is accepted until exit, and deterministically issue SDATAC before register/RDATA transactions because RDATAC is the power-up default.

Calling `ads1299_read_device_id()` caches the physical 4/6/8-channel count. Channel-oriented high-level APIs reject unavailable channels and mask BIAS/lead-off channel fields to the detected variant. Before ID probing, the API preserves compatibility by exposing the maximum eight-channel surface.

`ads1299_frame.[ch]` provides variant-aware 15/21/27-byte decoding and parses the 24-bit status word (`1100`, LOFF_STATP, LOFF_STATN and GPIO). The older fixed-27-byte acquisition helpers remain for existing eight-channel callers.

## Completion criterion

For the **register/API software layer**, “datasheet-complete” means that every user-visible register address and meaningful programmable field in SBAS499C is represented by the register model and is reachable through either a named high-level function or the datasheet-safe register API, with reserved/read-only rules and ADS1299-4/-6/8 differences tested. Convenience APIs may continue to grow after that point without changing the completeness claim.

Passing host unit tests and GitHub CI supports a **CI-verified software** claim only. It does not establish `Bench-tested` or `24h-tested`. Hardware claims require the exact target hardware to be exercised and measured.
