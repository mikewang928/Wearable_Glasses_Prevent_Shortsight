#include <ArduinoBLE.h>
#include <SoftwareSerial.h>

#define DEBUG true
#define trigPin D12
#define echoPin A6
#define Software_TX TX
#define Software_RX RX

SoftwareSerial mySerial(Software_TX, Software_RX);

BLEService nanoService("13012F00-F8C3-4F4A-A8F4-15CD926DA146");  // BLE Service
// BLE Characteristics - custom 128-bit UUID, read and writable by central device
BLEByteCharacteristic UltrasoundCharacteristic("13012F01-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);

float inches = 0;
float cm = 0;
const int bluetooth_baud = 115200;

static uint32_t newPulseIn(uint32_t pin, uint32_t state, uint32_t timeout = 300000L) {
  uint32_t begin = micros();
  // wait for any previous pulse to end
  while (digitalRead(pin)) {
    delay(1);
    if (micros() - begin >= timeout)
    return 0;
  }
  // wait for the pulse to start
  while (!digitalRead(pin)) {
    delay(1);
    if (micros() - begin >= timeout)
    return 0;
  }
  uint32_t pulseBegin = micros();
  // wait for the pulse to stop
  while (digitalRead(pin)) {
    delay(1);
    if (micros() - begin >= timeout)
    return 0; 
  }
  uint32_t pulseEnd = micros();
  Serial.println(pulseEnd - pulseBegin);
  return pulseEnd - pulseBegin;
}


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
  return newPulseIn(echoPin, HIGH);
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(bluetooth_baud);

  if (DEBUG) {
    while (!Serial);
  }

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Arduino Nano 33 BLE");
  BLE.setAdvertisedService(nanoService);

  // add the characteristic to the service
  nanoService.addCharacteristic(UltrasoundCharacteristic);

  // add the service
  BLE.addService(nanoService);

  // set the initial value for the characeristic:
  UltrasoundCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  delay(100);
  Serial.println("BLE device active, waiting for connections...");
}

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
      StaticJsonDocument<200> doc;

      // measure the ping time in cm
      cm = (0.034 * readUltrasonicDistance(trigPin,echoPin))/2;
      //convert to inches by dividing by 2.54
      inches = (cm / 2.54);
      Serial.print(inches);
      Serial.print(" inches, ");
      Serial.print(cm);
      Serial.println(" cm");
      //write the ping distance to the characteristic:
      UltrasoundCharacteristic.writeValue(cm);
      // wait 1 second before taking the next reading:
      delay(1000); // Wait for 1000 millisecond(s)
      if (mySerial.available()){
        dat = mySerial.read();
        delay(100);
        Serial.write(dat);
      }
      if (Serial.available()>0){
        dat = Serial.read();
        delay(100);
        mySerial.write(dat);
      }
    }
        
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}