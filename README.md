\# HX711 Smart Weight Telemetry Node



An industrial-grade digital weighing and telemetry node powered by an ESP32 and an HX711 24-bit differential ADC. This system acquires raw microvolt signals from resistive strain-gauge load cells, applies multi-stage digital filtering to reject environmental vibration and 50/60 Hz line noise, and exposes calibrated weight data over an isolated RS485 Modbus RTU bus.



\---



\## ⚙️ System Specifications

\* \*\*Core Controller:\*\* ESP32 (Dual-core 240 MHz)

\* \*\*Analog Front-End:\*\* HX711 (24-Bit Sigma-Delta ADC, programmable gain 64/128)

\* \*\*Sensor Interface:\*\* 4-Wire / 6-Wire Wheatstone Bridge Load Cells

\* \*\*Communication:\*\* RS485 / Modbus RTU Slave (Function Codes 03, 04, 06, 16)

\* \*\*Data Processing:\*\* Moving average \& exponential smoothing filters

\* \*\*Calibration Support:\*\* Two-point linear calibration stored in non-volatile storage (NVS)



\---



\## 🚀 Key Features

\* \*\*Vibration \& Noise Rejection:\*\* Configurable window-based digital filtering to stabilize measurements in high-vibration industrial settings.

\* \*\*Non-Volatile Calibration:\*\* Offset (tare) and calibration slope factors persist across power cycles without hardcoding.

\* \*\*Dynamic Modbus Telemetry:\*\* Standard 16-bit register mapping for live weight, tare status, calibration commands, and raw ADC counts.



\---



\## 📂 Repository Layout

```text

HX711-Smart-Weight-Telemetry-Node/

├── firmware/         # C/C++ firmware, filter routines \& Modbus handlers

├── hardware/         # Schematics, pinouts \& wiring diagrams

├── docs/             # Modbus register map \& calibration procedures

└── README.md

