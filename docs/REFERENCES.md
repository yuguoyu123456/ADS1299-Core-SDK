# Primary references

## ADS1299

Electrical, register and timing behavior must be checked against Texas Instruments' current ADS1299-x datasheet (SBAS499, latest revision available from TI):

- Product page: https://www.ti.com/product/ADS1299
- Datasheet: https://www.ti.com/lit/ds/symlink/ads1299.pdf

Key implementation facts used by this repository include the SPI command set, CPOL=0/CPHA=1 timing, the 24-bit status + 8 x 24-bit sample frame, and the device data-rate range.

## WCH CH32V307 / CH32V30x

The CH32V307 reference project uses WCH device materials through the PlatformIO CH32V ecosystem. Keep the MCU vendor layer clearly separated from the repository's ADS1299 application code.

- WCH/openwch CH32V307 SDK, datasheet and hardware materials: https://github.com/openwch/ch32v307
- Community PlatformIO CH32V platform: https://github.com/Community-PIO-CH32V/platform-ch32v
- PlatformIO packaging of the WCH NoneOS SDK: https://github.com/Community-PIO-CH32V/framework-wch-noneos-sdk

The first CH32V307 project pins the PlatformIO platform repository to commit `499971abbee9d191c43132770be02fabfbf572cd` so a future upstream change cannot silently alter the build environment.

WCH framework source carries its own vendor licensing/usage notices. Do not copy large framework source trees into this repository unless there is a clear licensing and maintenance reason; prefer reproducible external dependencies.

## EEG/BCI ecosystems to interoperate with rather than copy

Planned interoperability examples may target mature upstream projects such as MNE-Python, EEGLAB, FieldTrip, MOABB and pyRiemann. Their licenses and APIs must be checked before vendoring any third-party code. Prefer examples that depend on upstream packages instead of copying substantial source code.
