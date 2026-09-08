# ESP32-P4 ADS1299 ready-to-run path

Reuse this folder's existing ESP-IDF board/port integration and add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge GPIO interrupt. Use the ISR only to notify a high-priority acquisition task; read the exact 27-byte ADS1299 frame in that task and push it to a ring buffer.

Keep high-latency display, USB, image-processing or application workloads below acquisition priority. Validate ID -> internal test -> input short -> EEG profile before enabling those workloads.

For 8 x ADS1299 / 64 channels, consider FPGA-assisted parallel capture and let ESP32-P4 handle control/processing/transport.

See `../DIRECT_USE.md` for the complete workflow.
