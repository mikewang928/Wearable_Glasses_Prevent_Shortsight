#include <Wire.h>
#include <BH1750.h>

int SDA = A4;
int SCL = A5;
BH1750 lightMeter;

void setup(){

  Serial.begin(9600);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  // On esp8266 devices you can select SCL and SDA pins using Wire.begin(D4, D3);
  Wire.begin();

  lightMeter.begin();
  
  Serial.println(F("BH1750 Test"));
}

void loop() {

  if (lightMeter.begin()) {
    Serial.println(F("BH1750 initialised"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }
  float lux = lightMeter.readLightLevel();
  Serial.println("in loop!");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);

}

