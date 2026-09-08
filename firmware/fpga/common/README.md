# Vendor-neutral FPGA ADS1299 capture path

This directory contains pure Verilog building blocks that can be reused by every FPGA reference project in this repository.

## Intended architecture

```text
ADS1299 DRDY/DOUT
       |
       v
ads1299_capture_core
       | 216-bit frame + frame_valid
       +-----------------------> ads1299_frame_unpack
       |
       +-----------------------> ads1299_frame_to_bytes
                                  | 8-bit ready/valid
                                  v
                             FIFO / USB / UART /
                             Ethernet / AXI DMA
```

The ADS1299 must already be configured by an MCU, SoC, soft CPU, or a separate FPGA register-control block. `ads1299_capture_core` is deliberately focused on the deterministic high-rate path after `RDATAC` has been enabled.

## Signals

`ads1299_capture_core` exposes:

- `ads_drdy_n`: ADS1299 DRDY input, active low.
- `ads_miso`: ADS1299 DOUT.
- `ads_cs_n`: generated chip select.
- `ads_sclk`: generated SPI clock.
- `ads_mosi`: held low while capturing RDATAC data.
- `frame_data[215:0]`: exactly 27 bytes, MSB first.
- `frame_valid`: one system-clock pulse when a complete frame is available.
- `missed_drdy`: sticky warning when a new DRDY edge arrives before the previous frame finishes.

The frame layout is:

```text
[215:192] STATUS[23:0]
[191:168] CH1[23:0]
[167:144] CH2[23:0]
[143:120] CH3[23:0]
[119: 96] CH4[23:0]
[ 95: 72] CH5[23:0]
[ 71: 48] CH6[23:0]
[ 47: 24] CH7[23:0]
[ 23:  0] CH8[23:0]
```

All channel values are still raw signed 24-bit two's-complement ADC codes. Sign extension or scaling can be done later in FPGA DSP, MCU firmware, or host software.

## Clock divider

`ads1299_capture_core` toggles SCLK every `CLOCK_DIV` system clock cycles, therefore:

```text
fSCLK = fCLK / (2 * CLOCK_DIV)
```

Example with a 100 MHz FPGA clock and `CLOCK_DIV=10`:

```text
fSCLK = 100 MHz / 20 = 5 MHz
```

Choose a divider that satisfies the ADS1299 SPI timing requirements and your board timing budget. A complete 216-bit frame consumes 216 SPI clock periods, so the transfer must finish before the next DRDY edge.

At 250 SPS this is very relaxed. At higher sample rates or with daisy chains, calculate the minimum SCLK explicitly.

## 8 ADS1299 / 64-channel architecture

For eight independent ADS1299 devices, instantiate eight `ads1299_capture_core` blocks if every device has its own CS/DOUT path. This gives deterministic parallel acquisition and is the easiest FPGA architecture for 64 channels.

```text
ADS0 -> capture0 --\
ADS1 -> capture1 ---\
...                  +--> frame aligner --> FIFO/DMA
ADS7 -> capture7 ---/
```

If the hardware uses ADS1299 daisy-chain mode, one SPI capture engine can be parameterized with a larger `FRAME_BITS` value, for example `216 * 8 = 1728` bits for eight ADS1299-8 devices. In that mode, add a chain-frame splitter after capture.

## Integration recipe

1. Configure ADS1299 registers with the MCU/core driver.
2. Issue `SDATAC` while editing registers.
3. Configure channels, gain, BIAS, sample rate, lead-off, etc.
4. Issue `RDATAC`.
5. Issue `START` or assert START.
6. Assert `enable` on `ads1299_capture_core`.
7. On every `frame_valid`, either unpack the channels or push the frame into `ads1299_frame_to_bytes`.
8. Feed the byte stream into a FIFO or transport block.
9. Treat `missed_drdy` as an acquisition integrity fault.

## Simulation

A portable testbench is provided in `tb/ads1299_capture_core_tb.v`.

Example with Icarus Verilog:

```bash
iverilog -g2012 -o ads1299_tb \
  rtl/ads1299_capture_core.v \
  tb/ads1299_capture_core_tb.v
vvp ads1299_tb
```

Expected output:

```text
PASS: ADS1299 capture core captured 216-bit frame correctly
```

## Vendor projects

Each vendor directory can either copy these files into its project or add this directory as an external source path. Keeping the acquisition core here avoids eight divergent copies of the same ADS1299 SPI logic.
