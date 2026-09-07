# Sources and implementation provenance — 2026-09-06

- [Official ESP32-C6-DevKitC-1 v1.2 guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html):
  J1/J3 GPIO table and boot strap restrictions. GPIO4/5 removed from control profile.
- [Official IDF SPI master API](https://docs.espressif.com/projects/esp-idf/en/release-v5.5/esp32c6/api-reference/peripherals/spi_master.html):
  queued descriptors remain owned until returned, including after result timeout.
  Build uses installed IDF 5.4.0 headers and implementation, not this newer manual
  as a substitute for actual compilation.
- Local ESP-IDF package 3.50400.0 (5.4.0) and PlatformIO Espressif32 6.10.0 are
  external build dependencies. Original vendor licenses remain in those packages;
  IDF has component-specific licenses. No SDK implementation was copied/relicensed.

Port and board code are original. ADS1299 chip definitions stay in shared Core.
Existing installed tools are referenced through platform-local directory junctions;
new Python dependencies/cache and build outputs are in tests/.tools. Do not recurse
through junctions when packaging or cleaning. Only shipping adapter/build source
belongs in an SDK release, never the test tool cache or vendor package contents.

tests/fixtures intentionally contains minimal mock interfaces, not vendor SDK
headers. Its tests establish C logic behavior only. Actual IDF/C6 compilation is
separate evidence; neither test method establishes physical acquisition.
