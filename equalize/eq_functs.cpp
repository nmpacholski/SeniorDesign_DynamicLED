#include <Wire.h>

TwoWire I2C_32 = TwoWire(0);

void setup() {
  I2C_32.begin(13, 14, 100000);
}

void loop() {
  rec_data = I2C_32.requestFrom(0x50, 16, true); //add address
  //Serial.print(rec_data);
  if ((bool)rec_data) {
    uint8_t temp_data[rec_data];
    I2C_32.readBytes(temp_data, rec_data);
    log_print_buf(temp_data, rec_data);
  }
}
