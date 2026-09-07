# Capability

| Item | Capability |
|---|---|
| CPU | 80 MHz Cortex-M0+ |
| RAM | 32 KiB SRAM |
| Flash | 128 KiB Flash |
| SPI | SPI1 Mode 1; 4 MHz bring-up |
| DMA | 7-channel DMA; adapter reference is blocking |
| USB | No target USB peripheral; XDS110 provides debug/VCOM |
| BLE | No integrated radio |
| Multi-ADS1299 | One device reference; budget RAM and output bandwidth before adding devices |

These are planning limits, not throughput evidence. Bench measurements decide
the final supported ADS1299 count and transport rate.
