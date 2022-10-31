#include <ArduinoBLE.h>

#define DEBUG true
#define trigPin D12
#define echoPin A6
#define clockCyclesPerMicrosecond() ( 260L ) //260 is Clock Cycle of LinkIt ONE in MHz
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

float inches = 0;
float cm = 0;

BLEService nanoService("13012F00-F8C3-4F4A-A8F4-15CD926DA146");  // BLE Service
// BLE Characteristics - custom 128-bit UUID, read and writable by central device
BLEByteCharacteristic UltrasoundCharacteristic("13012F01-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);

//non-blocking pulseIn()
unsigned long pulseIn2(uint8_t pin, uint8_t state, unsigned long timeout) {
  unsigned long width = 0; // keep initialization out of time critical area

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
  return pulseIn2(echoPin, HIGH, 300000L);
}

void setup() {
  Serial.begin(115200);

  if (DEBUG) {
    while (!Serial)
      ;
  }

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
      // measure the ping time in cm
      cm = readUltrasonicDistance(trigPin,echoPin)/3;
      //convert to inches by dividing by 2.54
      inches = (cm / 2.54);
      Serial.print(inches);
      Serial.print(" inches, ");
      Serial.print(cm);
      Serial.println(" cm");
      //write the ping distance to the characteristic:
      UltrasoundCharacteristic.writeValue(cm);
      Serial.println(UltrasoundCharacteristic.value());
      // wait 1 second before taking the next reading:
      delay(1000); // Wait for 1000 millisecond(s)
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
