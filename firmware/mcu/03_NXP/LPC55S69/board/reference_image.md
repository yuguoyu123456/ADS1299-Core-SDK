# LPC55S69 Core0 reference binding

Status: Reference. This is a linked integration example, not a hardware-qualified image.
Use LPCXpresso55S69 / LPC55S69JBD100 Core0 with a normal standalone reset image.
The unchanged SDK flash linker places vectors at 0 and reserves Core1 memory;
this example neither boots Core1 nor programs TrustZone/security policy.
Do not use it as a nonsecure child image of a secure loader.

The board binding accepts only FRO12 main-clock selection and AHB divide-by-one;
it fails on other configurations instead of assuming a clock frequency. Flexcomm7
is attached to FRO12, SPI mode 1, 8-bit MSB-first, 1 MHz, GPIO CS, polling only.
The delay callback is a conservative minimum delay at 12 MHz, not an exact-width
timing source. Revalidate it if changing clocks, compiler or execution environment.

## Wiring and interlock

SPI7 signals follow NXP board documentation and SDK pin mux function 7:

| Signal | MCU pin | Board location / confidence |
|---|---|---|
| SCK | PIO0_21 | P17-14, official SDK mapping |
| MOSI / ADS DIN | PIO0_20 | P17-10, official SDK mapping |
| MISO / ADS DOUT | PIO0_19 | P17-12, official SDK mapping |
| CS | PIO1_31 | Provisional GPIO; physical route must be reviewed |
| RESET | PIO0_26 | Provisional GPIO; physical route must be reviewed |
| PWDN | PIO0_27 | Provisional GPIO; physical route must be reviewed |
| START | PIO1_6 | Provisional GPIO; physical route must be reviewed |
| DRDY | PIO1_7 | Provisional input; physical route must be reviewed |

Control pins are software profile choices, not a verified board wiring recipe.
Check their bond-out, header access, onboard loads, trace/debug/boot/mux conflicts
against the exact board revision. Change the profile if needed. Match ADS1299
DVDD and MCU I/O voltage, use a common ground, and never connect 5 V signals.
Do not attach human electrodes to an unreviewed, non-isolated bench setup.

`ads1299_board_wiring_reviewed` starts at zero. Set a breakpoint at `main`, review
and wire the hardware first, then set this debugger variable to 1 before continuing.
Otherwise the example records exit code 1 and sleeps without configuring the port.
This is an intentional fail-closed reference interlock, not a simulated acquisition.
Do not change it merely to obtain a passing test. Reset clears the interlock again.

The binding owns GPIO ports 0/1 initialization and Flexcomm7 exclusively. It is
not safe to insert into an already-running application with other owners; supply
your own binding there. SDK reset/clock initialization still contains hardware
waits without a timeout. Only SPI transfer polling has bounded failure evidence;
do not claim all initialization paths are bounded.

## Boot and output

The external official startup clears BSS with `__STARTUP_CLEAR_BSS` and enters
`ads1299_reference_start` through the SDK's `__START` hook. The repository entry
initializes the real C runtime, calls the minimal example and sleeps on return,
recording `ads1299_example_exit_code`. No placeholder startup/linker is shipped.
The internal ADS1299 test signal is configured only by shared Core APIs.
Inspect `ads1299_latest_frame` / `ads1299_frame_sequence` while halted; this latest
snapshot is not a lossless UART/USB stream. No EEG throughput claim is made.

See [sources](../sources.md) and [build evidence](../build.md).
