"""Generate the Planned 101-200 extension without regenerating existing targets.

Numbers are SDK maintenance identifiers, not measured global sales rankings.
Refuses to overwrite a previously created target; later maintenance is manual.
"""
import json
from dataclasses import replace
from pathlib import Path
import maintain_ads1299_ecosystem as base

# Each row is a separate device/subfamily candidate, not a package-size variant.
# Family-wide entries in 1-100 may overlap: explicit relationships are recorded.
GROUPS = [
    ('01_STMicroelectronics', 'STMicroelectronics', 'STM32Cube / official device package',
     'https://www.st.com/en/microcontrollers-microprocessors/STM32-32-bit-arm-cortex-mcus.html',
     '兼顾已有工程迁移、低功耗采集及较新高性能系列，具体供货周期待选型时核实。',
     'STM32F103 STM32F072 STM32F091 STM32F303 STM32F429 STM32F446 STM32F767 STM32H750 STM32H723 STM32H563 STM32H533 STM32N657 STM32U073 STM32C031 STM32G071 STM32L072 STM32L152 STM32L432 STM32L476 STM32L552'),
    ('02_Espressif', 'Espressif', 'ESP-IDF', 'https://www.espressif.com/en/products/socs',
     '补充联网采集和本地处理选型；各型号无线能力不同，不能默认全部带 Wi-Fi。',
     'ESP32-C3 ESP32-C2 ESP32-C5 ESP32-H2 ESP32-P4'),
    ('03_NXP', 'NXP', 'MCUXpresso SDK', 'https://www.nxp.com/',
     '覆盖 MCX 新平台及已有 i.MX RT、LPC、Kinetis 工程迁移候选。',
     'MCXN947 MCXN236 MCXA153 MCXA156 MCXW716 MCXW236 MIMXRT1011 MIMXRT1021 MIMXRT1042 MIMXRT1052 MIMXRT1166 MIMXRT1189 LPC54628 MK64FN1M0 MK66FN2M0'),
    ('04_TexasInstruments', 'Texas Instruments', 'TI device-specific SDK / Code Composer Studio', 'https://www.ti.com/microcontrollers-mcus-processors/microcontrollers/overview.html',
     '补充 MSPM0、SimpleLink 和存量 Tiva 工程；使用对应器件的官方 SDK。',
     'MSPM0L1306 MSPM0G1519 CC2745R10 CC2340R5 CC1312R7 TM4C123GH6PM'),
    ('05_Renesas', 'Renesas', 'FSP for RA; FIT/RX Driver Package for RX', 'https://www.renesas.com/en/products/microcontrollers-microprocessors',
     '覆盖 RA 性能、低功耗梯度及 RX 工程迁移；分别核对 FSP/FIT 支持。',
     'RA8M1 RA8D1 RA8T1 RA4M3 RA4E2 RA4L1 RA2L1 RA2E2 RX660 RX671'),
    ('06_Microchip', 'Microchip', 'MPLAB Harmony / device packs; AVR uses its own toolchain', 'https://www.microchip.com/en-us/products/microcontrollers',
     '补充 SAM、PIC32 和 AVR DB；RAM、编译器及指针模型必须分别评估。',
     'SAMC21 SAMD21 SAML21 SAMG55 SAME70 SAMV71 PIC32MZ_EF PIC32CM_LS PIC32CZ_CA80 AVR128DB48'),
    ('07_Nordic', 'Nordic Semiconductor', 'nRF Connect SDK / supported device SDK', 'https://www.nordicsemi.com/Products',
     '补充短距离无线和蜂窝应用处理器候选；无线发送与采集时序分别验证。',
     'nRF52832 nRF52833 nRF9151'),
    ('08_Infineon', 'Infineon', 'XMC device-supported SDK / ModusToolbox or DAVE as applicable', 'https://www.infineon.com/',
     '补充 XMC 工业控制平台；先验证 SPI 接口和工程环境再扩大采集规模。',
     'XMC4500 XMC4700 XMC7200'),
    ('09_SiliconLabs', 'Silicon Labs', 'Simplicity Studio / device SDK', 'https://www.silabs.com/',
     '补充低功耗和无线系列；避免将同厂商 SDK 版本兼容性直接等同。',
     'EFR32BG22 EFR32MG26 EFM32GG11'),
    ('11_GigaDevice', 'GigaDevice', 'GD32 device firmware library / wireless SDK', 'https://www.gigadevice.com/product/mcu',
     '覆盖国内通用、低功耗及无线选型；不同系列外设寄存器不可互换。',
     'GD32F303 GD32F350 GD32E230 GD32L233 GD32VW553'),
    ('12_WCH', 'WCH', 'WCH official EVT / MounRiver toolchain', 'https://www.wch-ic.com/products/productsCenter/mcuInterface?categoryId=70',
     '补充低成本、无线和高速接口候选；低资源器件先做 Core 容量评估。',
     'CH32V003 CH32V208 CH32V317'),
    ('13_Artery', 'Artery', 'AT32 device firmware library', 'https://www.arterychip.com/',
     '补充国内 AT32 产品梯度；按具体器件核对 SPI/DMA 与开发板。',
     'AT32F403A AT32F421 AT32F423'),
    ('14_Geehy', 'Geehy', 'APM32 device SDK', 'https://www.geehy.com/',
     '补充已有工程迁移候选；不能据命名推定与其他厂商二进制兼容。',
     'APM32F072 APM32F103'),
    ('15_HDSC', 'HDSC', 'HC32 device DDL', 'https://www.xhsc.com.cn/',
     '补充国内控制器选型；先锁定官方 DDL 版本及封装。',
     'HC32F448 HC32F4A0'),
    ('16_MindMotion', 'MindMotion', 'MM32 device SDK', 'https://www.mindmotion.com.cn/products/',
     '补充国内通用系列；按手册逐项确认时钟、SPI 和 DMA 能力。',
     'MM32F0160 MM32F5270'),
    ('17_Nationstech', 'Nationstech', 'N32 device firmware library', 'https://www.nationstech.com/',
     '补充 N32 低功耗和通用控制候选；具体板卡与供货情况待核对。',
     'N32L406 N32G430'),
    ('18_Nuvoton', 'Nuvoton', 'NuMicro device BSP', 'https://www.nuvoton.com/',
     '补充 NuMicro 入门及低功耗候选；先评估帧缓冲与实际 SPI 时序。',
     'M031 M251'),
    ('19_Puya', 'Puya', 'PY32 device SDK', 'https://www.puyasemi.com/',
     '补充低成本采集控制候选；能否容纳统一 Core 需要实际编译证明。',
     'PY32F030 PY32F072'),
    ('21_Ambiq', 'Ambiq', 'AmbiqSuite SDK', 'https://ambiq.com/product/apollo510/',
     '兼顾较新端侧处理与已有超低功耗生态；供电、电平与 SDK 分别核对。',
     'Apollo510 Apollo2'),
]

