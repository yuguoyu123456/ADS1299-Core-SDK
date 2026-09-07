# Consuming FSP project configuration

Target: EK-RA6M5, R7FA6M5BH3CFC. FSP API baseline: 6.5.0. Install official
e2 studio 2026-04.2 or newer compatible release; do not put its SDK in this repo.

1. Generate a bare-metal/non-secure or flat project appropriate for the board.
   Keep startup, linker, device packs, CMSIS and generated `ra_cfg/ra_gen` there.
2. Add `g_spi0` using **r_spi**: master, Mode 1, MSB first, 1 MHz. Configure all
   four interrupts at priorities that can run during application polling. Remove
   linked transfer/DTC/DMAC stacks. Never open g_spi0 in hal_entry before binding.
3. The official EK-RA6M5 SPI example identifies P410=MISO, P411=MOSI, P412=SCK
   on its PMOD2 side. These are MCU port identifiers, not connector pin numbers.
   Select the matching SPI channel/mux in the configurator. The instance name
   `g_spi0` does not determine the hardware channel. Do NOT copy the example's
   master/slave loopback connections, 32-bit width or DTC settings.
4. Select five free GPIOs from your exact board revision. Configure CS, RESET,
   PWDN as output-high and START as output-low **in pin_data startup settings**.
   Configure DRDY as input. Leave hardware SSL unconnected/unmuxed. Verify shared
   PMOD/peripheral connections before using them; no control-header mapping is
   claimed verified here.
5. Initialize a free-running hardware timebase independently of SPI. Export
   `uint32_t ads1299_board_now_us(void *user)` with modulo-2^32 microsecond
   semantics. Account for prescaler and counter rollover; a scaled raw counter
   that resets before 2^32 microseconds does not satisfy this contract.
   Export `void ads1299_board_delay_us(void *user, uint32_t us)`, using e.g.
   `R_BSP_SoftwareDelay(us, BSP_DELAY_UNITS_MICROSECONDS)` with valid BSP clocks.
6. In the consuming project's `ads1299_board_config.h`, define
   `ADS1299_BOARD_CS`, `ADS1299_BOARD_RESET`, `ADS1299_BOARD_PWDN`,
   `ADS1299_BOARD_START`, `ADS1299_BOARD_DRDY` as chosen FSP `BSP_IO_PORT_*_PIN_*`
   constants and declare the two timer functions. No defaults are intentionally
   supplied: unverified wiring must not silently compile into firmware.
7. Add `fsp_binding.c`, adapter C, standard Port C and the four Core C files.
   For FSP-generated main, compile `examples/main_ads1299.c` separately with
   `main=ads1299_example_main` (per-file macro only), then call that function from
   hal_entry after timer startup. Do not apply that macro to generated main.c.
   Store its returned error code in a debugger-visible variable and halt on error.

On return after an acquisition error, do not reconnect to a live context or
restart the loop blindly. Reset/reinitialize the board before retrying this
minimal binding. A product application should expose explicit shutdown/recovery.
