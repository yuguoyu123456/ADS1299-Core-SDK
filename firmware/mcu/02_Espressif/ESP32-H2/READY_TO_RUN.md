# ESP32-H2 ADS1299 ready-to-run path

Reuse this folder's existing ESP-IDF board/port integration and add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profile layer.

Use SPI master mode 1 and a falling-edge DRDY GPIO interrupt. The ISR should only signal the acquisition task; perform the 27-byte ADS1299 transfer in that task and queue frames for protocol/radio processing.

Validate ID -> internal test -> input short -> EEG profile before enabling application radio traffic. Keep sample sequence numbers because wireless scheduling is independent of ADS1299 timing.

See `../DIRECT_USE.md` for the complete task architecture.
