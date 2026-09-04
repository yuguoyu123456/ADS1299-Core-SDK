# ADS1299 portable core driver

This directory contains the controller-independent C core for the Texas Instruments ADS1299, ADS1299-6 and ADS1299-4 family. The normative specification is the TI ADS1299-x datasheet SBAS499C (Rev. C). OpenBCI, HackEEG and other public implementations are implementation cross-checks only; they are not specification authorities.

## Register layer

`ads1299_regs.h` defines the complete user-visible address map from `ID` (0x00) through `CONFIG4` (0x17), every SPI command opcode, semantic field masks/codes, fixed datasheet reset values, and explicit constants for encodings that TI marks Reserved or Do not use. `ads1299_register_model.[ch]` is the machine-readable safety model for all 24 addresses: reset value/known state, writable mask, prescribed reserved-one/reserved-zero bits, read-only classification, ADS1299-4/-6/8 availability, and field-semantic legality.

`ads1299_field_model.[ch]` adds a field-level specification layer. Each meaningful datasheet field has a stable field ID and machine-readable name, register location, mask, shift, reset code/known state, writable/read-only state, channel-relative behavior, variant-channel-mask behavior, and legal encoded-value set. Generic helpers resolve `CHnSET` fields to physical channel registers, decode fields, validate codes, encode fields through the register safety model, and safely write them through `ads1299_safe_write_field()`.

`ads1299_semantics.[ch]` adds the physical-meaning layer above encoded fields. A valid field code can be described with a stable short name, concise meaning, quantity class, unit, optional physical value, and explicit missing-context requirements. Clock/reference-dependent values are calculated from caller-supplied context rather than being hard-coded to the nominal device setup. Examples include data rate from `fCLK`, internal test amplitude from `VREFP-VREFN`, test/lead-off frequency from `fCLK` or `fDR`, internal BIASREF from `(AVDD+AVSS)/2`, lead-off threshold/current, and PGA gain. This lets host tools, configuration inspectors and future generated bindings explain what a register configuration physically means without duplicating datasheet formulas.

`ads1299_runtime.[ch]` intentionally exposes two validated whole-register write policies:

- `ads1299_strict_write_register(s)` accepts only bytes that are already exactly TI-valid. It never repairs the caller's request. Wrong reserved bits, unavailable variant bits, read-only registers, or TI do-not-use encodings reject the whole call before SPI I/O.
- `ads1299_safe_write_register(s)` is a normalizing API. It reconstructs the actual WREG byte from the TI model, reports that byte to the caller, and still rejects forbidden semantic encodings. This is useful when deliberate normalization is desired.

The distinction is intentional. For example, SBAS499C section 10.1.2.1 contains an inconsistent DC lead-off pseudo-code value `LOFF=0x13`: register Table 16 requires bit4=0 and defines `FLEAD_OFF=11` as `fDR/4`, whereas DC lead-off uses `FLEAD_OFF=00`. Strict mode rejects `0x13`; normalizing mode produces `0x03` and returns that changed byte, so applications can detect that normalization changed the requested configuration.

Raw RREG/WREG remain available for expert diagnostics and compatibility.

## Functional API layer

The core covers the meaningful programmable functions in the register map: CONFIG1 data rate, clock output and daisy/multiple-readback mode; CONFIG2 external/internal calibration source, amplitude and frequency; CONFIG3 reference/BIAS controls and BIAS sense masks/status; LOFF threshold/current/frequency, sense masks, current flip and comparator enable; every CHnSET field (power, PGA gain, SRB2 and all eight mux selections); SRB1; GPIO data/direction; CONFIG4 continuous/single-shot and lead-off comparator power; plus the full SPI command set.

Runtime APIs separate START-pin control from START/STOP opcodes, support PWDN and DRDY polling, enforce TI's STANDBY rule that only WAKEUP is accepted until exit, and deterministically issue SDATAC before register/RDATA transactions because RDATAC is the power-up default.

Calling `ads1299_read_device_id()` caches the physical 4/6/8-channel count. Channel-oriented high-level APIs reject unavailable channels and mask BIAS/lead-off channel fields to the detected variant. Before ID probing, the API preserves compatibility by exposing the maximum eight-channel surface.

`ads1299_frame.[ch]` provides variant-aware 15/21/27-byte decoding and parses the 24-bit status word (`1100`, LOFF_STATP, LOFF_STATN and GPIO). The older fixed-27-byte acquisition helpers remain for existing eight-channel callers.

## ADC code-to-voltage conventions

SBAS499C Rev. C presents two closely related ways to talk about the signed 24-bit transfer function. Equation 8 defines one quantizer LSB as `(2 × VREF / Gain) / 2^24 = +FS / 2^23`. The maximum positive code is nevertheless `0x7FFFFF` (`2^23-1`), while the negative endpoint is `0x800000` (`-2^23`). Table 9 and TI support examples also commonly normalize the maximum positive code to +FS using `2^23-1`.

The core therefore does not hide this distinction:

- `ads1299_lsb_volts_equation8()` and `ads1299_code_to_volts_equation8()` implement the Equation-8 quantizer-step convention. Under this convention `-8388608` maps exactly to `-FS` and `+8388607` maps to `+FS - 1 LSB`.
- `ads1299_code_to_volts_positive_fs()` maps `+8388607` exactly to `+FS`, matching the historical project behavior and positive-endpoint normalization used in TI Table-9/E2E guidance.
- `ads1299_code_to_volts()` remains a backward-compatible alias of the positive-full-scale convention; existing applications are not silently rescaled.

The constants `ADS1299_ADC_POSITIVE_FULL_SCALE_CODE`, `ADS1299_ADC_NEGATIVE_FULL_SCALE_CODE`, and `ADS1299_ADC_LSB_CODE_SCALE` keep endpoint codes separate from the `2^23` quantizer scale so future code cannot accidentally use one concept for the other.

## Verification strategy

Register-model tests exhaust all `24 registers × 256 byte values × 3 variants = 18,432` register-byte/variant combinations. The invariants require every successful normalization to produce an exact-valid byte, sanitizer idempotence, exact validity to match no-change normalization, and read-only/unavailable registers to remain unwritable. Field-model tests additionally exhaust every representable code of every machine-readable field across ADS1299-4/-6/8 and require valid writable fields to encode/decode round-trip through an exact-valid register byte.

Semantic-value tests independently check TI formulas and nominal values (including 16 kSPS/250 SPS at 2.048 MHz, calibration-signal amplitude/frequency, BIASREF midpoint, all lead-off thresholds/currents/frequencies and PGA gains) and then exhaust every valid field code across all three variants to ensure each code is machine-describable. Tests also prove that missing `fCLK`, `fDR`, reference span or analog-supply context is reported explicitly rather than silently replaced by nominal assumptions.

ADC-conversion tests separately verify sign-extension endpoints, the Equation-8 one-LSB weight, both full-scale conventions, their one-LSB endpoint difference, invalid-parameter handling, and backward compatibility of the historical conversion API.

## Completion criterion

For the **register/API software layer**, “datasheet-complete” means every user-visible register address and meaningful programmable field in SBAS499C is represented by machine-readable register, field and semantic models and reachable through either a named high-level function or the datasheet-safe register/field APIs. Safe APIs must additionally enforce reserved/read-only rules, ADS1299-4/-6/8 differences, and field encodings explicitly prohibited by TI. Convenience APIs may continue to grow after that point without changing the completeness claim.

Passing host unit tests and GitHub CI supports a **CI-verified software** claim only. It does not establish `Bench-tested` or `24h-tested`. Hardware claims require the exact target hardware to be exercised and measured.
