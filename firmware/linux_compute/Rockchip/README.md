# Rockchip Linux 算力板

Directory role: **Catalog**

当前包含 RK3288、RK3399、RK3566、RK3568、RK3576、RK3588 六个 SoC
候选目录，全部为 Planned。具体板卡尚未确定，不能据此认为 ADS1299 已兼容。

来源核对日期：2026-09-07。

- [Rockchip 官方产品目录](https://www.rock-chips.com/a/en/products/)
- [Rockchip 官方开源文档](https://opensource.rock-chips.com/)
- [Rockchip 官方下载目录](https://www.rock-chips.com/a/en/download/index.html)

这里只记录来源，不复制厂商 BSP、内核、启动代码或系统镜像。
维护时逐项检查所采用组件的许可证并保留原有版权信息。

## Direct-use bring-up path

The directory is still a board catalog, but it now has a runnable ADS1299 userspace bring-up path shared by every listed Rockchip family:

```text
../../linux_compute/common/ads1299_linux_capture.c
../../linux_compute/common/Makefile
```

Each RK model directory contains `DIRECT_USE.md` describing how to connect that board's enabled SPI node and DRDY GPIO line to the common program.

The common program provides:

- SPI mode-1 access through `/dev/spidevB.C`.
- falling-edge DRDY waiting through the Linux GPIO character-device ABI.
- ADS1299 RESET / ID read / register profile setup.
- internal-test, input-short and 250-SPS EEG starting profiles.
- exact 27-byte frame reads.
- signed 24-bit channel decoding.
- raw binary and CSV output.

### Why bus and GPIO numbers are not hard-coded

RK3288/RK3399/RK3566/RK3568/RK3576/RK3588 identify the SoC, not a unique development-board connector. SPI controller selection, chip select and GPIO line offset vary with the board schematic and device tree. The per-model guides therefore require the real `/dev/spidev*` and `/dev/gpiochip*` nodes from the target board instead of publishing guessed pin numbers.

### Recommended validation order

```text
device-tree/pinmux visible
  -> ADS1299 ID read
  -> internal test source
  -> input-short noise
  -> real signal
  -> long-duration frame-loss test
```

For a final 8 x ADS1299 / 64-channel system, use the Linux direct tool for board bring-up and diagnostics; prefer FPGA-aligned acquisition or a dedicated kernel/IIO path when deterministic multi-device synchronization is required.
