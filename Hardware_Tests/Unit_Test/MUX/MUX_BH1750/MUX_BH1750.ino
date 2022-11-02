//    Connection:
//
//    VCC -> 3V3 or 5V
//    GND -> GND
//    SCL -> SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due, on esp8266 free selectable)
//    SDA -> SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due, on esp8266 free selectable)
//    ADD -> (not connected) or GND (I2C Address is 0x23)
//
//    ADD pin is used to set sensor I2C address. If it has voltage greater or equal to
//    0.7VCC voltage (e.g. you've connected it to VCC) the sensor address will be
//    0x5C. In other case (if ADD voltage less than 0.7 * VCC) the sensor address will
//    be 0x23 (by default).

#include <Wire.h>

// Power
#define BH1750_POWER_DOWN 0x00  // No active state
#define BH1750_POWER_ON 0x01  // Waiting for measurement command
#define BH1750_RESET 0x07  // Reset data register value - not accepted in POWER_DOWN mode

// Measurement Mode
#define CONTINUOUS_HIGH_RES_MODE 0x10  // Measurement at 1 lux resolution. Measurement time is approx 120ms
#define CONTINUOUS_HIGH_RES_MODE_2 0x11  // Measurement at 0.5 lux resolution. Measurement time is approx 120ms
#define CONTINUOUS_LOW_RES_MODE 0x13  // Measurement at 4 lux resolution. Measurement time is approx 16ms
#define ONE_TIME_HIGH_RES_MODE 0x20  // Measurement at 1 lux resolution. Measurement time is approx 120ms
#define ONE_TIME_HIGH_RES_MODE_2 0x21  // Measurement at 0.5 lux resolution. Measurement time is approx 120ms
#define ONE_TIME_LOW_RES_MODE 0x23  // Measurement at 4 lux resolution. Measurement time is approx 16ms

// I2C Address
#define BH1750_1_ADDRESS 0x23  // Sensor 1 connected to GND
#define BH1750_2_ADDRESS 0x5C  // Sensor 2 connected to VCC

byte buff[2];

// Definition of Variable
int16_t s_en = 4;
int16_t s0 = 5;
int16_t s1 = 6;
int16_t s2 = 7;

int16_t RawData;
int16_t SensorValue[4];

void setup() {
  Wire.begin();
  Serial.begin(115200); // Baud Rate

  pinMode(s_en, OUTPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);

  digitalWrite(s_en, HIGH);
  digitalWrite(s0, HIGH); // 1
  digitalWrite(s1, HIGH); // 1
  digitalWrite(s2, HIGH); // 1
}


// read mutiplexer
int readMux(int channel) { 
  int controlPin[] = {s0, s1, s2}; 
  int muxChannel[8][3] = { 
    {0,0,0}, //channel 0 
    {1,0,0}, //channel 1 
    {0,1,0}, //channel 2 
    {1,1,0}, //channel 3 
    {0,0,1}, //channel 4 
    {1,0,1}, //channel 5 
    {0,1,1}, //channel 6 
    {1,1,1}, //channel 7  //loop through the 3 Signals 
  };
  for(int i = 0; i < 3; i++){ // Connecting MUX Channel
    digitalWrite(controlPin[i], muxChannel[channel][i]); 
    // Serial.print(muxChannel[channel][i]);
  }   
}


void init_BH1750(int ADDRESS, int MODE){
  //BH1750 Initializing & Reset
  // Serial.println("in init_BH1750");
  Wire.beginTransmission(ADDRESS);
  // Serial.println("begin transmission");
  Wire.write(MODE);  // PWR_MGMT_1 register
  // Serial.println("write mode");
  Wire.endTransmission();

  // Serial.println("end transmission");
}



int BH1750_Read(int address)
{
 int j=0;
 Wire.beginTransmission(address);
 Wire.requestFrom(address, 2);
 while(Wire.available())
 {
   buff[j] = Wire.read();
   j++;
 }
 Wire.endTransmission();  
 return j;
}



void loop() {

  digitalWrite(s_en, LOW); // Enabling the Enable Switch

    readMux(2);
    init_BH1750(0x23, 0x10);
    delay(120);
    if(2==BH1750_Read(BH1750_1_ADDRESS))
      {
        SensorValue[0]=((buff[0]<<8)|buff[1])/1.2;
      }
    delay(20);
  
  Serial.print("Sensor_1 = "); 
  Serial.println(SensorValue[0]);

}







