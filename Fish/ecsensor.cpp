#include "ecsensor.h"

ECSensor::ECSensor(uint8_t addr)
    : address(addr) {}

void ECSensor::begin() {
    Wire.begin();
}

// Reads EC|TDS|SAL|SG via I2C and updates the public variables
bool ECSensor::read() {
    // Send the "R" (read) command
    Wire.beginTransmission(address);
    Wire.write("R");
    Wire.endTransmission();

    delay(600);  // EZO EC requires ~600ms

    // Request up to 32 bytes
    Wire.requestFrom(address, (uint8_t)32, (uint8_t)1);

    if (!Wire.available()) return false;

    uint8_t code = Wire.read(); // response code
    if (code != 1) {
        return false;  // Success == 1
    }

    char buffer[32];
    byte i = 0;

    while (Wire.available()) {
        char c = Wire.read();
        buffer[i++] = c;
        if (c == 0 || i >= 31) break;
    }
    buffer[i] = 0;

    return parseCSV(buffer);
}

// Parse EC,TDS,SAL,SG from CSV string
bool ECSensor::parseCSV(char* data) {
    char* ec = strtok(data, ",");

    if (!ec) return false;

    ec_uS   = atof(ec);

    return true;
}
