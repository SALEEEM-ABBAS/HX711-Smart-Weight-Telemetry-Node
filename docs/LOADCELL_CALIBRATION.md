\# HX711 Load Cell Calibration \& Filtering Protocol



\## 1. Hardware Setup

\* \*\*Excitation Voltage:\*\* 5V via onboard regulator

\* \*\*Bridge Interface:\*\*

&#x20; \* `E+` : Red Wire (Excitation +)

&#x20; \* `E-` : Black Wire (Excitation -)

&#x20; \* `A+` : Green Wire (Signal +)

&#x20; \* `A-` : White Wire (Signal -)



\---



\## 2. Two-Point Linear Calibration Procedure

1\. \*\*Zero Point (Tare):\*\*

&#x20;  \* Keep load cell free of mechanical loads.

&#x20;  \* Call `scale.tare()` or write `1` to Modbus register `40005`.

&#x20;  \* Current zero offset is stored into ESP32 NVS memory.

2\. \*\*Span Calibration:\*\*

&#x20;  \* Place an accurate reference weight (e.g., 1000g calibration standard).

&#x20;  \* Read raw counts: `raw\_count = scale.get\_value(10)`.

&#x20;  \* Compute calibration slope:

&#x20;    $$\\text{Calibration Factor} = \\frac{\\text{raw\\\_count}}{\\text{known\\\_weight}}$$

&#x20;  \* Update firmware calibration coefficient.



\---



\## 3. Digital Filtering Implementation

To eliminate mechanical chatter and motor vibration in industrial environments, the node utilizes a circular 10-tap moving average filter alongside noise blanking for zero drift clamping:

\* \*\*Filter Window Size:\*\* 10 samples

\* \*\*Sampling Rate:\*\* 10 Hz (100 ms update period)

\* \*\*Zero Threshold Clamp:\*\* $-0.5\\text{ g} < \\text{Weight} < 0.0\\text{ g} \\rightarrow 0.0\\text{ g}$

