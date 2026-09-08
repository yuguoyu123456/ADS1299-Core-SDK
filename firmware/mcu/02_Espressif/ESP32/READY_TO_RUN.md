# ESP32 ADS1299 ready-to-run path

Reuse this folder's existing ESP-IDF ADS1299 port/board integration and add `../_shared/espidf_ads1299_ready_app.*` plus the core-driver profile layer.

Use SPI master mode 1 and DRDY falling-edge GPIO interrupt. Keep the ISR short and notify a high-priority acquisition task. The acquisition task performs one exact 27-byte frame transfer per DRDY and pushes the frame to a queue/ring buffer.

Wi-Fi/Bluetooth activity can introduce variable latency, so keep networking in lower-priority tasks and preserve sample sequence numbers/overflow counters.

Validate ID -> internal test -> input short -> EEG profile before wireless streaming. See `../DIRECT_USE.md` for the complete flow.
