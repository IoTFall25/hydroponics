//Libraries: RadioHead, SnappyProto
#include "sensor_readings.pb.h"
#include <RH_RF95.h>

// -------------------------------
// LoRa Radio Pins for RP2040 Feather RFM
// -------------------------------
#define RF95_CS 16
#define RF95_INT 21
#define RF95_RST 17
#define RF95_FREQ 915.0

RH_RF95 rf95(RF95_CS, RF95_INT);

// -------------------------------
// RP2040 Feather Battery Voltage Sensing
// -------------------------------
#define VBAT_PIN A3                   // RP2040 Feather LiPo sense pin
#define ADC_MAX 4095.0f               // 12-bit ADC
#define ADC_REF 3.3f                  // ADC reference voltage
#define VBAT_DIVIDER 2.0f             // Feather has 2:1 resistor divider

float readBatteryVoltage() {
    uint16_t raw = analogRead(VBAT_PIN);
    float vbat = (raw / ADC_MAX) * ADC_REF * VBAT_DIVIDER;
    return vbat;
}

// USB power ~4.7–5.0V, LiPo ~3.7–4.2V
#define BATTERY_THRESHOLD 4.4f

// -------------------------------
// Radio Message Structure
// -------------------------------
struct message {
  uint8_t myaddr = 128;  // change for your network
  uint8_t message_id = 0;
  uint8_t data[256];
} tosend;

// Protobuf data slug
ReadingSlug slug = ReadingSlug_init_default;

// Stream for encoding protobuf messages
pb_ostream_t pbout;

void setup() {
  Serial.begin(115200);
  delay(200);

  analogReadResolution(12);
  pinMode(VBAT_PIN, INPUT);

  // -------------------------------
  // LoRa Radio Initialization
  // -------------------------------
  pinMode(RF95_RST, OUTPUT);
  digitalWrite(RF95_RST, HIGH);
  delay(100);

  digitalWrite(RF95_RST, LOW);
  delay(10);
  digitalWrite(RF95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);

  Serial.println("LoRa transmitter ready");
}

void loop() {

  // -------------------------------
  // Read Battery Voltage
  // -------------------------------
  float vbat = readBatteryVoltage();
  Serial.print("Battery voltage: ");
  Serial.println(vbat);

  // Set has_power based on voltage
  slug.has_power = (vbat > BATTERY_THRESHOLD);

  // PRINT IT EXACTLY AS REQUESTED
  Serial.print("has_power: ");
  if (slug.has_power) {
    Serial.println("true");
  } else {
    Serial.println("false");
  }

  // -------------------------------
  // Build Protobuf Message
  // -------------------------------
  pbout = pb_ostream_from_buffer(tosend.data, 256);

  slug.r1.type = ReadingType_TEMP;
  slug.r1.value = 1 * random(100);
  slug.has_r1 = true;

  slug.r2.type = ReadingType_EC;
  slug.r2.value = 1 * random(100);
  slug.has_r2 = true;

  slug.r3.type = ReadingType_PH;
  slug.r3.value = 1 * random(100);
  slug.has_r3 = true;

  if (!pb_encode(&pbout, ReadingSlug_fields, &slug)) {
    Serial.println("Protobuf encoding failed!");
    return;
  }

  Serial.print("Encoded bytes: ");
  Serial.println(pbout.bytes_written);

  // -------------------------------
  // Transmit via LoRa
  // -------------------------------
  rf95.send((uint8_t*)&tosend, pbout.bytes_written + 2);
  tosend.message_id++;

  delay(10000);
}


