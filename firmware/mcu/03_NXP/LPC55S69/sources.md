# Sources and provenance

Checked 2026-09-07. Original project adapter/board/tests wrap official hardware
APIs and do not copy ADS1299 business logic or replace TI definitions.

- [NXP mcux-sdk](https://github.com/nxp-mcuxpresso/mcux-sdk/tree/MCUX_2.16.000),
  tag MCUX_2.16.000, commit 6f3fd257cdcf978a4d26e7d6e9eed9240037422b.
  Device headers, startup/linker, Flexcomm SPI/GPIO/clock/reset are used unchanged.
  Selected files use BSD-3-Clause; retain notices and check SW-Content-Register.txt
  for any additional SDK components. Do not treat the whole SDK as one license.
- SDK `boards/lpcxpresso55s69/project_template/pin_mux.c` and `.h`: SPI7 function 7
  on PIO0_19/20/21, P17-12/10/14. The I2S signal labels share the Flexcomm pad route;
  the actual peripheral is configured as SPI by this adapter.
- [NXP SPI polling board-to-board guide](https://mcuxpresso.nxp.com/mcuxsdk/24.12.00/html/examples/driver_examples/flexcomm/spi/polling_b2b_transfer/master/readme.html):
  independent board SPI7 header reference; its hardware SSEL configuration is
  not used for ADS1299 software CS.
- [TI ADS1299 datasheet](https://www.ti.com/lit/ds/symlink/ads1299.pdf): protocol
  authority, implemented only in the shared Core, not redefined in this adapter.

The ignored tests/.tools/mcux-sdk sparse checkout is a local build dependency,
not a distributed vendor SDK. No third-party source was relabeled as original.
