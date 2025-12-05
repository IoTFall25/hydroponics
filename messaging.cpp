#include "sensor_readings.pb.h"
#include "Adafruit_EEPROM_I2C.h"
#include <RadioLib.h>
#include <Arduino.h>

#define RFM95_CS 16
#define RFM95_INT 21
#define RFM95_RST 17

#define RFM95_FREQ 915.0

#define HEADER_LEN 2
#define EEPROM_ADDR 0x50

namespace Messaging {

  RFM95 radio = new Module(RFM95_CS, RFM95_INT, RFM95_RST);

  // ReadingSlug slug;
  //pb_ostream_t pbout;
  //int numbytes = 0;
  //uint8_t data[256];

  Adafruit_EEPROM_I2C i2ceeprom;

  struct message 
  {
    uint8_t myaddr=96; 
    uint8_t message_id=0;
    uint8_t data[256];
  };

  message tosend;

  // 5 9 6
  void debug(int pin) 
  {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(500);
  }

  void setup() 
  {
    pinMode(13, OUTPUT);
    int status = radio.begin(RFM95_FREQ);
    if (status != 0) debug(13);
    Serial.print("radio status"); Serial.println(status);
    i2ceeprom.begin(0x50);
    tosend.myaddr = i2ceeprom.read(0x00);
  }


  void send(ReadingSlug& slug) 
  {
    pb_ostream_t pbout = pb_ostream_from_buffer(tosend.data, 256);
    Serial.println(slug.r3.value);
    Serial.println(pb_encode(&pbout, ReadingSlug_fields, &slug));
    int status = radio.transmit((uint8_t *)&tosend, pbout.bytes_written+HEADER_LEN);
    Serial.print(pbout.bytes_written);Serial.println(status);
    tosend.message_id++;
  }

}
