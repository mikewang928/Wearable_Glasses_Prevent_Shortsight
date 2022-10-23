const int resistorpin = A1;
int LDR_voltage = 0;
void setup() {
  Serial.begin(9600);
  pinMode(resistorpin, INPUT);// put your setup code here, to run once:

}

void loop() {
  LDR_voltage = analogRead(resistorpin);
  Serial.println(LDR_voltage);
  delay(1000);// put your main code here, to run repeatedly:

}
