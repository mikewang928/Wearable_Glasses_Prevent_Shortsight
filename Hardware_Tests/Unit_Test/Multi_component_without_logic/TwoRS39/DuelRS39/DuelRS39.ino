const int resistorpin = A0;
const int resistorpin2 = A1;
int LDR_voltage = 0;
void setup() {
  Serial.begin(9600);
  pinMode(resistorpin, INPUT);// put your setup code here, to run once:
  pinMode(resistorpin, INPUT);

}

void loop() {
  LDR_voltage = analogRead(resistorpin);
  Serial.print("LDR1=");
  Serial.print(LDR_voltage);
  delay(1000);// put your main code here, to run repeatedly:
  LDR_voltage = analogRead(resistorpin2);
  Serial.print("; LDR2=");
  Serial.println(LDR_voltage);
}

