# Reproducible build

Validated on 2026-09-04 with:

- PlatformIO Core 6.1.19
- ST STM32 platform 20.0.0
- STM32CubeH7 framework 1.13.0
- GNU Arm Embedded 7.2.1 supplied by the installed platform
- PlatformIO board definition `nucleo_h743zi`

Run from the repository root:

```powershell
$env:PLATFORMIO_CORE_DIR = 'E:\PlatformIO'
platformio run -d firmware/mcu/01_STMicroelectronics/STM32H743
```

The successful clean link used 12,888 bytes of Flash and 184 bytes of static
RAM according to PlatformIO. This is compile evidence only: no board was flashed
and no ADS1299 hardware transaction is claimed.