def main():
    old=json.loads((base.MCU_ROOT/'catalog.json').read_text(encoding='utf-8'))['projects']
    old_paths={p['path'] for p in old}
    rows=[]
    for folder,vendor,sdk,source,reason,models in GROUPS:
        for model in models.split():
            rows.append((folder,vendor,sdk,source,reason,model))
    assert len(rows)==100, len(rows)
    paths=[f'{row[0]}/{row[5]}' for row in rows]
    assert len(set(paths))==100 and not old_paths.intersection(paths)
    # Preflight the entire batch before writing any targets; preserve maintained files.
    collisions=[p for p in paths if (base.MCU_ROOT/p).exists()]
    if collisions: raise SystemExit('Refusing to overwrite existing targets: '+', '.join(collisions))
    projects=[]
    for rank,(folder,vendor,sdk,source,reason,model) in enumerate(rows,101):
        target=base.ecosystem_target(rank,f'{folder}/{model}',vendor,model,
            'Confirm exact CPU/core variant in official device documentation',sdk,'Planned')
        target=replace(target,spi='Planned: SPI Mode 1, 8-bit MSB-first; frequency requires board validation',
            multi='Not validated; first establish one ADS1299, then measure buffering and timing')
        manifest=base.generate_target(target)
        manifest.update(selection_reason=reason,official_source=source,created='2026-09-08',
                        catalog_scope='extension_101_200',rank_meaning='maintenance_id_not_sales_rank')
        # Generic family entries in 1-100 are retained; these concrete entries refine them.
        parent=None
        if model.startswith('PIC32'): parent=51
        if model=='CC1312R7': parent=25
        if model=='EFM32GG11': parent=64
        if parent: manifest['related_family_rank']=parent
        path=base.MCU_ROOT/target.path
        base.write(path/'project.json',json.dumps(manifest,ensure_ascii=False,indent=2))
        base.write(path/'README.md',base.render_readme(target)+f'''
## 第 {rank} 项：后续开发入口

当前是 **Planned** 目录和通用回调模板，尚未实现 {model} 的官方 SDK 绑定。
编号是项目维护顺序，不是全球销量排名，也不表示未来供货保证。

选型理由：{reason}

先确定完整料号、封装、板卡和官方 SDK，再补充真实 SPI/GPIO/DRDY 适配。
CPU/RAM/Flash/SPI 上限、DMA、USB/BLE 与多 ADS1299 能力均以具体器件为准。
通用 examples/main_ads1299.c 需要板级 board_ads1299_hal，当前不能独立链接运行。
tests/ 是待运行的 Core/接口测试入口，不是该 MCU 编译或硬件测试记录。

[官方资料入口]({source}) · [101–200 总清单](../../ECOSYSTEM_101_200.md)
''')
        base.write(path/'sources.md',f'''# Sources

- Device/family candidate: {model}; vendor: {vendor}.
- [Official discovery entry]({source}). This may be a vendor catalog, not an exact-part datasheet.
- Selection recorded: 2026-09-08. Full ordering code, package, board revision,
  datasheet revision, errata, SDK version and source license remain to be verified.
- No vendor source downloaded for this scaffold. Record exact references and
  retain copyrights when implementing the hardware adapter.
- ADS1299 protocol authority remains the TI datasheet and shared Core.
''')
        base.write(path/'version.md',f'''# Version

- Scaffold created: 2026-09-08
- Hardware Port release: none (Planned)
- Intended shared Core: ADS1299 Core >= 2.0; target-toolchain compatibility unverified
- SDK/device package: {sdk}; exact version pending
- Maintenance ID: {rank}
''')
        base.write(path/'validation.md',f'''# Validation

Current status: **Planned**

Only a directory scaffold and generic callback contract exist for {model}.
No target SDK adapter, clean target compile/link, hardware test or 24h test is claimed.
The example requires a real board binding. Generic test source existence is not a test pass.

Next evidence: exact device/board/SDK -> SPI/GPIO binding -> full target link ->
ADS1299 ID/internal-test capture -> sustained acquisition and loss accounting.
''')
        projects.append(manifest)
    base.write(base.MCU_ROOT/'catalog_101_200.json',json.dumps({
        'schema':2,'scope':'extension_101_200','selection_date':'2026-09-08',
        'policy':'Engineering candidates, not market-share rankings. All newly scaffolded targets are Planned.',
        'projects':projects},ensure_ascii=False,indent=2))
    table='\n'.join(f"| {p['rank']} | {p['vendor']} | [{p['mcu']}]({p['path']}/README.md) | Planned |" for p in projects)
    base.write(base.MCU_ROOT/'ECOSYSTEM_101_200.md','''# MCU 扩展候选：101–200

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
'''+table+'\n')
    print('Created 100 Planned packages, ranks 101-200; existing catalog untouched.')

if __name__=='__main__': main()
