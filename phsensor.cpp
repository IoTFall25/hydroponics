#include "temp.h"
#include <Arduino.h>

#define PH_PIN A1
#define NEUTRAL_VOLTAGE 1559.766
#define ACID_VOLTAGE 2028.34

namespace PHsensor {

  void setup() {
    pinMode(PH_PIN, INPUT);
  }

  int getRaw() {
    return analogRead(PH_PIN);
  }
  
  float getVoltage() {
    return (getRaw()/1024.0)*3300;
  }

  float voltToPH(float voltage, float temperature) {
    float slope = (7.0-4.0)/((NEUTRAL_VOLTAGE-1500.0)/3.0 - (ACID_VOLTAGE-1500.0)/3.0);  // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    float intercept =  7.0 - slope*(NEUTRAL_VOLTAGE-1500.0)/3.0;
    return slope*(voltage-1500.0)/3.0+intercept;  //y = k*x + b
  }

  float getPH() {
    return voltToPH(getVoltage(), Temp::getC());
  }

}
