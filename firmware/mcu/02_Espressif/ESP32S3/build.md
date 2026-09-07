# Reproducible build

Validated on 2026-09-04 with:

- PlatformIO Core 6.1.19
- PlatformIO Espressif32 platform 6.10.0
- ESP-IDF framework 5.4.0
- Xtensa ESP ELF GCC 14.2.0+20241119
- PlatformIO board definition `esp32-s3-devkitc-1` (N8, no PSRAM)

```powershell
$env:PLATFORMIO_CORE_DIR = 'E:\PlatformIO'
platformio run -d firmware/mcu/02_Espressif/ESP32S3
```

The successful link used 13,312 bytes of the 327,680-byte RAM budget and
248,076 bytes of the 1,048,576-byte application partition. The generated
bootloader and application images completed successfully.

Project `main`, the generic MCU Port and the ESP-IDF HAL binding compile with
`-Werror`. The installed esptool 4.5.1 emitted a Python 3.14 `SyntaxWarning`
about a return in `finally`; it is an upstream host-tool warning and did not
come from repository C code.

This is compilation and linking evidence only. No board was flashed and no
ADS1299 ID, test signal or acquisition frame was observed.
