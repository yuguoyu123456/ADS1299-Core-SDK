# BCI 入门与参考算法

这里放置 ADS1299 采集之后的脑机接口算法。目标不是堆砌名称，而是让购买模块的用户可以从“采集到波形”继续走到“特征与分类”。

第一阶段包含：

- **SSVEP**：CCA 基线，后续 FBCCA、谐波 SNR
- **运动想象**：CSP 基线，后续 FBCSP、ERD/ERS、LDA/SVM、黎曼几何
- **P300**：基线校正、ERP 平均、P300 幅度与潜伏期，后续 xDAWN、LDA/SWLDA
- **在线 BCI**：ring buffer、滑动窗、实时推理、丢包与延迟检测

所有例程必须明确状态：Educational / Dataset-tested / Hardware-tested / Online-tested。未经真实硬件与数据集验证的例程不能宣传为“已验证算法”。
