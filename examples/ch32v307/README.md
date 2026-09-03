# CH32V307 WCH peripheral-library adapter — source added, not yet compiled

The adapter follows WCH's CH32V30x peripheral API (`SPI_I2S_GetFlagStatus`, `SPI_I2S_SendData`, `SPI_I2S_ReceiveData`, GPIO read/write).

Configure SPI master mode for ADS1299 serial timing (CPOL=0, CPHA=1), 8-bit, MSB first, with manual GPIO CS. Supply a microsecond delay callback.

First build target: CH32V307 using the official WCH/MounRiver toolchain. Hardware status must remain separate from compilation status.
