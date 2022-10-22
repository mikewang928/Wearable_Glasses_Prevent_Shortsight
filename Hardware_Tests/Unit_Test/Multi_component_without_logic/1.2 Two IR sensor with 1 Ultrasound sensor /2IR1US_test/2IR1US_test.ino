#include <Average.h>

long inches = 0;
long UltraSound_distance = 0;
long IR1_distance = 0;
long IR2_distance = 0; 
// Reserve space for 10 entries in the average bucket.
// Change the type between < and > to change the entire way the library works.
Average<float> ave(5);
Average<float> ave2(5);

// // Reading the distance from the Ultrasonic Sensor
// long readUltrasonicDistance(int triggerPin, int echoPin){
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

// // Reading the distance from the IR Sensor
// long readIRDistance(Average<float> average, int IROutPin){
//   Serial.print("in readIRDistance !");
//   average.push(27.728/0.7*1.3 * pow(map(analogRead(IROutPin), 0, 1023, 0, 5000) / 1000.0, -1.2045));
//   Serial.print("end readIRDistance !");
//   return average.mean();
// }


void setup(){
  Serial.begin(9600);
}
 

void loop(){
  // UltraSound_distance = (0.034 * readUltrasonicDistance(12, 11)) /2;
  // Serial.print("US1=");
  // Serial.print(UltraSound_distance);
  // delay(5);

  ave.push(27.728/0.7*1.3 * pow(map(analogRead(A0), 0, 1023, 0, 5000) / 1000.0, -1.2045));
  Serial.print("; IR1=");
  Serial.print(ave.mean());
  delay(5);
  
  ave2.push(27.728/0.07*1.3 * pow(map(analogRead(A1), 0, 1023, 0, 5000) / 1000.0, -1.2045));
  Serial.print("; IR2=");
  Serial.print(ave2.mean());
  Serial.println("cm");
  delay(100); // Wait for 100 millisecond(s)
}
