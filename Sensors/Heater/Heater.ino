int PIN_SWITCH = 12;

String mode = Serial.readString();
String buttonHeater = Serial.readString();
float Temperature=Serial.parseFloat();
float Target_min=Serial.parseFloat();
float Target_max=Serial.parseFloat();




void setup() {
  pinMode(PIN_SWITCH, OUTPUT);
  Serial.println(mode);
  Serial.begin(9600);
}


void loop() {

  if (Serial.available()){
 
  //automatic
  if(mode=="Automatic_temp"){
      if(Temperature < Target_min){
         digitalWrite(PIN_SWITCH, HIGH);
         Serial.println("Low temperature,switch on heater");
        }
       else if(Temperature > Target_max){
         digitalWrite(PIN_SWITCH, LOW);
         Serial.println("High temperature,switch off heater");
        }
    
    }
    // Manual
    if(mode=="Manual_temp"){
        if (buttonHeater=="HeaterOn"){
            digitalWrite(PIN_SWITCH, HIGH);
            Serial.println("Heater activated manually");
          }
         else if (buttonHeater=="HeaterOn"){
            digitalWrite(PIN_SWITCH, LOW);
            Serial.println("Heater desactivated manually");
          }
      }
      
  }
  delay(600000);
}
