void setup() {
  // put your setup code here, to run once:
  BLE.setLocalName("Nano33BLE"); // Set name for connection 
  BLE.setAdvertisedService(greetingService); // Advertise service 
  greetingService.addCharacteristic(greetingCharacteristic); // Add characteristic to service 
  BLE.addService(greetingService); // Add service 
  greetingCharacteristic.setValue(greeting); // Set greeting string 
  BLE.advertise(); // Start advertising
}

void loop() {
  // put your main code here, to run repeatedly:

}
