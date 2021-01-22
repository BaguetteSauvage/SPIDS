int a = 9;
const int sensorPin = A1;
const float baselineTemp = 20;
int pinNumber;

void setup(){
  pinMode(a,OUTPUT);
  Serial.begin(9600);
  }


void loop(){
  digitalWrite(a,HIGH);

  int sensorVal = analogRead(sensorPin);
    Serial.print("Valor sensor: ");
    Serial.print(sensorVal);
    float voltage=(sensorVal/1024.0)*5.0;
    Serial.print(" , Volts: ");
    Serial.print(voltage);
    Serial.print(" , degrees C: ");
    float temperature = (voltage - 0.5)*100.0;
    Serial.println(temperature);  

 delay(1000);
  }
