/*#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS A1

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


namespace Temp {
  void setup() {
    pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
    sensors.begin();
  }

  float getC() {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0); 
  }

  float getF() {
    sensors.requestTemperatures();
    return sensors.getTempFByIndex(0); 
  }
}
*/
