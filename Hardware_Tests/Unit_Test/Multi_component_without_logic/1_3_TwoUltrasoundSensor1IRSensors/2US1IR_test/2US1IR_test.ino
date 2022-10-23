#include <Average.h>

// Reserve space for 10 entries in the average bucket.
// Change the type between < and > to change the entire way the library works.
Average<float> ave(10);

long inches = 0;
long inches2 = 0;
long cm = 0;
long cm2 = 0;
 
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



void setup(){
  Serial.begin(9600);

}
 

void loop(){
  // Add a new random value to the bucket
  ave.push(27.728/0.7*1.3 * pow(map(analogRead(A0), 0, 1023, 0, 5000) / 1000.0, -1.2045));
  Serial.print("IR1=");
  Serial.print(ave.mean());
  delay(5);
  cm = (0.034 * readUltrasonicDistance(12, 11)) /2;
  Serial.print("cm, Ultra_1=");
  Serial.print(cm);
  delay(5);
  cm2 = (0.034 * readUltrasonicDistance(10, 9)) /2;
  Serial.print("cm, Ultra_2=");
  Serial.print(cm2);
  Serial.println("cm");
  delay(100); // Wait for 100 millisecond(s)
}
