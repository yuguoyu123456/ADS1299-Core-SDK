# ADS1299 Core version

- Core Version: 2.0.0
- Specification baseline: Texas Instruments ADS1299-x SBAS499C (Rev. C)
- Updated: 2026-09-04
- Validation: Compiles (ARM GCC; STM32F407, GD32F450 and CH32V307 reference builds)
- Hardware validation: not claimed

## 2.0.0

- Consolidated ten implementation units into four and the public surface into
  four headers.
- Corrected `FLEAD_OFF=11b` to `fDR/450`.
- Added datasheet-safe WAKEUP and power-up guards.
- Made direct acquisition use ADS1299-4/-6/-8 frame lengths.
- Added configuration snapshot/apply/verify and multi-device daisy decoding.
