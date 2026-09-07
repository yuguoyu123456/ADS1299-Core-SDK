# EVK-derived reference profile (not verified EVKB wiring)

The user authorized a similar-board reference on 2026-09-05. This optional
profile uses the NXP-authored Zephyr EVK signal map and official SDK IOMUX
definitions. It does not assert matching EVKB header pin numbers.

| ADS1299 signal | RT1176 pad / GPIO | EVK Arduino signal |
|---|---|---|
| SCK | GPIO_AD_28 / LPSPI1 SCK | D13 |
| DIN | GPIO_AD_30 / LPSPI1 SOUT | D11 |
| DOUT | GPIO_AD_31 / LPSPI1 SIN | D12 |
| CS | GPIO_AD_29 / GPIO9[28] | D10 |
| DRDY | GPIO_DISP_B2_12 / GPIO11[13] | D2 |
| RESET | GPIO_AD_06 / GPIO9[5] | D4 |
| PWDN | GPIO_AD_05 / GPIO9[4] | D5 |
| START | GPIO_AD_07 / GPIO9[6] | D8 |

D3 is deliberately not used: the reference maps it to the user LED. This
profile differs from the proposed EVKB control assignments in pinmap.md.
Do not mix the two profiles. Verify actual board net continuity, conflicts,
voltage and routing before powering or connecting an ADS1299 board. GPIO11
DRDY is polled, not an M7 GPIO interrupt. CLK uses the ADS1299 board's clock;
this profile does not drive it. Shared ground and compatible 3.3 V digital
levels are required; never apply 5 V to MCU I/O.

LPSPI1 runs at a requested 1 MHz, Mode 1, 8-bit, MSB-first with GPIO CS.
Core clock roots are preserved and queried. The reference leaves pad electrical
settings at reset values; validate edge quality and tune them for the real PCB.
Do not use concurrently with display, PWM, network or another pin owner.

Add evk_reference.c only when selecting this profile. The minimal example
publishes the latest real decoded frame and an update sequence in debugger-
visible globals. This is not USB/UART streaming, lossless recording or an EEG
product. No connected hardware or successful sampling is claimed.

Source (signal facts only; no Zephyr implementation copied):
[NXP EVK board description, Apache-2.0](https://github.com/zephyrproject-rtos/zephyr/blob/main/boards/nxp/mimxrt1170_evk/mimxrt1170_evk.dtsi).
SDK headers are supplied by the external NXP installation, not redistributed.
