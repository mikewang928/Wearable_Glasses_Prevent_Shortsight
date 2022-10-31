

#include <ArduinoBLE.h>

 #define RED_PIN 22     
 #define BLUE_PIN 24     
 #define GREEN_PIN 23

 #define DEBUG true

// These UUIDs have been randomly generated. - they must match between the Central and Peripheral devices
// Any changes you make here must be suitably made in the Python program as well

BLEService nanoService("13012F00-F8C3-4F4A-A8F4-15CD926DA146"); // BLE Service

// BLE Characteristics - custom 128-bit UUID, read and writable by central device
BLEByteCharacteristic LEDCharacteristic("13012F01-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLEWrite);

void setup() {
    Serial.begin(9600);
    
    if (DEBUG) {
      while(!Serial);
    }
    
    // intitialize the LED Pins as an output
    pinMode(RED_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);

    // Set them to the OFF position to start with
    // The onboard RGB LED has an inverse logic - i.e. HIGH turns it off, and LOW turns in ON
    digitalWrite(RED_PIN, HIGH);         // will turn the LED off
    digitalWrite(GREEN_PIN, HIGH);         // will turn the LED off
    digitalWrite(BLUE_PIN, HIGH);         // will turn the LED off

    // begin initialization
    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1);
    }

    // set advertised local name and service UUID:
    BLE.setLocalName("Arduino Nano 33 BLE");
    BLE.setAdvertisedService(nanoService);

    // add the characteristic to the service
    nanoService.addCharacteristic(LEDCharacteristic);

    // add service
    BLE.addService(nanoService);

    // set the initial value for the characeristic:
    LEDCharacteristic.writeValue(0);

    // start advertising
    BLE.advertise();
    delay(100);
    Serial.println("Arduino Nano BLE LED Peripheral Service Started");
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
            // if the remote device wrote to the characteristic,
            // use the value to control the LED:
            
            if(LEDCharacteristic.written()){
              if (LEDCharacteristic.value()==1){
                Serial.println("RED LED on");
                digitalWrite(RED_PIN, LOW);
                digitalWrite(GREEN_PIN, HIGH);
                digitalWrite(BLUE_PIN, HIGH);
              }
              else if (LEDCharacteristic.value()==2){
                Serial.println("GREEN LED on");
                digitalWrite(RED_PIN, HIGH);
                digitalWrite(GREEN_PIN, LOW);
                digitalWrite(BLUE_PIN, HIGH);
              }
              else if (LEDCharacteristic.value()==3){
                Serial.println("BLUE LED on");
                digitalWrite(RED_PIN, HIGH);
                digitalWrite(GREEN_PIN, HIGH);
                digitalWrite(BLUE_PIN, LOW);
              }
              else if (LEDCharacteristic.value()==12){
                Serial.println("RED&GREEN LED on");
                digitalWrite(RED_PIN, LOW);
                digitalWrite(GREEN_PIN, LOW);
                digitalWrite(BLUE_PIN, HIGH);
              }
              else if (LEDCharacteristic.value()==13){
                Serial.println("RED&BLUE LED on");
                digitalWrite(RED_PIN, LOW);
                digitalWrite(GREEN_PIN, HIGH);
                digitalWrite(BLUE_PIN, LOW);
              }
              else if (LEDCharacteristic.value()==23){
                Serial.println("GREEN&BLUE LED on");
                digitalWrite(GREEN_PIN, LOW);
                digitalWrite(BLUE_PIN, LOW);
                digitalWrite(RED_PIN, HIGH);
              }
              else if (LEDCharacteristic.value()==123){
                Serial.println("RED&GREEN&BLUE LED on");
                digitalWrite(RED_PIN, LOW);
                digitalWrite(GREEN_PIN, LOW);
                digitalWrite(BLUE_PIN, LOW);
              }
              else{
                    Serial.println("All LED off");
                    digitalWrite(RED_PIN, HIGH);
                    digitalWrite(GREEN_PIN, HIGH);
                    digitalWrite(BLUE_PIN, HIGH);
              } 
            }
  
        }

        // when the central disconnects, print it out:
        Serial.print(F("Disconnected from central: "));
        Serial.println(central.address());
    }
}
