# ADS1299 portable core driver

This directory contains the controller-independent C core for the Texas Instruments ADS1299, ADS1299-6 and ADS1299-4 family. The normative device specification is the TI ADS1299-x datasheet SBAS499C (Rev. C). OpenBCI/HackEEG and other public implementations are useful implementation cross-checks, but they are not specification authorities.

## Register completeness

`ads1299_regs.h` defines the complete user-visible register address map from `ID` (0x00) through `CONFIG4` (0x17), command opcodes, field masks/codes, and fixed reset values. `ads1299_register_model.[ch]` is the machine-readable safety model for all 24 addresses: reset value/known state, writable mask, prescribed reserved-one/reserved-zero bits, read-only classification, and ADS1299-4/-6/8 register/channel availability.

The safe mutation API in `ads1299_runtime.[ch]` validates and sanitizes requested writes against that model before SPI is touched. `ads1299_safe_write_registers()` validates an entire consecutive write first, so an invalid/read-only/unavailable register makes the whole operation side-effect-free. `ads1299_safe_update_register_bits()` rejects masks that include reserved or read-only bits.

## Functional API completeness

The public core covers all meaningful writable functions described by the register map: CONFIG1 data rate, clock output and daisy/multiple-readback mode; CONFIG2 external/internal calibration source, amplitude and frequency; CONFIG3 reference/BIAS controls and BIAS sense masks/status; LOFF threshold/current/frequency, sense masks, current flip and comparator enable; every CHnSET field (power, PGA gain, SRB2 and all eight mux selections); SRB1; GPIO data/direction; CONFIG4 continuous/single-shot and lead-off comparator power; raw RREG/WREG plus safe register access; and the full SPI command set.

The runtime layer also separates START-pin control from the START/STOP opcodes, supports PWDN and DRDY polling, enforces the TI STANDBY rule that only WAKEUP is valid until exit, and deterministically establishes command mode with SDATAC before register/RDATA transactions because RDATAC is the power-up default.

Calling `ads1299_read_device_id()` caches the physical 4/6/8-channel count in the handle. Channel-oriented high-level APIs then reject unavailable channels and mask BIAS/lead-off channel bits to the detected device. Before ID probing, the API preserves backward compatibility by assuming the maximum eight-channel surface.

`ads1299_frame.[ch]` provides variant-aware 15/21/27-byte frame decoding and parses the 24-bit status word (`1100`, LOFF_STATP, LOFF_STATN and GPIO). The older fixed-27-byte acquisition helpers remain for existing ADS1299 8-channel callers.

## Validation language

Passing host unit tests and GitHub CI supports a **CI-verified software** claim only. It does not establish `Bench-tested` or `24h-tested`. Hardware timing claims require the exact target board/device to be exercised and measured.
