# Reproducible build

Validated on 2026-09-04 with:

- PlatformIO Core 6.1.19
- ST STM32 platform 20.0.0
- STM32CubeF4 framework 1.28.3
- GNU Arm Embedded 7.2.1 supplied by the installed platform
- PlatformIO board definition `disco_f407vg`

```powershell
$env:PLATFORMIO_CORE_DIR = 'E:\PlatformIO'
platformio run -d firmware/mcu/01_STMicroelectronics/STM32F407
```

The successful link used 10,004 bytes of Flash and 132 bytes of static RAM.
Project `main`, MCU Port and STM32Cube adapter compile with `-Werror`. The
unmodified vendor HAL emitted three `unused-parameter` warnings in
`stm32f4xx_hal_flash_ex.c`; these are retained as upstream warnings rather than
being hidden or fixed in copied vendor code.

This is compilation evidence only. No board flashing or ADS1299 hardware test is
claimed.
