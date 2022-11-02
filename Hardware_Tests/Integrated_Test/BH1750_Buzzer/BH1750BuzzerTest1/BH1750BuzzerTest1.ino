/*
Connection:
BH1750 -   Arduino
------------------
VCC    -   5v
GND    -   GND
SCL    -   SCL(Analog Pin 5)
SDA    -   SDA(Analog Pin 4)
ADD    -   NC (or GND)
*/
#include <Wire.h>
#include <stdlib.h>

static unsigned long LIGHT_STATE_CHANGE_TIME = 10000;

unsigned long lastGoodLighting_Time = millis();
unsigned long lastVeryDarkOrVeryBright_Time = millis();
unsigned long lastDatlaLarge_Time = millis();


int BH1750address = 0x23; // was 0x23
int BH1750address2 = 0x5C;
int lightIntensityReading[2];
int enviromentLightIntensity = 0;
int deltaLightIntensity = 0;
const int buzzer = 9; //buzzer to arduino pin 9
byte buff[2];

bool hasBeenDeltaTooLarge = 0;


void setup()
{
  Serial.begin(9600);
  Wire.begin();
  delay(2000);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
}


void buzzer_constant(){
  // noTone(buzzer);     // Stop sound...
  tone(buzzer, 3000); // Send 1KHz sound signal...
}

void buzzer_mute(){
  noTone(buzzer);
}


void buzzer_periodic(){
  noTone(buzzer);
  tone(buzzer, 3000);
  delay(200);
  noTone(buzzer);
  delay(200);
}


int BH1750_Read(int address)
{
 int i=0;
 Wire.beginTransmission(address);
 Wire.requestFrom(address, 2);
 while(Wire.available())
 {
   buff[i] = Wire.read();
   i++;
 }
 Wire.endTransmission();  
 return i;
}

void BH1750_Init(int address)
{
 Wire.beginTransmission(address);
 Wire.write(0x10);
 Wire.endTransmission();
}


uint16_t BH1750_Main(){
  uint16_t value=0;
  BH1750_Init(BH1750address);
  // delay(200);
  if(2==BH1750_Read(BH1750address))
  {
    value=((buff[0]<<8)|buff[1])/1.2;
    // Serial.print(value);
    lightIntensityReading[0] = value;
  }
  delay(150);
  BH1750_Init(BH1750address2);
  if(2==BH1750_Read(BH1750address2))
  {
    value=((buff[0]<<8)|buff[1])/1.2;
    lightIntensityReading[1] = value;
  }
}


void tooDarkOrTooBright(){
  if(millis() - lastGoodLighting_Time > LIGHT_STATE_CHANGE_TIME){
    lastVeryDarkOrVeryBright_Time = millis();
    Serial.println(" || entered tooDarkOrTooBright state!!");
    buzzer_periodic();
  }else{
    Serial.println(" || entering tooDarkOrTooBright state waiting for threshold time!!");
  }
}


void deltaTooLarge(){
  if(millis() - lastGoodLighting_Time > LIGHT_STATE_CHANGE_TIME){
    lastDatlaLarge_Time = millis();
    Serial.println(" || entered deltaTooLarge state!!");
    buzzer_constant();
  }else{
    Serial.println(" || entering deltaTooLarge state waiting for threshold time!!");
  }
}


void loop()
{
    BH1750_Main();
    Serial.print("BH1750_1=");
    Serial.print(lightIntensityReading[0]);
    Serial.print(" lux");
    Serial.print("; BH1750_2=");
    Serial.print(lightIntensityReading[1]);
    Serial.print(" lux");
    enviromentLightIntensity = (lightIntensityReading[0] + lightIntensityReading[1])/2;
    deltaLightIntensity = abs(lightIntensityReading[0] - lightIntensityReading[1]);
    if(deltaLightIntensity>1000){
      deltaTooLarge();
    }else if(enviromentLightIntensity < 200 || enviromentLightIntensity > 1000 ){
      tooDarkOrTooBright();
    }else{
      Serial.println(" || Light situation GOOD!!");
      buzzer_mute();
      lastGoodLighting_Time = millis();
    }
    


    
}
