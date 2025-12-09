#include "temp.h"
#include "phsensor.h"
#include "messaging.h"
#include "sensor_readings.pb.h"
#include "ecsensor.h"
#include <TaskScheduler.h>

#define SEND_DURATION 5*TASK_SECOND

ECSensor ecs;
ReadingSlug slug;

void read_and_send();
Scheduler ts;
Task sendtask(SEND_DURATION, TASK_FOREVER, read_and_send, &ts, true);

#define BATTERY_PIN A3

void setup() {
  //Serial.begin(115200);
  //while (!Serial) delay(1);

  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);

  Temp::setup();
  debugLED(5);

  PHsensor::setup();
  debugLED(6);

  Messaging::setup();
  debugLED(5);

  ecs.begin();                

  // Battery check
  slug.has_power = analogRead(BATTERY_PIN) > 400;

  // Pre-define readings
  slug.has_r1 = true;  
  slug.has_r2 = true;
  slug.has_r3 = true;

  slug.r1.type = ReadingType::ReadingType_TEMP;
  slug.r2.type = ReadingType::ReadingType_PH;
  slug.r3.type = ReadingType::ReadingType_EC;

  // Values will be overwritten each send cycle
}

void loop() {
  ts.execute();
}

void read_and_send() {
    // Reset nested reading structures
    slug.r1 = Reading_init_default;
    slug.r2 = Reading_init_default;
    slug.r3 = Reading_init_default;

    /* TEMP */
    slug.r1.type  = ReadingType_TEMP;
    slug.r1.value = Temp::getC();

    /* PH */
    slug.r2.type  = ReadingType_PH;
    slug.r2.value = PHsensor::getPH();

    /* EC */
    if (ecs.read()) {                   // Read EC sensor
        slug.r3.type  = ReadingType_EC;
        slug.r3.value = ecs.ec_uS;      // Conductivity in µS
    } else {
        slug.r3.type  = ReadingType_EC;
        slug.r3.value = -1;             // Error indicator (optional)
    }

    Messaging::send(slug);
    debugLED(5);
}

void debugLED(int pin) {
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
  delay(500);
}
