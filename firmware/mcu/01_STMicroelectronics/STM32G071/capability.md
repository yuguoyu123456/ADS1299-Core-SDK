# Capability

Current repository status: **integration candidate-complete**, not hardware-verified.

| Item | Current capability |
|---|---|
| Reference MCU | STM32G071RBT6 |
| Reference board | NUCLEO-G071RB (MB1360) |
| ADS1299 SPI | SPI1 starter integration, Mode 1, 8-bit, MSB-first, software-controlled CS |
| Board/config layer | `board/board_config.h` present |
| HAL/portable port | SPI, GPIO, DRDY and delay adapter sources present |
| Probe / ID | progressive example present for ADS1299-4/-6/-8 family detection |
| Internal test | progressive example path present |
| Input short | progressive example path present |
| EEG starter profile | 250 SPS, gain 24 path present |
| Raw frame | shared ADS1299 frame contract; ADS1299-8 continuous raw frame is 27 bytes |
| Canonical stream | shared 49-byte packet encoder integrated by example |
| Host transport | USART2/ST-LINK VCP starter path at 460800 baud |
| Host tests | model integration test sources and Makefile recipe present; execution PASS not yet recorded |
| Sustained acquisition | architecture guidance present; not validated |
| DMA | not required for blocking starter; sustained DMA/IRQ path not validated |
| Multi-ADS1299 | shared core capability exists; STM32G071 model integration not validated |
| 8x ADS1299 / 64 channels | not validated on STM32G071 |

## Resource policy for this small MCU

Use static bounded storage in timing-critical acquisition code. Keep DRDY/EXTI handling short, queue raw frames, and move packetization/host transport outside the timing path. Record queue overflow explicitly. Avoid heap allocation and floating-point conversion in the DRDY path.

These are repository integration capabilities, not throughput or bench evidence. BUILD-VERIFIED, BOARD-VERIFIED, sustained-acquisition, multi-device and 64-channel claims require separate recorded evidence.
