namespace PHsensor {
  void setup();
  int getRaw();
  float getVoltage();
  float voltToPH(float voltage, float temperature);
  float getPH();
}
