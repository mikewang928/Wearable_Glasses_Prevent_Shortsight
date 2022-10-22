// int inches = 0;
// int cm = 0;
 
// byte readUltrasonicDistance(int triggerPin, int echoPin){
//   pinMode(triggerPin, OUTPUT);  // Clear the trigger
//   digitalWrite(triggerPin, LOW);
//   delayMicroseconds(2);
//   // Sets the trigger pin to HIGH state for 10 microseconds
//   digitalWrite(triggerPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(triggerPin, LOW);
//   pinMode(echoPin, INPUT);
//   // Reads the echo pin, and returns the sound wave travel time in microseconds
//   return pulseIn(echoPin, HIGH);
// }



// void setup(){
//   Serial.begin(9600);
// }
 

// void loop(){
//   // measure the ping time in cm
//   // cm = 0.01723 * readUltrasonicDistance(12, 11);
//   // Serial.println(readUltrasonicDistance(12, 11));
//   // cm = readUltrasonicDistance(7, A0);
//   // convert to inches by dividing by 2.54
//   cm = (0.034 * readUltrasonicDistance(12, 11)) /2;
//   inches = (cm / 2.54);
//   Serial.print(inches);
//   Serial.print("in, ");
//   Serial.print(cm);
//   Serial.println("cm");
//   delay(100); // Wait for 100 millisecond(s)
// }



#include <Arduino.h>
const int TriggerPin = 12;
const int EchoPin    = 11;
long  Duration       = 0;
int   Delay_timer    = 100; 


void setup() {
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.begin(9600);
}

long Distance(long time){
  long DistanceCalc;
  DistanceCalc = ((time * 0.034) / 2); // centimeters
  return DistanceCalc;
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TriggerPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  Duration = pulseIn(EchoPin,HIGH);
  long Distance_cm = Distance(Duration); 
  Serial.print("Distance = "); 
  Serial.print(Distance_cm); 
  Serial.println (" cm");
  delay(Delay_timer); 
}
