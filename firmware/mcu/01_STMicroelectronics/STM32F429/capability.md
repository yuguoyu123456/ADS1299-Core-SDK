# Capability

Reference path: **STM32F429I-DISC1 (MB1075) / STM32F429ZIT6**.

| Item | Capability / current repository use |
|---|---|
| CPU | Arm Cortex-M4 with FPU, up to 180 MHz |
| RAM | STM32F429ZI family: up to 256+4 KB SRAM including 64 KB CCM |
| Flash | STM32F429ZI: 2 MB on the reference device |
| SPI | Up to 6 SPI peripherals on device family; repository starter uses SPI1, Mode 1, 8-bit MSB-first, software CS |
| DMA | 16-stream general-purpose DMA controller; recommended for sustained/high-rate or multi-device acquisition |
| USB | USB OTG FS and HS capability exists on MCU family; not the current beginner streaming path |
| Ethernet | 10/100 Ethernet MAC exists on MCU family; not the current beginner streaming path |
| Starter transport | USART1 PA9/PA10 through the reference-board VCP path when board bridges/routing permit |
| Static starter buffering | `board_config.h` defines a bounded 16-frame starter queue target |
| ADS1299 raw frame | 27 bytes for ADS1299-8 continuous data path |
| Canonical repository packet | 49 bytes with 0xA5 0x5A sync contract where the model example uses canonical streaming |
| Beginner acquisition | Probe/ID, internal-test, input-short, 250-SPS gain-24 EEG, packet stream, clean stop are represented in the progressive example |
| Multi-ADS1299 | Shared-SPI/independent-CS architecture remains the intended default; STM32F429 multi-device hardware throughput is **not yet validated** |

## Integration maturity

- One board/config edit point: **present**
- STM32F429 port/HAL binding: **present**
- Progressive beginner example: **present**
- Host integration smoke-test sources/build recipe: **present**
- Recorded host test PASS: **no**
- Recorded clean STM32CubeIDE build for the progressive path: **no**
- ADS1299 physical-board validation: **no**
- Sustained-loss / 8xADS1299 validation: **no**

These device capabilities are not throughput or bench evidence. Final supported sample rate, ADS1299 count and transport rate require measured buffering, latency and loss accounting on the intended hardware.
