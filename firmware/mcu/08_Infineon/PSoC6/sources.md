# Official source audit — 2026-09-06

- [Infineon mtb-pdl-cat1 release-v3.17.0](https://github.com/Infineon/mtb-pdl-cat1/tree/release-v3.17.0),
  `92a009f42d50e75583a2384599bec925c00155ca`: cy_scb_spi.h/.c,
  cy_scb_common.h, cy_gpio.h and exact CY8C624ABZI_S2D44 device headers.
  Checked Init/Disable low-level NULL-context support, nonblocking Write/Read,
  overflow status and IsTxComplete FIFO+shifter semantics.
- [Infineon core-lib release-v1.4.4](https://github.com/Infineon/core-lib/tree/release-v1.4.4),
  `933c475863a75ae24188e93b3602161b11a0301a`: cy_utils/cy_result dependencies.
- [Official CY8CKIT-062S2-43012 BSP](https://github.com/Infineon/TARGET_CY8CKIT-062S2-43012/tree/35f23314696e5de840f21699e090e8a22883886c),
  release notes v5.0.1, `35f23314696e5de840f21699e090e8a22883886c`:
  actual system_psoc6.h, device/core identity and BSP build dependency selection.
  `bsp.mk` selects CM0P_SLEEP; startup/linker resources linked locally,
  but physical dual-core boot remains unverified.
- [Official psoc6cm0p image](https://github.com/Infineon/psoc6cm0p/tree/7c113ccd8b683565f31a3fc1cfffc1dbb384e6cc),
  `7c113ccd8b683565f31a3fc1cfffc1dbb384e6cc`: unchanged
  COMPONENT_CM0P_SLEEP/psoc6_02_cm0p_sleep.c, 6216-byte embedded image.
  **Permissive Binary License v1.0**, not Apache-2.0. Original LICENSE and notices
  retained in the external checkout. Binary is not modified, disassembled,
  relicensed, or copied into shipping source. Redistribution requires compliance
  with its binary notice and applicable SDK DEPENDENCIES requirements.

Selected PDL/BSP/core-library source headers carry Apache-2.0 notices; their
licenses/copyright files remain intact under ignored tests/.tools dependencies.
No vendor implementation is pasted or relicensed into the adapter. Radio firmware
and its additional licensing are outside this SPI work; none is integrated.

The existing board identity is retained. This task has not qualified a GPIO
header assignment. Official HAL/DMA loopback examples are references only, not
ADS1299 projects and not runtime evidence for this implementation.
