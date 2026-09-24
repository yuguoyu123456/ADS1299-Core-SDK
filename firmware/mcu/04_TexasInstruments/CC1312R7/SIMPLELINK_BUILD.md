# CC1312R7 SimpleLink build integration

Status: TEMPLATE / PLANNED. This is an integration recipe, not a build-verified or board-verified claim.

Use an official TI CC1312R7 SimpleLink SDK project generated with SysConfig for the exact board. Keep TI startup, linker and generated ti_drivers_config files in that project.

Add the shared sources from firmware/core_driver/ads1299, the three ads1299_port source files, board/board_ads1299_binding.c, and board/cc1312r7_simplelink_adapter.c. Add exactly one progressive example at a time: probe_id.c, internal_test.c, input_short.c, eeg_250sps.c, then stream_bounded.c.

Add include paths for the shared ADS1299 headers, ads1299_port, board, and the normal TI Drivers/generated configuration paths.

Enable the real adapter by defining CC1312R7_ADS1299_ENABLE_SIMPLELINK_ADAPTER=1 for cc1312r7_simplelink_adapter.c. Without that opt-in the generic binding intentionally fails closed.

In SysConfig create the SPI and GPIO resources. ADS1299 uses SPI Mode 1 (CPOL=0, CPHA=1), MSB first, with software-controlled CS. DRDY is an input; CS, RESET, PWDN and START are outputs. Put the generated resource indices in board/board_ads1299_config.h; do not move ADS1299 register/profile logic out of the shared core. Start at the repository default 1 MHz SPI clock; the current bring-up validator rejects rates above 4 MHz.

Bring-up order is probe ID, internal test, input short, 250-SPS EEG, then bounded streaming. Keep radio/network/console work outside the DRDY-critical acquisition path.

Failure boundaries: incomplete board resources or invalid clock are configuration errors; SPI open/init failures are SDK binding errors; identification failures are probe errors; missing samples are DRDY timeouts; failed sample transfers are frame/transport errors.

Keep TEMPLATE / PLANNED until an actual documented TI toolchain build succeeds. Claim BUILD-VERIFIED only with build evidence and BOARD-VERIFIED only after physical-board execution.
