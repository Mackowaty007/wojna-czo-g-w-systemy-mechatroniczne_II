#include <Arduino.h>
#include <Wire.h>

//MPU
#define SDA_PIN 8
#define SCL_PIN 9
#define MPU_ADDR 0x68

#define LED_1_PIN 0
#define LED_2_PIN 1
#define BUTTON_PIN 2

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

int16_t read16(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)2);

  if (Wire.available() < 2) return 0;

  uint8_t high = Wire.read();
  uint8_t low = Wire.read();

  return (int16_t)((high << 8) | low);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  delay(100);

  // Wake up the sensor
  writeRegister(0x6B, 0x01);

  // Accelerometer: +/- 8g
  writeRegister(0x1C, 0x10);

  // Gyroscope: +/- 500 deg/s
  writeRegister(0x1B, 0x08);

  // Digital low-pass filter
  writeRegister(0x1A, 0x03);

  pinMode(LED_1_PIN,OUTPUT);
  pinMode(LED_2_PIN,OUTPUT);
  pinMode(BUTTON_PIN,INPUT_PULLDOWN);
}

void loop() {
  // Accelerometer registers
  int16_t ax_raw = read16(0x3B);
  int16_t ay_raw = read16(0x3D);
  int16_t az_raw = read16(0x3F);

  // Temperature
  int16_t temp_raw = read16(0x41);

  // Gyroscope registers
  int16_t gx_raw = read16(0x43);
  int16_t gy_raw = read16(0x45);
  int16_t gz_raw = read16(0x47);

  // Convert to physical units
  float ax = ax_raw / 4096.0;
  float ay = ay_raw / 4096.0;
  float az = az_raw / 4096.0;
  float gx = gx_raw / 65.5;
  float gy = gy_raw / 65.5;
  float gz = gz_raw / 65.5;
  float temperature = temp_raw / 340.0 + 36.53;
  
  Serial.printf(
    "ACC [g]: %.3f %.3f %.3f | "
    "GYRO [deg/s]: %.2f %.2f %.2f | "
    "TEMP: %.2f C\n",
    ax, ay, az,
    gx, gy, gz,
    temperature
  );

    if(digitalRead(BUTTON_PIN) == HIGH){
    digitalWrite(LED_1_PIN,HIGH);
    digitalWrite(LED_2_PIN,LOW);
  }
  else{
    digitalWrite(LED_1_PIN,LOW);
    digitalWrite(LED_2_PIN,HIGH);
  }

  delay(20);
}