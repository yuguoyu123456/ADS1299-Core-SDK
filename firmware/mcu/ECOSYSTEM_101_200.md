# MCU 扩展候选：101–200

这 100 项是面向 ADS1299 后续移植的工程选型清单，结合已有开发生态、
低功耗、接口、国内可选供应商和新平台方向编排。不是全球销量统计，
也不保证未来供货或每款都适合多通道高采样率采集。

全部为 **Planned**。每个目录包含通用 Port 合约、最小示例、元信息、
连接表占位、集成步骤、版本及测试入口；实际 SDK 绑定和验证后续逐项开展。
已有 1–100 中的宽泛系列可能包含这里的具体器件，不能将目录数量当作独立架构数量。
Linux 算力板在 ../linux_compute/；此扩展全部留在 MCU 树内。

官方入口记录在各自 sources.md；精确器件手册和版本在实际移植时锁定。
不要重新运行生成器覆盖已维护平台；工具会拒绝任何已有目标目录。

| 编号 | 厂商 | 器件/子系列 | 状态 |
|---|---|---|---|
| 101 | STMicroelectronics | [STM32F103](01_STMicroelectronics/STM32F103/README.md) | Planned |
| 102 | STMicroelectronics | [STM32F072](01_STMicroelectronics/STM32F072/README.md) | Planned |
| 103 | STMicroelectronics | [STM32F091](01_STMicroelectronics/STM32F091/README.md) | Planned |
| 104 | STMicroelectronics | [STM32F303](01_STMicroelectronics/STM32F303/README.md) | Planned |
| 105 | STMicroelectronics | [STM32F429](01_STMicroelectronics/STM32F429/README.md) | Planned |
| 106 | STMicroelectronics | [STM32F446](01_STMicroelectronics/STM32F446/README.md) | Planned |
| 107 | STMicroelectronics | [STM32F767](01_STMicroelectronics/STM32F767/README.md) | Planned |
| 108 | STMicroelectronics | [STM32H750](01_STMicroelectronics/STM32H750/README.md) | Planned |
| 109 | STMicroelectronics | [STM32H723](01_STMicroelectronics/STM32H723/README.md) | Planned |
| 110 | STMicroelectronics | [STM32H563](01_STMicroelectronics/STM32H563/README.md) | Planned |
| 111 | STMicroelectronics | [STM32H533](01_STMicroelectronics/STM32H533/README.md) | Planned |
| 112 | STMicroelectronics | [STM32N657](01_STMicroelectronics/STM32N657/README.md) | Planned |
| 113 | STMicroelectronics | [STM32U073](01_STMicroelectronics/STM32U073/README.md) | Planned |
| 114 | STMicroelectronics | [STM32C031](01_STMicroelectronics/STM32C031/README.md) | Planned |
| 115 | STMicroelectronics | [STM32G071](01_STMicroelectronics/STM32G071/README.md) | Planned |
| 116 | STMicroelectronics | [STM32L072](01_STMicroelectronics/STM32L072/README.md) | Planned |
| 117 | STMicroelectronics | [STM32L152](01_STMicroelectronics/STM32L152/README.md) | Planned |
| 118 | STMicroelectronics | [STM32L432](01_STMicroelectronics/STM32L432/README.md) | Planned |
| 119 | STMicroelectronics | [STM32L476](01_STMicroelectronics/STM32L476/README.md) | Planned |
| 120 | STMicroelectronics | [STM32L552](01_STMicroelectronics/STM32L552/README.md) | Planned |
| 121 | Espressif | [ESP32-C3](02_Espressif/ESP32-C3/README.md) | Planned |
| 122 | Espressif | [ESP32-C2](02_Espressif/ESP32-C2/README.md) | Planned |
| 123 | Espressif | [ESP32-C5](02_Espressif/ESP32-C5/README.md) | Planned |
| 124 | Espressif | [ESP32-H2](02_Espressif/ESP32-H2/README.md) | Planned |
| 125 | Espressif | [ESP32-P4](02_Espressif/ESP32-P4/README.md) | Planned |
| 126 | NXP | [MCXN947](03_NXP/MCXN947/README.md) | Planned |
| 127 | NXP | [MCXN236](03_NXP/MCXN236/README.md) | Planned |
| 128 | NXP | [MCXA153](03_NXP/MCXA153/README.md) | Planned |
| 129 | NXP | [MCXA156](03_NXP/MCXA156/README.md) | Planned |
| 130 | NXP | [MCXW716](03_NXP/MCXW716/README.md) | Planned |
| 131 | NXP | [MCXW236](03_NXP/MCXW236/README.md) | Planned |
| 132 | NXP | [MIMXRT1011](03_NXP/MIMXRT1011/README.md) | Planned |
| 133 | NXP | [MIMXRT1021](03_NXP/MIMXRT1021/README.md) | Planned |
| 134 | NXP | [MIMXRT1042](03_NXP/MIMXRT1042/README.md) | Planned |
| 135 | NXP | [MIMXRT1052](03_NXP/MIMXRT1052/README.md) | Planned |
| 136 | NXP | [MIMXRT1166](03_NXP/MIMXRT1166/README.md) | Planned |
| 137 | NXP | [MIMXRT1189](03_NXP/MIMXRT1189/README.md) | Planned |
| 138 | NXP | [LPC54628](03_NXP/LPC54628/README.md) | Planned |
| 139 | NXP | [MK64FN1M0](03_NXP/MK64FN1M0/README.md) | Planned |
| 140 | NXP | [MK66FN2M0](03_NXP/MK66FN2M0/README.md) | Planned |
| 141 | Texas Instruments | [MSPM0L1306](04_TexasInstruments/MSPM0L1306/README.md) | Planned |
| 142 | Texas Instruments | [MSPM0G1519](04_TexasInstruments/MSPM0G1519/README.md) | Planned |
| 143 | Texas Instruments | [CC2745R10](04_TexasInstruments/CC2745R10/README.md) | Planned |
| 144 | Texas Instruments | [CC2340R5](04_TexasInstruments/CC2340R5/README.md) | Planned |
| 145 | Texas Instruments | [CC1312R7](04_TexasInstruments/CC1312R7/README.md) | Planned |
| 146 | Texas Instruments | [TM4C123GH6PM](04_TexasInstruments/TM4C123GH6PM/README.md) | Planned |
| 147 | Renesas | [RA8M1](05_Renesas/RA8M1/README.md) | Planned |
| 148 | Renesas | [RA8D1](05_Renesas/RA8D1/README.md) | Planned |
| 149 | Renesas | [RA8T1](05_Renesas/RA8T1/README.md) | Planned |
| 150 | Renesas | [RA4M3](05_Renesas/RA4M3/README.md) | Planned |
| 151 | Renesas | [RA4E2](05_Renesas/RA4E2/README.md) | Planned |
| 152 | Renesas | [RA4L1](05_Renesas/RA4L1/README.md) | Planned |
| 153 | Renesas | [RA2L1](05_Renesas/RA2L1/README.md) | Planned |
| 154 | Renesas | [RA2E2](05_Renesas/RA2E2/README.md) | Planned |
| 155 | Renesas | [RX660](05_Renesas/RX660/README.md) | Planned |
| 156 | Renesas | [RX671](05_Renesas/RX671/README.md) | Planned |
| 157 | Microchip | [SAMC21](06_Microchip/SAMC21/README.md) | Planned |
| 158 | Microchip | [SAMD21](06_Microchip/SAMD21/README.md) | Planned |
| 159 | Microchip | [SAML21](06_Microchip/SAML21/README.md) | Planned |
| 160 | Microchip | [SAMG55](06_Microchip/SAMG55/README.md) | Planned |
| 161 | Microchip | [SAME70](06_Microchip/SAME70/README.md) | Planned |
| 162 | Microchip | [SAMV71](06_Microchip/SAMV71/README.md) | Planned |
| 163 | Microchip | [PIC32MZ_EF](06_Microchip/PIC32MZ_EF/README.md) | Planned |
| 164 | Microchip | [PIC32CM_LS](06_Microchip/PIC32CM_LS/README.md) | Planned |
| 165 | Microchip | [PIC32CZ_CA80](06_Microchip/PIC32CZ_CA80/README.md) | Planned |
| 166 | Microchip | [AVR128DB48](06_Microchip/AVR128DB48/README.md) | Planned |
| 167 | Nordic Semiconductor | [nRF52832](07_Nordic/nRF52832/README.md) | Planned |
| 168 | Nordic Semiconductor | [nRF52833](07_Nordic/nRF52833/README.md) | Planned |
| 169 | Nordic Semiconductor | [nRF9151](07_Nordic/nRF9151/README.md) | Planned |
| 170 | Infineon | [XMC4500](08_Infineon/XMC4500/README.md) | Planned |
| 171 | Infineon | [XMC4700](08_Infineon/XMC4700/README.md) | Planned |
| 172 | Infineon | [XMC7200](08_Infineon/XMC7200/README.md) | Planned |
| 173 | Silicon Labs | [EFR32BG22](09_SiliconLabs/EFR32BG22/README.md) | Planned |
| 174 | Silicon Labs | [EFR32MG26](09_SiliconLabs/EFR32MG26/README.md) | Planned |
| 175 | Silicon Labs | [EFM32GG11](09_SiliconLabs/EFM32GG11/README.md) | Planned |
| 176 | GigaDevice | [GD32F303](11_GigaDevice/GD32F303/README.md) | Planned |
| 177 | GigaDevice | [GD32F350](11_GigaDevice/GD32F350/README.md) | Planned |
| 178 | GigaDevice | [GD32E230](11_GigaDevice/GD32E230/README.md) | Planned |
| 179 | GigaDevice | [GD32L233](11_GigaDevice/GD32L233/README.md) | Planned |
| 180 | GigaDevice | [GD32VW553](11_GigaDevice/GD32VW553/README.md) | Planned |
| 181 | WCH | [CH32V003](12_WCH/CH32V003/README.md) | Planned |
| 182 | WCH | [CH32V208](12_WCH/CH32V208/README.md) | Planned |
| 183 | WCH | [CH32V317](12_WCH/CH32V317/README.md) | Planned |
| 184 | Artery | [AT32F403A](13_Artery/AT32F403A/README.md) | Planned |
| 185 | Artery | [AT32F421](13_Artery/AT32F421/README.md) | Planned |
| 186 | Artery | [AT32F423](13_Artery/AT32F423/README.md) | Planned |
| 187 | Geehy | [APM32F072](14_Geehy/APM32F072/README.md) | Planned |
| 188 | Geehy | [APM32F103](14_Geehy/APM32F103/README.md) | Planned |
| 189 | HDSC | [HC32F448](15_HDSC/HC32F448/README.md) | Planned |
| 190 | HDSC | [HC32F4A0](15_HDSC/HC32F4A0/README.md) | Planned |
| 191 | MindMotion | [MM32F0160](16_MindMotion/MM32F0160/README.md) | Planned |
| 192 | MindMotion | [MM32F5270](16_MindMotion/MM32F5270/README.md) | Planned |
| 193 | Nationstech | [N32L406](17_Nationstech/N32L406/README.md) | Planned |
| 194 | Nationstech | [N32G430](17_Nationstech/N32G430/README.md) | Planned |
| 195 | Nuvoton | [M031](18_Nuvoton/M031/README.md) | Planned |
| 196 | Nuvoton | [M251](18_Nuvoton/M251/README.md) | Planned |
| 197 | Puya | [PY32F030](19_Puya/PY32F030/README.md) | Planned |
| 198 | Puya | [PY32F072](19_Puya/PY32F072/README.md) | Planned |
| 199 | Ambiq | [Apollo510](21_Ambiq/Apollo510/README.md) | Planned |
| 200 | Ambiq | [Apollo2](21_Ambiq/Apollo2/README.md) | Planned |
