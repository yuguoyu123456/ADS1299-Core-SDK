# Linux 接入规划

Current status: **Planned**

1. 先选择实际开发板型号与硬件版本，记录官方原理图、系统镜像版本、
   内核、设备树和编译器版本；不同品牌的同 SoC 板不能共用排针号。
2. 核对 ADS1299 DVDD 与排针电平，确定 SPI SCK/MOSI/MISO、CS、DRDY、
   RESET、PWDN、START 的真实连接，排查复用/启动/调试冲突。
3. 规划 Linux SPI 与 GPIO character-device 适配，处理权限、设备发现、
   错误返回、资源释放和设备树配置。未核对设备前不硬编码 spidev 编号。
4. 统一 Core 负责寄存器、命令和帧解析；Linux 层只负责硬件 I/O。
   设计前确认 CS 能覆盖 Core 所需的完整命令事务，不可假设单次 ioctl
   的片选行为适用于全部操作。
5. 采集质量需要实际测量 DRDY 响应延迟、丢帧、SPI 波形和长时间运行。
   普通 Linux 用户态调度不作为无丢帧或硬实时保证。
6. 对严格连续采集，另外评估 MCU/FPGA 采集后经 USB/Ethernet 送入算力板；
   这与算力板直接 SPI 采集是两个独立验证路径，不能互相替代测试证据。

当前没有 Linux Port、可运行示例或实板测试；不得标记 Compiles/Bench-tested。
后续每个具体板卡的代码、文档、测试与忽略的依赖缓存放在相应 SoC 子目录内。
