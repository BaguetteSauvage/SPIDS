
const int SensorPin=A0;
int SensorValue=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put you0r main code here, to run repeatedly:
  float acc = 0;
  for (int i = 0; i < 300; i++) {
    acc = acc +analogRead(SensorPin);
  }
  SensorValue= acc / 300;
  delay(1000);
  Serial.print("Lux : ");
  Serial.println(SensorValue*0.5279+2.386);
}
