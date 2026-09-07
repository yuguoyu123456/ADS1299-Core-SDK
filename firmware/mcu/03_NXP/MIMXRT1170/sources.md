# Authoritative sources

Checked: 2026-09-05.

Reproducible source for the actual object compilation:
[NXP MCUX_2.16.000](https://github.com/nxp-mcuxpresso/mcux-sdk/tree/6f3fd257cdcf978a4d26e7d6e9eed9240037422b).
The official [LPSPI header](https://github.com/nxp-mcuxpresso/mcux-sdk/blob/6f3fd257cdcf978a4d26e7d6e9eed9240037422b/drivers/lpspi/fsl_lpspi.h)
defines the transfer API used by the adapter. The SDK remains external.

Correction: J9 physical pin numbers for D2-D5 were previously inferred from a
table without verifying the double-row connector. Those physical numbers have
been withdrawn; wiring remains pending board-schematic confirmation.

- NXP i.MX RT1170 product page and datasheet: device identity, architecture,
  clocks, memory and peripheral capabilities.
- NXP MIMXRT1170-EVKB board page and hardware user guide: board device,
  expansion headers, voltage domains, USB/Ethernet and MCU-Link.
- MCUXpresso SDK 25.06.00 EVKB documentation: supported board/core and build.
- `nxp-mcuxpresso/mcuxsdk-examples`, EVKB LPSPI1 examples: GPIO_AD_28 SCK,
  GPIO_AD_30 SOUT, GPIO_AD_31 SIN and GPIO_AD_29 PCS0; J10 routing and required
  zero-ohm resistor changes.

No source code was copied. The adapter is independently structured around the
public MCUXpresso driver API. Vendor SDK and generated files are not included.
# Verification access note (2026-09-05)

The [official EVKB design-files download](https://www.nxp.com/webapp/Download?colCode=MIMXRT1170-EVKB-DESIGNFILES)
redirected to NXP sign-in. The physical D2-D5 header positions are therefore
still unverified. Supply the schematic for the actual board revision before
publishing those positions or selecting a fixed board binding.
