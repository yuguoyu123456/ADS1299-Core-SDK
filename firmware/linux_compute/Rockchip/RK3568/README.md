# RK3568 ADS1299 Linux 接入规划

Current status: **Planned**

厂商：Rockchip（瑞芯微）。此目录标识 SoC 系列，不代表某块开发板。
开发板型号、硬件版本、Linux BSP/内核、工具链和排针连接均待选择并核对。
RAM、Flash/eMMC、USB/网络接口以实际板卡资料为准，本目录不填猜测值。

维护范围：复用统一 ADS1299 Core，补充 Linux SPI/GPIO 硬件接口和最小采集示例。
目前只创建规划与元信息，尚无可运行适配层、编译记录或硬件测试。

实施顺序：选定板卡 → 核对官方资料和许可证 → 确认连接与电平 → 实现 Port
→ 编译/异常测试 → 实板 SPI/DRDY 波形验证 → 数据完整性和长时间测试。

详见 [Linux 接入规划](../../integration.md) 和 [官方资料入口](../README.md)。
