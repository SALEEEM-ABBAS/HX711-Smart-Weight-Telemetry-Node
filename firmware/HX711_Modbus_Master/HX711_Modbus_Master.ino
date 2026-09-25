#include <Arduino.h>
#include <ModbusMaster.h>

// ==========================================
// PIN CONFIGURATION & HARDWARE ASSIGNMENTS
// ==========================================
// RS485 Transceiver (MAX485) Pins
const uint8_t RS485_RX_PIN    = 16;
const uint8_t RS485_TX_PIN    = 17;
const uint8_t RS485_DE_RE_PIN = 4; // Flow Control Pin

// ==========================================
// MODBUS RTU MASTER CONFIGURATION
// ==========================================
const uint8_t TARGET_SLAVE_ID = 1;
ModbusMaster node;

// Direction Control Callbacks
void preTransmission() {
  digitalWrite(RS485_DE_RE_PIN, HIGH); // Transmit Mode
}

void postTransmission() {
  digitalWrite(RS485_DE_RE_PIN, LOW);  // Receive Mode
}

unsigned long lastPollTime = 0;
const unsigned long POLL_INTERVAL = 500; // Har 500ms baad data request karein

void setup() {
  Serial.begin(115200);

  // Configure Flow Control Pin
  pinMode(RS485_DE_RE_PIN, OUTPUT);
  digitalWrite(RS485_DE_RE_PIN, LOW); // Default: Receive Mode

  // Initialize Hardware Serial 2 for RS485
  Serial2.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);

  // Configure Modbus Node
  node.begin(TARGET_SLAVE_ID, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("==========================================");
  Serial.println("ESP32 Modbus RTU Master Initialized");
  Serial.println("Polling Slave ID: 1 for HX711 Weight Data");
  Serial.println("==========================================");
}

void loop() {
  if (millis() - lastPollTime >= POLL_INTERVAL) {
    lastPollTime = millis();

    // Slave ID 1 ke holding registers 0 se lekar 4 tak (5 registers) read karein
    uint8_t result = node.readHoldingRegisters(0, 5);

    if (result == node.ku8MBSuccess) {
      // Data unpack karein
      int16_t weight_int   = (int16_t)node.getResponseBuffer(0);
      uint16_t weight_frac = node.getResponseBuffer(1);
      uint16_t adc_high    = node.getResponseBuffer(2);
      uint16_t adc_low     = node.getResponseBuffer(3);

      // Raw ADC aur Final Float Weight construct karein
      int32_t raw_adc = ((int32_t)adc_high << 16) | adc_low;
      float final_weight = (float)weight_int + ((float)weight_frac / 100.0);

      // Telemetry Output
      Serial.printf("[TELEMETRY] Weight: %8.2f g | Raw ADC: %10ld | Status: OK\n", 
                    final_weight, raw_adc);
    } else {
      // Communication Error Diagnostics
      Serial.printf("[ERROR] Modbus Read Failed! Code: 0x%02X\n", result);
    }
  }

  // Serial input se remote tare trigger karne ki capability ('t' press karein)
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 't' || cmd == 'T') {
      Serial.println("[COMMAND] Sending Remote Tare Signal to Slave...");
      // Register 4 par value 1 write karein
      uint8_t writeResult = node.writeSingleRegister(4, 1);
      if (writeResult == node.ku8MBSuccess) {
        Serial.println("[COMMAND] Tare Signal Successfully Acknowledged.");
      } else {
        Serial.printf("[ERROR] Tare Command Failed! Code: 0x%02X\n", writeResult);
      }
    }
  }
}