#include <ArduinoBLE.h>
#include <Average.h>
#include <Wire.h>
#include <stdlib.h>

#define DEBUG true
#define clockCyclesPerMicrosecond() (260L)  //260 is Clock Cycle of LinkIt ONE in MHz
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a) ((a)*clockCyclesPerMicrosecond())



// Reserve space for 10 entries in the average bucket.
// Change the type between < and > to change the entire way the library works.
Average<float> ave(10);

int motorPin = 2;      //motor transistor is connected to pin 3
const int buzzer = 9;  //buzzer to arduino pin 9
const int resistorpin1 = A0;
const int resistorpin2 = A1;

static unsigned long LIGHT_STATE_CHANGE_TIME = 10000;
static unsigned long GOOD_CLOSE_BOUNDRY = 40;
static unsigned long CLOSE_VERYCLOSE_BOUNDRY = 20;
static unsigned long CHANGE_STATE_TIME = 10000;     // 10 secs = 10000 miliseconds
static unsigned long MOTOR_VIB2STATIC_TIME = 5000;  // 5 secs = 5000 miliseconds
// static unsigned long MOTOR_VIB2VIBLOOP_TIME = 600000;  // 10 mins = 600000 miliseconds (FOR THE SAKE OF TESTING)
static unsigned long MOTOR_VIB2VIBLOOP_TIME = 60000;  // 1 min = 60000 miliseconds (FOR THE SAKE OF TESTING)
// static unsigned long MOTOR_ALERT2STATIC_TIME = 120000; // 1 min = 120000 miliseconds
static unsigned long MOTOR_ALERT2STATIC_TIME = 60000;  // 1 min = 6000 miliseconds (FOR THE SAKE OF TESTING)
static unsigned long MOTOR_VIB_TIME = 300;             // 0.1 secs = 100 miliseconds
unsigned long lastGoodLighting_Time = millis();
unsigned long lastVeryDarkOrVeryBright_Time = millis();
unsigned long lastDatlaLarge_Time = millis();
unsigned long lastVeryClose_Time = millis();
unsigned long lastClose_Time = millis();
unsigned long lastCloseLongTime_Time = millis();
unsigned long lastEnteredCloseLongTime_Time = millis();
unsigned long lastGood_Time = millis();
unsigned long enteringClose_Time = millis();
unsigned long OfficalEnterClose_Time = millis();


bool hasBeenDeltaTooLarge = 0;
bool hasBeenVeryClose = 0;
bool hasBeenClose = 0;
bool hasBeenCloseLongTime = 0;

byte buff[2];

int LDR_voltage1 = 0;
int LDR_voltage2 = 0;
int LDR_voltage = 0;
int BH1750address = 0x23;  // was 0x23
int BH1750address2 = 0x5C;
int lightIntensityReading[2];
int enviromentLightIntensity = 0;
int deltaLightIntensity = 0;
long inches = 0;
long inches2 = 0;
long cm_US1 = 0;
long cm_US2 = 0;
long cm_IR = 0;
long dist = 0;


/*
#########################################################################################################
#                                     Bluetooth related parameter                                        #
#########################################################################################################
*/
BLEService nanoService("13012F00-F8C3-4F4A-A8F4-15CD926DA146");  // BLE Service

