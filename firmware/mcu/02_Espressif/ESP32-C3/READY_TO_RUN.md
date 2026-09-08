# ESP32-C3 ADS1299 ready-to-run path

Reuse this folder's existing ESP-IDF board/port integration and add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profile layer.

Use SPI master mode 1 and DRDY falling-edge GPIO interrupt. The ISR should only notify a high-priority acquisition task; perform the 27-byte transfer in that task and immediately queue the completed frame.

Validate ID -> internal test -> input short -> EEG profile before Wi-Fi/BLE/application load. Attach sequence numbers to transported samples so wireless loss is detectable.

See `../DIRECT_USE.md` for the full task/queue architecture.
