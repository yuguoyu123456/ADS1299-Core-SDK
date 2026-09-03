# ADS1299 全终端生态适配

本目录负责把 **ADS1299 采集硬件**连接到更大的终端与操作系统生态，而不是让数据只停留在串口助手或电脑脚本里。

长期目标是形成一条统一的数据链：

```text
ADS1299 / 多片 ADS1299
        ↓
MCU / FPGA
        ↓
统一数据协议
        ↓
手机 / 电脑 / 手表 / 耳机 / 眼镜 / XR / 智能家居 / Linux边缘设备
        ↓
EEG分析 / BCI / 科研 / 教学 / 产品原型
```

## 我想接到什么设备？

| 目标 | 目录 |
|---|---|
| Android、iPhone、iPad、鸿蒙手机/平板 | [`mobile/`](mobile/) |
| Windows、macOS、Linux、ChromeOS | [`desktop_os/`](desktop_os/) |
| Apple Watch、Wear OS、鸿蒙手表、Garmin、Zepp | [`smart_watches/`](smart_watches/) |
| 智能耳机、头戴耳机、耳EEG | [`smart_audio/`](smart_audio/) |
| 智能眼镜、AR、XR | [`smart_glasses_xr/`](smart_glasses_xr/) |
| Home Assistant、openHAB、Node-RED等中控 | [`smart_home_hubs/`](smart_home_hubs/) |
| Linux SBC、OpenWrt、Zephyr、边缘网关 | [`open_edge_systems/`](open_edge_systems/) |
| BLE、USB、TCP、WebSocket、MQTT、LSL、OSC | [`protocols/`](protocols/) |

## 已经开始真正可运行的部分

### 1. 跨平台串口 → WebSocket 网关

目录：[`open_edge_systems/linux_gateway/`](open_edge_systems/linux_gateway/)

它可以在 Windows / macOS / Linux 上运行，把现有 ADS1299-Core 49 字节串口包解析后转换成统一 WebSocket JSON 数据。

没有真实 ADS1299 板子也可以：

```bash
python gateway.py --simulate
```

### 2. 浏览器 / PWA 8通道实时查看器

目录：[`mobile/web_pwa/`](mobile/web_pwa/)

它可以连接上面的网关，实时显示：

- 8通道数据；
- 序列号；
- 时间戳；
- 数据帧计数；
- 丢帧/序列跳变；
- ADS1299 状态字。

这套 Web/PWA 界面可以作为 Android、iPhone/iPad、Windows、macOS、Linux、ChromeOS 以及部分智能显示/XR浏览器的共同参考客户端。

## 为什么先建立统一网关？

如果没有统一协议，每做一个 Android、iOS、手表或眼镜 App，都要重新写一遍串口解析、CRC、丢帧判断和通道定义，长期一定会失控。

本项目采用：

> **一个 ADS1299 数据模型 + 多种可靠传输方式 + 多类终端客户端。**

以后 Android、iOS、Wear OS、watchOS、智能眼镜等只需要适配传输和界面，不需要重新定义 EEG 数据本身。

## 状态标记

- **Planned**：已经定义目标，但还没有可运行实现；
- **Prototype**：已经有代码可以运行，但还没有完成系统性设备验证；
- **Compiles**：干净环境编译通过；
- **Device-tested**：真实设备验证通过；
- **Long-run tested**：完成长时间持续运行验证。

详细状态见 [`SUPPORT_MATRIX.md`](SUPPORT_MATRIX.md)。

我们的目标不是堆几十个空目录，而是逐步让每一个重要终端真正做到：**能运行、能连接、能显示、能记录、能检测错误、能复现。**
