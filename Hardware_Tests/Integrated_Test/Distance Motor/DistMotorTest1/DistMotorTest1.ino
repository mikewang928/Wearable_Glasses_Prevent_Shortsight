#include <Average.h>

// Reserve space for 10 entries in the average bucket.
// Change the type between < and > to change the entire way the library works.
Average<float> ave(10);

int motorPin = 2; //motor transistor is connected to pin 3

static unsigned long GOOD_CLOSE_BOUNDRY = 40;
static unsigned long CLOSE_VERYCLOSE_BOUNDRY = 20;
static unsigned long CHANGE_STATE_TIME = 10000; // 10 secs = 10000 miliseconds
static unsigned long MOTOR_VIB2STATIC_TIME = 5000; // 5 secs = 5000 miliseconds
static unsigned long MOTOR_VIB2VIBLOOP_TIME = 600000;  // 10 mins = 600000 miliseconds
static unsigned long MOTOR_ALERT2STATIC_TIME = 120000; // 2 mins = 120000 miliseconds 
static unsigned long MOTOR_VIB_TIME = 100; // 0.1 secs = 100 miliseconds
unsigned long lastTimeGood_Time = millis();
unsigned long lastTimeVeryClose_Time = millis();


long inches = 0;
long inches2 = 0;
long cm_US1 = 0;
long cm_US2 = 0;
long cm_IR = 0;
// long lastTimeGood = 0;
long dist = 0;


/*
Helper functions
*/

long readUltrasonicDistance(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}





/*
calculating the distance between the glasses to the object, 
we choose the smallest US value and the IR sensor value. Because, 
the IR sensor is in the middle and the two US sensors are on the
side. 
*/
long distCalculate(float US1, float US2, float IR1){
  long US = 0; 
  if(US1 > US2){
    US = US2;
  }
  else{
    US = US1; 
  }
  // Serial.println(US);
  Serial.print("||");
  Serial.print(US+IR1);
  Serial.print(" ");
  Serial.print((US+IR1)/2);
  Serial.print("||");
  float currentDist = (US+IR1)/2;
  return currentDist;
}


long distDetection(int US1_triggerPin, int US1_echoPin, int US2_triggerPin, int US2_echoPin, int IR1_Pin){
  // ave.push(27.728/0.7*1.3 * pow(map(analogRead(IR1_Pin), 0, 1023, 0, 5000) / 1000.0, -1.2045));
  ave.push(30*pow(analogRead(IR1_Pin)*0.0048828125, -2));
  // Serial.print("IR1=");
  cm_IR = ave.mean();
  // Serial.print(cm_IR);
  // delay(5);
  cm_US1 = (0.034 * readUltrasonicDistance(US1_triggerPin, US1_echoPin)) /2;
  // Serial.print("cm, Ultra_1=");
  // Serial.print(cm_US1);
  // delay(5);
  cm_US2 = (0.034 * readUltrasonicDistance(US2_triggerPin, US2_echoPin)) /2;
  // Serial.print("cm, Ultra_2=");
  // Serial.print(cm_US2);
  return cm_US1, cm_US2, cm_IR;
}


/*
Motor No Action state 
*/
bool motorNoAction(float dist){
  return 1;
}


/*
Motor Vibrate Constantly state  
*/
void motorVibrate(int motorPin){
  digitalWrite(motorPin, HIGH); //vibrate
  // delay(1000);
  // digitalWrite(motorPin, LOW);
}

void motorVibrateStop(int motorPin){
  digitalWrite(motorPin, LOW);
  // delay(MOTOR_VIB2STATIC_TIME);
}

/*
Motor Vibrate 0.1 sec per sec state 
*/
void motorVibratePeriodic(int motorPin){
  digitalWrite(motorPin, HIGH); //vibrate
  delay(MOTOR_VIB_TIME);  // delay one second
  digitalWrite(motorPin, LOW);  //stop vibrating
  delay(1000-MOTOR_VIB_TIME); //wait 50 seconds. 
}

/*
Motor Alert state
*/
bool motorAlert(float dist){
  return 1; 
}


/*
Motor Vibrate Once for 0.1 sec  
*/
void motorVibrateOnce(int motorPin){
  digitalWrite(motorPin, HIGH); //vibrate
  delay(MOTOR_VIB_TIME);  // delay one second
  digitalWrite(motorPin, LOW);  //stop vibrating
}




/*
Basic Set up
*/
void setup(){
  Serial.begin(9600);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

}
 

/*
Main Loop
*/

void loop(){
  // Add a new random value to the bucket
  cm_US1, cm_US2, cm_IR = distDetection(12,11,10,9,A0);
  // Serial.print("cm");
  dist = distCalculate(cm_US1, cm_US2, cm_IR);
  Serial.print("cm, calculated_dist=");
  Serial.print(dist);
  Serial.println("cm");

  
  if (dist < 10)
  {
    // Present
    if (millis() - lastTimeGood_Time > CHANGE_STATE_TIME){
        Serial.print("Very Close!!!");
        lastTimeVeryClose_Time = millis();
    }
    else{
      Serial.print("Motor Stopped!");
    }
  }
  else{
    // Absent
    // Serial.print("Motor Stopped!");
    lastTimeGood_Time = millis();
    if (millis() - lastTimeVeryClose_Time < MOTOR_VIB2STATIC_TIME){
      Serial.print("Very Close!!!");
    }
    else if (millis() - lastTimeVeryClose_Time > MOTOR_VIB2STATIC_TIME){
      Serial.print("Good Now!!!");
    }
  }


  // // Entering the Very Close State
  // if(dist<10){
  //   Serial.print("Very Close!!!");
  //   // motorVibrate(motorPin);
  // }
  // else{
  //   Serial.print("Motor Stopped!");
  //   // motorVibrateStop(motorPin);
  // }


  // delay(10); // Wait for 100 millisecond(s)
}