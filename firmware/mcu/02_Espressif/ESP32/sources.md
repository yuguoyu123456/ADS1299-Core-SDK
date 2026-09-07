# Source record — 2026-09-07

[Official ESP32-DevKitC V4 guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)
provides the J2/J3 GPIO mapping. The reference retains Flash and UART pins and
targets the WROOM module without PSRAM. Physical board behavior is unverified.

Actual build headers and driver sources are from installed ESP-IDF 5.4.0,
PlatformIO framework package 3.50400.0. The original queue-lifetime implementation
is maintained in this platform; no Espressif source or ADS1299 chip driver was
copied into it. Vendor packages retain their own component-specific licenses.

Existing tools are referenced through local tests/.tools/pio junctions. The IDF
Python environment is reused from rank 13's local cache; new build/cache data stay
in this platform. Do not recursively package or delete junction targets. Shipping
source includes neither vendor SDKs nor startup/linker resources from test caches.

The Windows ldgen hook is original project tooling, bypassing cmd.exe for one
long command without modifying PlatformIO/IDF files. Test API doubles are explicitly
not official headers and cannot establish real driver timing or binary ABI support.
