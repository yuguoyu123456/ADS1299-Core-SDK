# Cold-start reference image

Target: ATSAME54P20A, SAME54 Xplained Pro. This profile uses the external official
interrupt-loopback example's **clock/NVM/PORT configuration**, but not its SPI
PLIB, NVIC setup, main, interrupts or loopback application.

| Signal | MCU reference pin | Evidence boundary |
|---|---|---|
| SCK | PC05 / SERCOM6 PAD1, mux C | Official pin_configurations.csv |
| DIN | PC04 / SERCOM6 PAD0, mux C | Official pin_configurations.csv |
| DOUT | PC07 / SERCOM6 PAD3, mux C | Official pin_configurations.csv |
| CS | PC06 / GPIO | Original provisional control-pin choice |
| DRDY | PA02 / GPIO input | Original provisional control-pin choice |
| RESET | PB04 / GPIO | Original provisional control-pin choice |
| PWDN | PB05 / GPIO | Original provisional control-pin choice |
| START | PB06 / GPIO | Original provisional control-pin choice |

Control GPIOs must be checked against the exact board revision, bonded package,
header routing, attached modules and onboard peripherals. No connector pin
numbers or physical compatibility are claimed verified. Use compatible 3.3 V
digital supplies and common digital ground. Remove the SPI loopback jumper.

The selected clock configuration supplies CPU 120 MHz and SERCOM6 60 MHz.
Requested SCK is 1 MHz (BAUD=29). Do not use these constants with another clock
profile. GPIO output latches are set before output direction; CS/RESET/PWDN high,
START low. DRDY is physical active-low. No MCU-generated ADS1299 CLK is supplied:
configure the converter's supported clock source in the hardware design.

Delay uses DWT cycle counting, checks initial counter progress and includes a
stall failsafe. Do not change CPU clock or sleep during acquisition. Debug halt
pauses the counter and is not elapsed wall time. This is a cold-reset reference,
not a bootloader/RTOS/shared-bus integration. No flash operation has been run.

The external Apache-2.0 GCC startup initializes C data/BSS and vectors, but does
not run C++ constructors. The current example is C only. No fake replacement
for the incompatible XC32 data-initialization function is provided.
