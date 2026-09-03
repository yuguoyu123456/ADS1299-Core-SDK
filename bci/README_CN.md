# BCI — 脑机接口入门与参考工作流

这里放置 ADS1299 采集和基础 EEG 分析之后的脑机接口内容。目录按照**BCI 范式优先**组织，把可复用的分类器和数学方法单独放到 `common_methods/`，避免初学者把“P300/SSVEP”和“LDA/黎曼几何”混为一类。

## 我应该点哪个文件夹？

- 第一次接触 BCI → [`00_beginner/`](00_beginner/)
- 看闪烁刺激、识别频率 → [`ssvep/`](ssvep/)
- 想象左右手运动 → [`motor_imagery/`](motor_imagery/)
- 做 P300 / oddball → [`p300/`](p300/)
- 做实时滑动窗和在线识别 → [`online/`](online/)
- 找通用分类器 / 黎曼方法 → [`common_methods/`](common_methods/)

```text
bci/
├── 00_beginner/              # BCI 零基础入口
├── ssvep/                    # CCA / FBCCA 等
├── motor_imagery/            # ERD/ERS / CSP 等
├── p300/                     # ERP / P300 工作流
├── online/                   # 实时缓存、滑动窗、延迟与丢帧
└── common_methods/
    ├── classification/       # LDA 等通用分类方法
    └── riemannian/           # 协方差 / 黎曼几何方法
```

## 当前重点

- **SSVEP**：CCA、FBCCA、谐波与在线识别路线；
- **运动想象**：ERD/ERS、CSP、FBCSP、LDA/SVM、黎曼几何路线；
- **P300**：基线校正、ERP、幅度/潜伏期、xDAWN 与分类路线；
- **在线 BCI**：ring buffer、滑动窗、实时推理、丢包、丢窗和延迟检测；
- **深度学习**：只在有明确可复现价值时加入，而不是为了堆算法名称。

## 验证等级

所有例程都应明确自己达到的最高验证状态：

- **Educational**：教学/方法演示；
- **Dataset-tested**：在明确命名的公开数据集上验证；
- **Hardware-tested**：用 ADS1299-Core 真实采集数据验证；
- **Online-tested**：在真实实时采集与分类闭环中验证。

没有真实证据时不能升级验证等级。

通用 EEG 滤波、PSD、质量控制、ERP 基础和特征提取请先看 [`../eeg_analysis/`](../eeg_analysis/)。

本目录用于科研、教学和工程开发，不等同于临床验证或医疗器械算法软件。
