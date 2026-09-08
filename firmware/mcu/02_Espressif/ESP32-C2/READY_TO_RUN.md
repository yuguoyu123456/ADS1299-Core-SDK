# ESP32-C2 ADS1299 ready-to-run path

Reuse this folder's existing ESP-IDF board/port integration and add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profile layer.

Configure SPI master mode 1 and a falling-edge DRDY GPIO interrupt. The ISR should notify a high-priority acquisition task; that task performs the exact 27-byte ADS1299 frame transfer and pushes the frame to a queue/ring buffer.

Validate ID -> internal test -> input short -> EEG profile before enabling wireless/application traffic. Start at 250 SPS during bring-up.

See `../DIRECT_USE.md` for the full ESP-IDF task architecture.
