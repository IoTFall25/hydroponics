#pragma once
#include <Arduino.h>
#include <Wire.h>

class ECSensor {
public:
    ECSensor(uint8_t addr = 100);

    void begin();
    bool read();   // returns true if new reading was obtained

    float ec_uS = NAN;

private:
    uint8_t address;
    bool parseCSV(char* data);
};
