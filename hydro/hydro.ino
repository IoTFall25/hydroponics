#include <Arduino.h>
#include "DFRobot_ECPRO.h"

// === Pin configuration ===
#define EC_PIN A0          // Analog pin for EC sensor
#define TEMP_PIN A1        // Analog pin for PT1000 temperature sensor

// === Sensor objects ===
DFRobot_ECPRO ecSensor;          // EC probe
DFRobot_ECPRO_PT1000 tempSensor; // Temperature probe

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Starting EC & Temperature Sensor Demo...");

  // If you’ve calibrated before, it will read the stored K value
  float kValue = ecSensor.getCalibration();
  Serial.print("Loaded calibration K: ");
  Serial.println(kValue, 3);

  delay(1000);
}

void loop() {
  // === Read analog values ===
  int rawEC = analogRead(EC_PIN);
  int rawTemp = analogRead(TEMP_PIN);

  // Convert ADC to voltage (0–3.3V range for RP2040)
  float voltageEC = (rawEC / 4095.0) * 3.3;
  float voltageTemp = (rawTemp / 4095.0) * 3.3;

  // === Calculate temperature (°C) ===
  float temperatureC = tempSensor.convVoltagetoTemperature_C(voltageTemp);

  // === Calculate EC (µS/cm), compensated for temperature ===
  float ecValue = ecSensor.getEC_us_cm(voltageEC, temperatureC);

  // === Print results ===
  Serial.print("Temp: ");
  Serial.print(temperatureC, 2);
  Serial.print(" °C\t");

  Serial.print("EC: ");
  Serial.print(ecValue, 2);
  Serial.println(" µS/cm");

  delay(10000);
}
