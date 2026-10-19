# 第三方参考代码来源声明

本仓库**未包含**以下第三方代码，此处仅作来源记录，供学习溯源之用。

## 1. 送药小车参考实现（2021 国赛 F 题）

- **来源**：他人分享的参考代码包（含 `main.cpp`、`Control.cpp`、`drv_huidu.cpp`、`drv_Uart1.cpp`、`drv_Uart2.cpp`、`medicine_car.py`）
- **原作者说明**：该代码包的指南文件明确写有"**所有代码仅供参考思路**"
- **涉及技术**：8 路灰度传感器循迹、PID 环、K210 + KPU 跑 YOLOv5 `.kmodel` 做数字识别、ESP32 做车际通信
- **本仓库处理**：❌ 未收录。该代码非本人所写，不作为个人作品展示

## 2. 二自由度云台激光笔绘图例程

- **来源**：基于 `MiniBalance`（平衡小车之家开源框架）的示例工程
- **本仓库处理**：❌ 未收录（目录名即为"例程"，属示例代码）

## 3. STM32 标准外设库 / HAL 库 / CMSIS

- **来源**：STMicroelectronics 官方
- **本仓库处理**：❌ 未收录（已在 `.gitignore` 中排除 `Library/`、`Start/`、`STM32F10x_FWLib/`）。使用前请自行从 ST 官网或开发板资料补齐

## 4. 厂商资料包

- 正点原子 ATK-MS6DSV 姿态解算模块资料、4 路电机驱动板厂商包、TB6612FNG 模块资料
- **本仓库处理**：❌ 未收录（第三方版权，含 HAL 源码与 PDF 手册）

## 5. 赛题文件

- 2023 / 2025 年电赛 E 题题目 PDF、参赛报告原件
- **本仓库处理**：❌ 未收录（组委会版权）。报告提取文本见 `../docs/2025-report-raw-text.md`

---

## 关于"学习练习工程"的说明

`embedded-lab-collection/stm32-cube-practice/` 下的 9 个工程（blink / hello / key / motor / motor_2 / motor_3 / PWM_light / serial / CubeMX）是我在备赛期间**自己用 STM32CubeMX 从零配置**的练习序列，保留在此作为学习轨迹记录。其中的 HAL 库与自动生成文件已在收录时剔除，只保留 `main.c` 与 `.ioc`。
