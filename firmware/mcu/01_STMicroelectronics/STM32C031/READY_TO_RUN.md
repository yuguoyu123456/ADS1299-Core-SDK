# STM32C031 ADS1299 ready-to-run path

Use this folder's existing board/port integration together with `../_shared/stm32_ads1299_ready_app.*` and `firmware/core_driver/ads1299/ads1299_profiles.*`.

Bring-up order: SPI mode 1 -> GPIO CS/RESET/PWDN/START -> DRDY falling-edge interrupt -> `ads1299_init()` -> device ID read -> internal test profile -> input-short profile -> normal EEG profile.

Keep the DRDY ISR short: set a flag/task notification only, then read the 27-byte ADS1299 frame in the acquisition context. Start at 250 SPS and a conservative SPI clock during bring-up.

Do not copy pin numbers from another STM32C031 board; use this board's schematic/CubeMX pin assignment.

See `../DIRECT_USE.md` for the complete application flow and 64-channel architecture guidance.
