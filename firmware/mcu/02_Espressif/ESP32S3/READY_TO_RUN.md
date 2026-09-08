# ESP32-S3 ADS1299 ready-to-run path

This folder already contains a complete ESP-IDF project skeleton. Keep the existing `ads1299_port`, `board`, `components`, `examples`, `main` and integration files, then add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profile layer.

Use SPI master mode 1 and a DRDY falling-edge GPIO ISR that only notifies a high-priority acquisition task. The task performs one exact 27-byte frame read per DRDY and immediately places the result in a ring buffer/queue.

Keep USB, Wi-Fi, BLE and application DSP/AI work below acquisition priority. Preserve sample sequence numbers and count queue overflows.

Validate ID -> internal test -> input short -> EEG profile before enabling network/USB streaming. For 64-channel systems, FPGA-assisted capture plus ESP32-S3 transport is a strong architecture.

See `../DIRECT_USE.md` for the complete flow.
