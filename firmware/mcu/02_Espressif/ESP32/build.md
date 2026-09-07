# Rank 14 verification in progress — 2026-09-07

Status **Reference**. Actual IDF build is in progress, not yet recorded as passed.
Do not proceed to rank 15 until the software reference gate is complete.

Run from this platform directory:

```powershell
powershell -File tests/build_reference.ps1
```

Prepared local tool cache: PlatformIO Core, Espressif32 6.10.0, ESP-IDF 5.4.0,
Xtensa ESP ELF GCC 14.2.0+20241119. PlatformIO esp32dev selects the generic 4 MB
ESP32 image; board GPIO configuration is explicitly DevKitC V4 WROOM in
board/esp_idf_board.c. No Arduino, PSRAM or radio application is configured.

Build/cache outputs remain in tests/.tools. The wrapper restores environment
variables and uses the local core directory. tests/windows_ldgen.py avoids the
Windows shell's command length limit without modifying upstream tools.

## Executed mock queue tests

tests/run_queue_tests.py --cc <arm-none-eabi-gcc> with Python Unicorn 2.1.4 and
pyelftools 0.33 passed. Uses the existing RA6M5 test Python environment.
Tests execute actual adapter C logic against mock APIs on ARM, not an ESP32/IDF
runtime. Covered nullable/boundary transactions, queue/result failure, wrong
descriptor, late completion with persistent buffers, GPIO failure, invalid pin and
wait values, and rejecting GPIO34 as an output while allowing it as an input.

Remaining: actual bootloader/application build, final source verification and
recorded sizes. Hardware boot, chip ID/readback, internal-test capture, SPI timing
and long-duration loss counters are separate, unverified work.
