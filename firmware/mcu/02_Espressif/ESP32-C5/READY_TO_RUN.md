# ESP32-C5 ADS1299 ready-to-run path

Reuse this folder's existing ESP-IDF port/board integration and add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profiles.

Configure ADS1299 SPI mode 1 and DRDY falling-edge GPIO interrupt. Keep the ISR minimal, wake a high-priority acquisition task, perform the exact 27-byte frame read there, and push data to a ring buffer for wireless/USB/storage tasks.

Validate ID -> internal test -> input short -> EEG profile before enabling communication load. Preserve frame sequence numbers and queue-overflow statistics.

See `../DIRECT_USE.md` for the complete ESP-IDF workflow.
