\# Modbus RTU Register Map: HX711 Weight Telemetry Node



\## 1. Communication Settings

\* \*\*Default Slave ID:\*\* 1

\* \*\*Baud Rate:\*\* 9600 bps (8N1)

\* \*\*Hardware UART:\*\* ESP32 Serial2 (GPIO18 RX, GPIO19 TX, GPIO5 DE/RE)



\---



\## 2. Holding Register Definitions

| Register Offset | Register Address | Name | Data Format | Description |

| :--- | :--- | :--- | :--- | :--- |

| `0` | \*\*40001\*\* | `WEIGHT\_INT` | 16-bit Signed | Whole integer portion of current calibrated weight (grams) |

| `1` | \*\*40002\*\* | `WEIGHT\_FRAC` | 16-bit Unsigned | Decimal portion (0-99 representing 0.00 to 0.99 grams) |

| `2` | \*\*40003\*\* | `RAW\_ADC\_HIGH` | 16-bit Unsigned | Raw 24-bit HX711 counts (High Word / MSB) |

| `3` | \*\*40004\*\* | `RAW\_ADC\_LOW` | 16-bit Unsigned | Raw 24-bit HX711 counts (Low Word / LSB) |

| `4` | \*\*40005\*\* | `CMD\_REG` | 16-bit Unsigned | Control register: Write `1` to execute remote Tare operation |



\---



\## 3. Data Reconstruction Example (C/Python)

```python

\# Modbus master reading register 0 and 1

weight\_int = registers\[0]   # e.g., 250

weight\_frac = registers\[1]  # e.g., 45

total\_weight = weight\_int + (weight\_frac / 100.0)  # Yields 250.45 g

