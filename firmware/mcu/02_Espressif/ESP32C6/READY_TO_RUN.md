# ESP32-C6 ADS1299 ready-to-run path

Reuse this folder's existing ESP-IDF port/board integration and add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge GPIO interrupt. The ISR should notify a high-priority acquisition task; perform the 27-byte ADS1299 transfer in the task and queue completed frames for Wi-Fi/BLE/802.15.4 application processing.

Keep radio/network work independent from ADS1299 timing and attach sample sequence numbers to transported data. Validate ID -> internal test -> input short -> EEG profile first.

See `../DIRECT_USE.md` for the complete workflow.
