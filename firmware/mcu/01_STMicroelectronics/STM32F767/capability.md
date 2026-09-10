# Capability

| Item | Current repository capability | Validation state |
|---|---|---|
| Reference board | NUCLEO-F767ZI (MB1137, STM32F767ZIT6) | Configuration documented; board run not recorded |
| Board/config layer | `board/board_config.h` is the single repository-owned starter configuration point | Present |
| SPI | SPI1 starter path, Mode 1, 8-bit, MSB-first, software CS | Source/config present; target build and board behavior not yet recorded |
| GPIO control | CS, DRDY, RESET, PWDN and START routed through the model port | Source present |
| Portable port | STM32F767 adapter can construct the shared `ads1299_port_t` interface | Source present |
| Beginner flow | probe/ID -> internal test -> input short -> 250-SPS gain-24 EEG -> stream -> clean stop | Source present |
| Canonical transport | 49-byte repository packet over USART3/ST-LINK VCP starter path | Source present; sustained board throughput not yet measured |
| Host smoke tests | STM32F767 port linked with shared core in `tests/Makefile.host` | Recipe present; PASS not recorded |
| DMA / sustained acquisition | Architecture guidance: DRDY IRQ/DMA -> bounded queue/ring -> asynchronous transport | Guidance only; DMA/cache path not implemented/verified here |
| Multi-ADS1299 | Shared-core architecture retained; shared SPI + independent CS is the default repository topology | Not board-validated on STM32F767 |
| 8x ADS1299 / 64 channels | Shared-core guidance can be reused | Not timing-, transport-, or board-validated on STM32F767 |

## Important limits

The current blocking HAL example is intentionally a first-bring-up implementation, not proof of sustained performance. High-rate, multi-device and long-run applications require measured buffering/latency behavior and explicit overflow accounting.

On STM32F7, DMA access to cacheable SRAM also requires a deliberate cache-coherency strategy. Source guidance is not equivalent to a validated DMA implementation.

No claim is made here for electrical safety, EMC, production readiness, long-run stability or regulatory compliance.
