#include <Arduino.h>
#include <HX711.h>
#include <ModbusRTUSlave.h>

// ==========================================
// PIN CONFIGURATION & HARDWARE ASSIGNMENTS
// ==========================================
// HX711 Pins
const uint8_t HX711_DOUT_PIN = 16;
const uint8_t HX711_SCK_PIN  = 4;

// RS485 Transceiver (MAX485) Pins
const uint8_t RS485_RX_PIN   = 18;
const uint8_t RS485_TX_PIN   = 19;
const uint8_t RS485_DE_RE_PIN = 5; // Direction Control Pin

// ==========================================
// MODBUS RTU SLAVE CONFIGURATION
// ==========================================
const uint8_t SLAVE_ID = 1;
ModbusRTUSlave modbus(Serial2, RS485_DE_RE_PIN);

// Holding Registers:
// 0: Live Weight (Integer part in grams)
// 1: Live Weight (Decimal fraction part, 2 decimal places)
// 2: Raw ADC High Word
// 3: Raw ADC Low Word
// 4: Command Register (Write 1 to trigger Tare)
uint16_t holdingRegisters[5];

// ==========================================
// FILTER & CALIBRATION PARAMETERS
// ==========================================
HX711 scale;

// Apne load cell ke mutabiq calibration factor adjust karein
float calibration_factor = 420.0; 

// Moving Average Filter
const int FILTER_SIZE = 10;
float readings[FILTER_SIZE];
int readIndex = 0;
float total = 0;
float filtered_weight = 0;

unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 100; // Har 100ms baad read karein

void setup() {
  Serial.begin(115200);

  // Initialize HX711
  scale.begin(HX711_DOUT_PIN, HX711_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); // Startup par auto-tare

  // Initialize Filter Array
  for (int i = 0; i < FILTER_SIZE; i++) {
    readings[i] = 0.0;
  }

  // Initialize RS485 Serial (Serial2)
  Serial2.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);

  // Configure Modbus
  modbus.configureHoldingRegisters(holdingRegisters, 5);
  modbus.begin(SLAVE_ID, 9600);

  Serial.println("HX711 Modbus Telemetry Node Initialized.");
}

void loop() {
  // Modbus requests handle karein
  modbus.poll();

  // Periodic Sensor Acquisition & Digital Filtering
  if (millis() - lastReadTime >= READ_INTERVAL) {
    lastReadTime = millis();

    if (scale.is_ready()) {
      float raw_units = scale.get_units(1); // 1 sample read
      long raw_adc = scale.read();

      // Moving Average Filter calculation
      total = total - readings[readIndex];
      readings[readIndex] = raw_units;
      total = total + readings[readIndex];
      readIndex = (readIndex + 1) % FILTER_SIZE;
      filtered_weight = total / FILTER_SIZE;

      // Agar weight negative me thoda drift ho toh zero clamp karein
      if (filtered_weight < 0 && filtered_weight > -0.5) {
        filtered_weight = 0.0;
      }

      // Registers update karein
      int16_t int_part = (int16_t)filtered_weight;
      uint16_t frac_part = (uint16_t)(abs(filtered_weight - int_part) * 100);

      holdingRegisters[0] = (uint16_t)int_part;
      holdingRegisters[1] = frac_part;
      holdingRegisters[2] = (uint16_t)((raw_adc >> 16) & 0xFFFF);
      holdingRegisters[3] = (uint16_t)(raw_adc & 0xFFFF);

      // Diagnostic output via USB Serial
      Serial.printf("Raw ADC: %ld | Filtered Weight: %.2f g\n", raw_adc, filtered_weight);
    }
  }

  // Remote Tare Command Check (Register 4)
  if (holdingRegisters[4] == 1) {
    scale.tare();
    total = 0;
    for (int i = 0; i < FILTER_SIZE; i++) readings[i] = 0.0;
    holdingRegisters[4] = 0; // Command reset karein
    Serial.println("Modbus Remote Tare Executed.");
  }
}