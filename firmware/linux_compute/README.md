# Linux 算力板扩展目录

Directory role: **Catalog**

此目录位于 firmware/linux_compute，与 mcu、fpga 并列，属于 Linux 应用处理器/算力板，
不是裸机 MCU Port。它是原有 1–100 序号之外的扩展，不增加已支持平台数量，
也不替换第 70 项 Rockchip_MCU_IoT。现阶段仅建立有内容的规划目录。

## 首批 Rockchip 系列

- [RK3288](Rockchip/RK3288/README.md)：用户指定。
- [RK3588](Rockchip/RK3588/README.md)：用户指定。
- [RK3399](Rockchip/RK3399/README.md)：补充既有 Linux 板卡生态。
- [RK3566](Rockchip/RK3566/README.md)：补充同系列板卡选型。
- [RK3568](Rockchip/RK3568/README.md)：补充同系列板卡选型。
- [RK3576](Rockchip/RK3576/README.md)：补充较新平台选型。

以上是维护候选，不是销量排名或已验证兼容列表。RAM、存储、排针电平、
SPI 路由和内核均取决于具体开发板，不能仅用 SoC 型号确定。

统一 ADS1299 驱动仍使用 firmware/core_driver/ads1299；不复制芯片驱动。
下一步见 [Linux 接入边界](integration.md)。不下载完整 BSP 或系统镜像。
