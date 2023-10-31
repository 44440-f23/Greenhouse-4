#include <Arduino.h>
#include "Wire.h"
#include <painlessMesh.h>
#include<ArduinoJson.h>
#define addr 0x40
#define Light_Pin A5;

uint8_t readReg(uint8_t reg, const void* pBuf, size_t size);
uint8_t buff[4] = {0};
DynamicJsonDocument doc(1024);

void setup() {
  Serial.begin(9600);
  Serial.println();
  Wire.begin();
  }
  uint16_t data, data1;
  float temperture;
  float humid;


void loop() {
  readReg(0x00, buff, 4);
  data=buff[0]<<8|buff[1];
  data1=buff[2]<<8|buff[3];
  temperture=((float)data*165/65535.0)-40.0;
  humid=((float)data1/65535.0)*100;
    Serial.print("temp(C):");
  Serial.print(temperture);
  Serial.print("\t");
  Serial.print("hum(%RH):");
  Serial.println(humid);
  delay(1000);  

  int light;
  light=analogRead(A5);
  Serial.print(light, DEC);
  delay(100);

  doc["time"]   = 1351824120;
}
  uint8_t readReg(uint8_t reg, const void* pBuf, size_t size)
  {
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  Wire.beginTransmission(addr);
  Wire.write(&reg, 1);
  if ( Wire.endTransmission() != 0) {
    return 0;
  }
  delay(15);
  Wire.requestFrom(addr, (uint8_t) size);
  for (uint16_t i = 0; i < size; i++) {
    _pBuf[i] = Wire.read();
  }
  return size;
}