// BLE Characteristics - custom 128-bit UUID, read and writable by central device
BLEUnsignedCharCharacteristic FinalDistanceCharacteristic("13012F01-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic IRCharacteristic("13012F02-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic Ultrasound_1_Characteristic("13012F03-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic Ultrasound_2_Characteristic("13012F04-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic DisState_Characteristic("13012F05-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic LightBH_1_Characteristic("13012F06-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic LightBH_2_Characteristic("13012F07-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);
BLEUnsignedCharCharacteristic LightState_Characteristic("13012F08-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);



/*
*********************************************************************************************************
*                                         Helper functions                                              *
*********************************************************************************************************
*/

  //non-blocking pulseIn()
unsigned long pulseIn2(uint8_t pin, uint8_t state, unsigned long timeout) {
  unsigned long width = 0;  // keep initialization out of time critical area

  // convert the timeout from microseconds to a number of times through
  // the initial loop; it takes 16 clock cycles per iteration.
  unsigned long numloops = 0;
  unsigned long maxloops = microsecondsToClockCycles(timeout) / 16;

  // wait for any previous pulse to end
  while (digitalRead(pin) == state)
    if (numloops++ == maxloops)
      return 0;

  // wait for the pulse to start
  while (digitalRead(pin) != state)
    if (numloops++ == maxloops)
      return 0;

  // wait for the pulse to stop
  while (digitalRead(pin) == state) {
    if (numloops++ == maxloops)
      return 0;
    width++;
  }

  // convert the reading to microseconds. The loop has been determined
  // to be 20 clock cycles long and have about 16 clocks between the edge
  // and the start of the loop. There will be some error introduced by
  // the interrupt handlers.
  //Serial.println(clockCyclesToMicroseconds(width * 21 + 16));
  return clockCyclesToMicroseconds(width * 21 + 16);
}

/*
#########################################################################################################
#                                         BH1750 related functions                                      #
#########################################################################################################
*/
int BH1750_Read(int address) {
  int i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) {
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
  return i;
}


void BH1750_Init(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x10);
  Wire.endTransmission();
}


uint16_t BH1750_Main() {
  uint16_t value = 0;
  BH1750_Init(BH1750address);
  // delay(200);
  if (2 == BH1750_Read(BH1750address)) {
    value = ((buff[0] << 8) | buff[1]) / 1.2;
    // Serial.print(value);
    lightIntensityReading[0] = value;
  }
  delay(150);
  BH1750_Init(BH1750address2);
  if (2 == BH1750_Read(BH1750address2)) {
    value = ((buff[0] << 8) | buff[1]) / 1.2;
    lightIntensityReading[1] = value;
  }
}




/*
#########################################################################################################
#                                 light intensity related functions                                     #
#########################################################################################################
*/

void tooDarkOrTooBright() {
  if (millis() - lastGoodLighting_Time > LIGHT_STATE_CHANGE_TIME) {
    lastVeryDarkOrVeryBright_Time = millis();
    Serial.println(" || entered tooDarkOrTooBright state!!");
    buzzer_periodic();
  } else {
    Serial.println(" || entering tooDarkOrTooBright state waiting for threshold time!!");
  }
}


void deltaTooLarge() {
  if (millis() - lastGoodLighting_Time > LIGHT_STATE_CHANGE_TIME) {
    lastDatlaLarge_Time = millis();
    Serial.println(" || entered deltaTooLarge state!!");
    buzzer_constant();
  } else {
    Serial.println(" || entering deltaTooLarge state waiting for threshold time!!");
  }
}

int duelLDR_findMin(int resistorpin1, int resistorpin2) {
  LDR_voltage1 = analogRead(resistorpin1);
  LDR_voltage2 = analogRead(resistorpin2);
  Serial.print("LDR1=");
  Serial.print(LDR_voltage1);
  Serial.print(" LDR2=");
  Serial.print(LDR_voltage2);
  if (LDR_voltage1 > LDR_voltage2) {
    LDR_voltage = LDR_voltage2;
  } else {
    LDR_voltage = LDR_voltage1;
  }
  Serial.print("LDR_all=");
  Serial.println(LDR_voltage);
}



/*
#########################################################################################################
#                                     Distance related Functions                                        #
#########################################################################################################
*/
long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn2(echoPin, HIGH, 300000L);
}

/*
calculating the distance between the glasses to the object, 
we choose the smallest US value and the IR sensor value. Because, 
the IR sensor is in the middle and the two US sensors are on the
side. 
*/
long distCalculate(float US1, float US2, float IR) {
  long US = 0;
  float currentDist = 0;
  if (IR > 100) {
    if (US1 < US2 || US2 == 0) {
      US = US1;
      Serial.print("||");
      Serial.print(US);
      Serial.print(" ");
      Serial.print(US);
      Serial.print("||");
      currentDist = US;
    } else {
      US = US2;
      Serial.print("||");
      Serial.print(US);
      Serial.print(" ");
      Serial.print(US);
      Serial.print("||");
      currentDist = US;
    }

  } else {
    if (US1 < US2 || US2 == 0) {
      US = US1;
      if (US < 40) {
        Serial.print("||");
        Serial.print(US + IR);
        Serial.print(" ");
        Serial.print((US + IR) / 2);
        Serial.print("||");
        currentDist = (US + IR) / 2;
      } else if (40 <= US <= 100) {
        Serial.print("||");
        Serial.print(US);
        Serial.print(" ");
        Serial.print(US);
        Serial.print("||");
        currentDist = US;
      } else {
        Serial.print("||");
        Serial.print(US + IR);
        Serial.print(" ");
        Serial.print((US + IR) / 2);
        Serial.print("||");
        currentDist = (US + IR) / 2;
      }
    } else if (US1 >= US2 || US1 == 0) {
      US = US2;
      if (US < 40) {
        Serial.print("||");
        Serial.print(US + IR);
        Serial.print(" ");
        Serial.print((US + IR) / 2);
        Serial.print("||");
        currentDist = (US + IR) / 2;
      } else if (40 <= US <= 100) {
        Serial.print("||");
        Serial.print(US);
        Serial.print(" ");
        Serial.print(US);
        Serial.print("||");
        currentDist = US;
      } else {
        Serial.print("||");
        Serial.print(US + IR);
        Serial.print(" ");
        Serial.print((US + IR) / 2);
        Serial.print("||");
        currentDist = (US + IR) / 2;
      }
    }
  }

  return currentDist;
}


/*
detecting the distance from the IR1, US1, and US2 sensors
*/
long distDetectionUS(int US1_triggerPin, int US1_echoPin, int US2_triggerPin, int US2_echoPin) {
  cm_US1 = readUltrasonicDistance(US1_triggerPin, US1_echoPin) / 3;
  //write the ping distance to the characteristic:
  Ultrasound_1_Characteristic.writeValue(cm_US1);
  cm_US2 = readUltrasonicDistance(US2_triggerPin, US2_echoPin) / 3;
  //write the ping distance to the characteristic:
  Ultrasound_2_Characteristic.writeValue(cm_US2);
  Serial.print("US1=");
  Serial.print(cm_US1);
  Serial.print(";");
  Serial.print("US2=");
  Serial.print(cm_US2);
  Serial.print(";");

  return cm_US1, cm_US2;
}

long distDetectionIR(int IR1_Pin) {
  ave.push(60 * pow(analogRead(IR1_Pin) * 0.0048828125, -2));
  cm_IR = ave.mean();
  float cmIR_final = 0;

  if (0 < cm_IR <= 5) {
    cmIR_final = cm_IR;
  } else if (5 < cm_IR <= 10) {
    cmIR_final = 2.9 * cm_IR;
  } else if (10 < cm_IR <= 20) {
    cmIR_final = 1.8 * cm_IR;
  } else if (20 < cm_IR < 30) {
    cmIR_final = 1.25 * cm_IR;
  } else if (30 <= cm_IR <= 40) {
    cmIR_final = 0.64 * cm_IR;
  } else {
    cmIR_final = cm_IR;
  }
  Serial.print("IR1=");
  Serial.print(cmIR_final);
  //write the ping distance to the characteristic:
  IRCharacteristic.writeValue(cmIR_final);

  return cmIR_final;
}

/*
#########################################################################################################
#                                         Buzzer State Functions                                        #
#########################################################################################################
*/
void buzzer_constant() {
  // noTone(buzzer);     // Stop sound...
  tone(buzzer, 3000);  // Send 1KHz sound signal...
}

void buzzer_mute() {
  noTone(buzzer);
}


void buzzer_periodic() {
  noTone(buzzer);
  tone(buzzer, 3000);
  delay(200);
  noTone(buzzer);
  delay(200);
}



/*
#########################################################################################################
#                                          Motor State Functions                                        #
#########################################################################################################
*/

/*
Motor No Action state 
*/
bool motorNoAction(float dist) {
  return 1;
}


/*
Motor Vibrate Constantly state  
*/
void motorVibrate(int motorPin) {
  digitalWrite(motorPin, LOW);  //vibrate
}

/*
Motor Vibrate stop 
*/
void motorVibrateStop(int motorPin) {
  digitalWrite(motorPin, HIGH);
}

/*
Motor Vibrate 0.1 sec per sec state 
*/

void motorVibratePeriodic(int motorPin) {
  digitalWrite(motorPin, LOW);   //vibrate
  delay(MOTOR_VIB_TIME);         // delay one second
  digitalWrite(motorPin, HIGH);  //stop vibrating
  delay(1000 - MOTOR_VIB_TIME);  //wait
}

/*
Motor Alert state
*/
bool motorAlert(float dist) {
  return 1;
}

/*
Motor Vibrate Once for 0.1 sec  
*/
void motorVibrateOnce(int motorPin) {
  digitalWrite(motorPin, HIGH);  //stop vibrating
  delay(MOTOR_VIB_TIME);         //
  digitalWrite(motorPin, LOW);   //vibrate
  delay(MOTOR_VIB_TIME);         //
  digitalWrite(motorPin, HIGH);  //stop vibrating
}


/*
#########################################################################################################
#                                       Model State Logic Functions                                     #
#########################################################################################################
*/
/*
To enter the veryClose state: 
  1, must be very close for at least 10 sec
To Leave the veryClose state: 
  1, Enter the Good state: 
    Must be in good distance for at least 5 sec
  2, Enter the close state:
    Must be in the close state for at least 5 sec


To enter the Close state: 
  1, must be close for at lease 10 sec 
To Leave the close state: 
  1, Enter the Good State
     must be in good distance for at least 5 sec
  2, Enter the Very Close state
     must be in very close sate for at least 10 sec 
  3, Enter the close alert state
     must be within the close state for at least 10 minutes
*/
void DistanceLogic(long dist) {
  if (dist < 10) {
    if (millis() - lastGood_Time > CHANGE_STATE_TIME) {
      hasBeenVeryClose = 1;
      Serial.print("Very Close Alert!!!");
      motorVibrate(motorPin);
      lastVeryClose_Time = millis();
    } else {
      motorVibrateStop(motorPin);
      Serial.print("Entering Very Close Waiting for threshold time!");
    }
  } else {
    lastGood_Time = millis();
    if (hasBeenVeryClose == 0) {
      Serial.print("Good Now!!!");
      motorVibrateStop(motorPin);
    } else if (millis() - lastVeryClose_Time < MOTOR_VIB2STATIC_TIME && hasBeenVeryClose == 1) {
      Serial.print("Leaving Very Close Waiting for threshold time!");
    } else if (millis() - lastVeryClose_Time > MOTOR_VIB2STATIC_TIME) {
      hasBeenVeryClose = 0;
      motorVibrateStop(motorPin);
      Serial.print("Good Now!!!");
    }
  }
}


/*
The logic when entering the very close state
*/
void veryCloseLogic() {
  if (millis() - lastClose_Time > CHANGE_STATE_TIME && hasBeenClose == 1) {
    hasBeenClose = 0;
  } else if (millis() - lastCloseLongTime_Time > CHANGE_STATE_TIME && hasBeenCloseLongTime == 1) {
    hasBeenCloseLongTime = 0;
  } else if (millis() - lastCloseLongTime_Time < CHANGE_STATE_TIME && hasBeenCloseLongTime == 1) {
    Serial.print("Motor not stop Entering Very Close state Waiting for threshold time!");
    motorVibratePeriodic(motorPin);
  }
  if (millis() - lastGood_Time > CHANGE_STATE_TIME || hasBeenVeryClose == 1) {
    hasBeenVeryClose = 1;
    Serial.print("Very Close Alert!!!");
    DisState_Characteristic.writeValue(2);
    motorVibrate(motorPin);
    lastVeryClose_Time = millis();

  } else {
    // motorVibrateStop(motorPin);
    Serial.print("Entering Very Close state Waiting for threshold time!");
  }
  enteringClose_Time = millis();
}



/*
The logic when entering the close state
*/
void closeLogic() {

  if (millis() - lastVeryClose_Time > CHANGE_STATE_TIME && hasBeenVeryClose == 1) {
    hasBeenVeryClose = 0;
  }

  // now we entered the close state
  if (hasBeenClose == 1) {
    hasBeenClose = 1;
    hasBeenCloseLongTime = 0;

    if (millis() - lastEnteredCloseLongTime_Time < MOTOR_VIB2VIBLOOP_TIME) {
      lastClose_Time = millis();
      Serial.print("Entered Close State!!!!");
    }
    // if the user has been in close state for more than threshold time, vibrate the motor Periodicly
    if (millis() - lastEnteredCloseLongTime_Time > MOTOR_VIB2VIBLOOP_TIME) {
      lastClose_Time = millis();
      hasBeenCloseLongTime = 1;
      lastCloseLongTime_Time = millis();
      motorVibratePeriodic(motorPin);
      Serial.print("Within Close State for too much long time!!!!");
    }
  }
  // first alert the user he/she has entered the close mode
  else if (millis() - enteringClose_Time > CHANGE_STATE_TIME && millis() - enteringClose_Time < CHANGE_STATE_TIME + 300) {
    hasBeenClose = 1;
    hasBeenCloseLongTime = 0;
    lastClose_Time = millis();
    lastEnteredCloseLongTime_Time = millis();
    Serial.print("Entered Close State!!!!");
    motorVibrateOnce(motorPin);
    DisState_Characteristic.writeValue(1);
  }
  // if the user is entering the close state
  else {
    Serial.print("Entering Close state Waiting for threshold time!!!");
  }
  lastGood_Time = millis();
}


/*
The logic when entering the good state
*/
void goodDistLogic() {
  lastGood_Time = millis();
  // dealing with exiting the very close state
  if (hasBeenVeryClose == 0 && hasBeenClose == 0 && hasBeenCloseLongTime == 0) {
    Serial.print("Good Now!!!");
    motorVibrateStop(motorPin);
    DisState_Characteristic.writeValue(0);
  }
  // dealing with exiting the very close state
  else if (millis() - lastVeryClose_Time < MOTOR_VIB2STATIC_TIME && hasBeenVeryClose == 1) {
    Serial.print("Leaving the Very Close state Waiting for threshold time!");
  } else if (millis() - lastVeryClose_Time > MOTOR_VIB2STATIC_TIME && hasBeenVeryClose == 1) {
    hasBeenVeryClose = 0;
    motorVibrateStop(motorPin);
    DisState_Characteristic.writeValue(0);
    Serial.print("Good Now!!!");
  }

  // dealing with exiting the close state
  // when leaving motor close state
  if (millis() - lastClose_Time < MOTOR_VIB2STATIC_TIME && hasBeenClose == 1 && hasBeenCloseLongTime == 0 || millis() - lastCloseLongTime_Time < MOTOR_ALERT2STATIC_TIME && hasBeenClose == 1 && hasBeenCloseLongTime == 1) {
    Serial.print("Leaving the Close State Waiting for threshold time!");
  }
  // when passed threshold pred but not yet in closeLongTime
  else if (millis() - lastClose_Time > MOTOR_VIB2STATIC_TIME && hasBeenClose == 1) {
    hasBeenClose = 0;
    // hasBeenCloseLongTime = 0;
    motorVibrateStop(motorPin);
    DisState_Characteristic.writeValue(0);
    Serial.print("Good Now!!!");
  }
  // when passed threshold pred in closeLongTime
  else if (millis() - lastCloseLongTime_Time > MOTOR_ALERT2STATIC_TIME && hasBeenCloseLongTime == 1) {
    hasBeenClose = 0;
    hasBeenCloseLongTime = 0;
    motorVibrateStop(motorPin);
    DisState_Characteristic.writeValue(0);
    Serial.print("Good Now!!!");
  }
  enteringClose_Time = millis();
}






/*
#########################################################################################################
#                                       Set up                                    #
#########################################################################################################
*/

void setup() {
  Serial.begin(9600);

  if (DEBUG) {
    while (!Serial)
      ;
  }

  /*
Bluetooth Set up
*/

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1)
      ;
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Arduino Nano 33 BLE");
  BLE.setAdvertisedService(nanoService);

  // add the characteristic to the service
  nanoService.addCharacteristic(FinalDistanceCharacteristic);
  nanoService.addCharacteristic(IRCharacteristic);
  nanoService.addCharacteristic(Ultrasound_1_Characteristic);
  nanoService.addCharacteristic(Ultrasound_2_Characteristic);
  nanoService.addCharacteristic(DisState_Characteristic);
  nanoService.addCharacteristic(LightState_Characteristic);
  nanoService.addCharacteristic(LightBH_1_Characteristic);
  nanoService.addCharacteristic(LightBH_2_Characteristic);
  


  // add the service
  BLE.addService(nanoService);

  // set the initial value for the characeristic:
  FinalDistanceCharacteristic.writeValue(0);
  IRCharacteristic.writeValue(0);
  Ultrasound_1_Characteristic.writeValue(0);
  Ultrasound_2_Characteristic.writeValue(0);
  DisState_Characteristic.writeValue(0);
  LightBH_1_Characteristic.writeValue(0);
  LightBH_2_Characteristic.writeValue(0);
  LightState_Characteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  delay(100);
  Serial.println("BLE device active, waiting for connections...");

  /*
Components Set up
*/

  pinMode(motorPin, OUTPUT);
  pinMode(resistorpin1, INPUT);  // put your setup code here, to run once:
  pinMode(resistorpin2, INPUT);
  delay(2000);
  pinMode(buzzer, OUTPUT);  // Set buzzer - pin 9 as an output
}





/*
Main Loop
*/

void loop() {
  // listen for BLE centrals to connect:
  BLEDevice central = BLE.central();
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      duelLDR_findMin(resistorpin1, resistorpin2);
      if (LDR_voltage > 644) {
        BH1750_Main();
        Serial.print("BH1750_1=");
        Serial.print(lightIntensityReading[0]);
        Serial.print(" lux");
        Serial.print("; BH1750_2=");
        Serial.print(lightIntensityReading[1]);
        Serial.print(" lux");
        enviromentLightIntensity = (lightIntensityReading[0] + lightIntensityReading[1]) / 2;
        deltaLightIntensity = abs(lightIntensityReading[0] - lightIntensityReading[1]);
        if (deltaLightIntensity > 1000) {
          deltaTooLarge();
        } else if (enviromentLightIntensity < 200 || enviromentLightIntensity > 1000) {
          tooDarkOrTooBright();
        } else {
          Serial.println(" || Light situation GOOD!!");
          buzzer_mute();
          lastGoodLighting_Time = millis();
        }
        // Add a new random value to the bucket
        Serial.print("***");
        Serial.print("hasBeenVeryClose=");
        Serial.print(hasBeenVeryClose);
        Serial.print("|");
        Serial.print("hasBeenClose=");
        Serial.print(hasBeenClose);
        Serial.print("|");
        Serial.print("hasBeenCloseLongTime=");
        Serial.print(hasBeenCloseLongTime);
        Serial.print("***");
        cm_US1, cm_US2 = distDetectionUS(12, 11, 10, 9);
        cm_IR = distDetectionIR(A2);
        dist = distCalculate(cm_US1, cm_US2, cm_IR);
        FinalDistanceCharacteristic.writeValue(dist);
        Serial.print("calculated_dist=");
        Serial.print(dist);
        Serial.println("cm");

        if (dist < 10) {
          veryCloseLogic();
        }
        if (dist > 10 && dist < 40) {
          closeLogic();
        }

        if (dist > 40) {
          goodDistLogic();
        }

      } else {
        Serial.print("Device NOT PUT ON!!!!!");
        buzzer_mute();
        delay(100);
      }
      //delay(2000);  
    }
    // when the central disconnects, print it out:
    Serial.println("");
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}