# Primary references

## ADS1299

Electrical, register and timing behavior must be checked against Texas Instruments' current ADS1299-x datasheet (SBAS499, latest revision available from TI):

- Product page: https://www.ti.com/product/ADS1299
- Datasheet: https://www.ti.com/lit/ds/symlink/ads1299.pdf

Key implementation facts used by this repository include the SPI command set, CPOL=0/CPHA=1 timing, the 24-bit status + 8 x 24-bit sample frame, and the device data-rate range.

## EEG/BCI ecosystems to interoperate with rather than copy

Planned interoperability examples may target mature upstream projects such as MNE-Python, EEGLAB, FieldTrip, MOABB and pyRiemann. Their licenses and APIs must be checked before vendoring any third-party code. Prefer examples that depend on upstream packages instead of copying substantial source code.
